/*
  ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

  ==============================================================================
*/

void MACAddress::findAllAddresses (Array<MACAddress>& result)
{
    ifaddrs* addrs = nullptr;

    if (getifaddrs (&addrs) == 0)
    {
        for (const ifaddrs* cursor = addrs; cursor != nullptr; cursor = cursor->ifa_next)
        {
            sockaddr_storage* sto = (sockaddr_storage*) cursor->ifa_addr;

            if (sto->ss_family == AF_LINK)
            {
                const sockaddr_dl* const sadd = (const sockaddr_dl*) cursor->ifa_addr;

               #ifndef IFT_ETHER
                enum { IFT_ETHER = 6 };
               #endif

                if (sadd->sdl_type == IFT_ETHER)
                {
                    MACAddress ma (((const uint8*) sadd->sdl_data) + sadd->sdl_nlen);

                    if (! ma.isNull())
                        result.addIfNotAlreadyThere (ma);
                }
            }
        }

        freeifaddrs (addrs);
    }
}

//==============================================================================
bool JUCE_CALLTYPE Process::openEmailWithAttachments (const String& targetEmailAddress,
                                                      const String& emailSubject,
                                                      const String& bodyText,
                                                      const StringArray& filesToAttach)
{
  #if JUCE_IOS
    ignoreUnused (targetEmailAddress, emailSubject, bodyText, filesToAttach);

    //xxx probably need to use MFMailComposeViewController
    jassertfalse;
    return false;
  #else
    JUCE_AUTORELEASEPOOL
    {
        String script;
        script << "tell application \"Mail\"\r\n"
                  "set newMessage to make new outgoing message with properties {subject:\""
               << emailSubject.replace ("\"", "\\\"")
               << "\", content:\""
               << bodyText.replace ("\"", "\\\"")
               << "\" & return & return}\r\n"
                  "tell newMessage\r\n"
                  "set visible to true\r\n"
                  "set sender to \"sdfsdfsdfewf\"\r\n"
                  "make new to recipient at end of to recipients with properties {address:\""
               << targetEmailAddress
               << "\"}\r\n";

        for (int i = 0; i < filesToAttach.size(); ++i)
        {
            script << "tell content\r\n"
                      "make new attachment with properties {file name:\""
                   << filesToAttach[i].replace ("\"", "\\\"")
                   << "\"} at after the last paragraph\r\n"
                      "end tell\r\n";
        }

        script << "end tell\r\n"
                  "end tell\r\n";

        NSAppleScript* s = [[NSAppleScript alloc] initWithSource: juceStringToNS (script)];
        NSDictionary* error = nil;
        const bool ok = [s executeAndReturnError: &error] != nil;
        [s release];

        return ok;
    }
  #endif
}

//==============================================================================
// Unfortunately, we need to have this ugly ifdef here as long as some older OS X versions do not support NSURLSession
#if JUCE_IOS || (defined (__MAC_OS_X_VERSION_MIN_REQUIRED) && defined (__MAC_10_10) && __MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_10)

//==============================================================================
class URLConnectionState   : private Thread
{
public:
    URLConnectionState (NSURLRequest* req, const int maxRedirects)
        : Thread ("http connection"),
          request ([req retain]),
          data ([[NSMutableData data] retain]),
          numRedirectsToFollow (maxRedirects)
    {
        static DelegateClass cls;
        delegate = [cls.createInstance() init];
        DelegateClass::setState (delegate, this);
    }

    ~URLConnectionState()
    {
        signalThreadShouldExit();

        {
            const ScopedLock sl (dataLock);
            isBeingDeleted = true;
            [task cancel];
            DelegateClass::setState (delegate, nullptr);
        }

        stopThread (10000);
        [task release];
        [request release];
        [headers release];
        [session release];

        const ScopedLock sl (dataLock);
        [delegate release];
        [data release];
    }

    bool start (URL::OpenStreamProgressCallback* callback, void* context)
    {
        startThread();

        while (isThreadRunning() && ! initialised)
        {
            if (callback != nullptr)
                if (! callback (context, (int) latestTotalBytes, (int) [[request HTTPBody] length]))
                    return false;

            Thread::sleep (1);
        }

        return true;
    }

    int read (char* dest, int numBytes)
    {
        int numDone = 0;

        while (numBytes > 0)
        {
            const int available = jmin (numBytes, (int) [data length]);

            if (available > 0)
            {
                const ScopedLock sl (dataLock);
                [data getBytes: dest length: (NSUInteger) available];
                [data replaceBytesInRange: NSMakeRange (0, (NSUInteger) available) withBytes: nil length: 0];

                numDone += available;
                numBytes -= available;
                dest += available;
            }
            else
            {
                if (hasFailed || hasFinished)
                    break;

                Thread::sleep (1);
            }
        }

        return numDone;
    }

    void didReceiveResponse (NSURLResponse* response, id completionHandler)
    {
        {
            const ScopedLock sl (dataLock);
            if (isBeingDeleted)
                return;

            [data setLength: 0];
        }

        contentLength = [response expectedContentLength];

        [headers release];
        headers = nil;

        if ([response isKindOfClass: [NSHTTPURLResponse class]])
        {
            auto httpResponse = (NSHTTPURLResponse*) response;
            headers = [[httpResponse allHeaderFields] retain];
            statusCode = (int) [httpResponse statusCode];
        }

        initialised = true;

        if (completionHandler != nil)
        {
            // Need to wrangle this parameter back into an obj-C block,
            // and call it to allow the transfer to continue..
            void (^callbackBlock)(NSURLSessionResponseDisposition) = completionHandler;
            callbackBlock (NSURLSessionResponseAllow);
        }
    }

    void didComplete (NSError* error)
    {
        const ScopedLock sl (dataLock);
        if (isBeingDeleted)
            return;

       #if JUCE_DEBUG
        if (error != nullptr)
            DBG (nsStringToJuce ([error description]));
       #endif

        hasFailed = (error != nullptr);
        initialised = true;
        signalThreadShouldExit();
    }

    void didReceiveData (NSData* newData)
    {
        const ScopedLock sl (dataLock);
        if (isBeingDeleted)
            return;

        [data appendData: newData];
        initialised = true;
    }

    void didSendBodyData (int64_t totalBytesWritten)
    {
        latestTotalBytes = static_cast<int> (totalBytesWritten);
    }

    void willPerformHTTPRedirection (NSURLRequest* urlRequest, void (^completionHandler)(NSURLRequest *))
    {
        {
            const ScopedLock sl (dataLock);
            if (isBeingDeleted)
                return;
        }

        completionHandler (numRedirects++ < numRedirectsToFollow ? urlRequest : nil);
    }

    void run() override
    {
        jassert (task == nil && session == nil);

        session = [[NSURLSession sessionWithConfiguration: [NSURLSessionConfiguration defaultSessionConfiguration]
                                                 delegate: delegate
                                            delegateQueue: [NSOperationQueue currentQueue]] retain];

        task = [session dataTaskWithRequest: request];

        if (task == nil)
            return;

        [task retain];
        [task resume];

        while (! threadShouldExit())
            wait (5);

        hasFinished = true;
        initialised = true;
    }

    int64 contentLength = -1;
    CriticalSection dataLock;
    id delegate = nil;
    NSURLRequest* request = nil;
    NSURLSession* session = nil;
    NSURLSessionTask* task = nil;
    NSMutableData* data = nil;
    NSDictionary* headers = nil;
    int statusCode = 0;
    bool initialised = false, hasFailed = false, hasFinished = false, isBeingDeleted = false;
    const int numRedirectsToFollow;
    int numRedirects = 0;
    int64 latestTotalBytes = 0;

private:
    //==============================================================================
    struct DelegateClass  : public ObjCClass<NSObject>
    {
        DelegateClass()  : ObjCClass<NSObject> ("JUCE_URLDelegate_")
        {
            addIvar<URLConnectionState*> ("state");

            addMethod (@selector (URLSession:dataTask:didReceiveResponse:completionHandler:),
                                                                            didReceiveResponse,        "v@:@@@@");
            addMethod (@selector (URLSession:didBecomeInvalidWithError:),   didBecomeInvalidWithError, "v@:@@");
            addMethod (@selector (URLSession:dataTask:didReceiveData:),     didReceiveData,            "v@:@@@");
            addMethod (@selector (URLSession:task:didSendBodyData:totalBytesSent:totalBytesExpectedToSend:),
                                                                            didSendBodyData,           "v@:@@qqq");
            addMethod (@selector (URLSession:task:willPerformHTTPRedirection:newRequest:completionHandler:),
                                                                            willPerformHTTPRedirection, "v@:@@@@@");

            addMethod (@selector (URLSession:task:didCompleteWithError:), didCompleteWithError,   "v@:@@@");

            registerClass();
        }

        static void setState (id self, URLConnectionState* state)  { object_setInstanceVariable (self, "state", state); }
        static URLConnectionState* getState (id self)              { return getIvar<URLConnectionState*> (self, "state"); }

    private:
        static void didReceiveResponse (id self, SEL, NSURLSession*, NSURLSessionDataTask*, NSURLResponse* response, id completionHandler)
        {
            if (auto state = getState (self)) state->didReceiveResponse (response, completionHandler);
        }

        static void didBecomeInvalidWithError (id self, SEL, NSURLSession*, NSError* error)
        {
            if (auto state = getState (self)) state->didComplete (error);
        }

        static void didReceiveData (id self, SEL, NSURLSession*, NSURLSessionDataTask*, NSData* newData)
        {
            if (auto state = getState (self)) state->didReceiveData (newData);
        }

        static void didSendBodyData (id self, SEL, NSURLSession*, NSURLSessionTask*, int64_t, int64_t totalBytesWritten, int64_t)
        {
            if (auto state = getState (self)) state->didSendBodyData (totalBytesWritten);
        }

        static void willPerformHTTPRedirection (id self, SEL, NSURLSession*, NSURLSessionTask*, NSHTTPURLResponse*,
                                                NSURLRequest* request, void (^completionHandler)(NSURLRequest *))
        {
            if (auto state = getState (self)) state->willPerformHTTPRedirection (request, completionHandler);
        }

        static void didCompleteWithError (id self, SEL, NSURLConnection*, NSURLSessionTask*, NSError* error)
        {
            if (auto state = getState (self)) state->didComplete (error);
        }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (URLConnectionState)
};

//==============================================================================
#else

// This version is only used for backwards-compatibility with older OSX targets,
// so we'll turn off deprecation warnings. This code will be removed at some point
// in the future.

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"

//==============================================================================
class URLConnectionState   : public Thread
{
public:
    URLConnectionState (NSURLRequest* req, const int maxRedirects)
        : Thread ("http connection"),
          contentLength (-1),
          delegate (nil),
          request ([req retain]),
          connection (nil),
          data ([[NSMutableData data] retain]),
          headers (nil),
          statusCode (0),
          initialised (false),
          hasFailed (false),
          hasFinished (false),
          numRedirectsToFollow (maxRedirects),
          numRedirects (0),
          latestTotalBytes (0)
    {
        static DelegateClass cls;
        delegate = [cls.createInstance() init];
        DelegateClass::setState (delegate, this);
    }

    ~URLConnectionState()
    {
        stop();
        [connection release];
        [request release];
        [headers release];
        [delegate release];
        [data release];
    }

    bool start (URL::OpenStreamProgressCallback* callback, void* context)
    {
        startThread();

        while (isThreadRunning() && ! initialised)
        {
            if (callback != nullptr)
                if (! callback (context, latestTotalBytes, (int) [[request HTTPBody] length]))
                    return false;

            Thread::sleep (1);
        }

        return connection != nil && ! hasFailed;
    }

    void stop()
    {
        {
            const ScopedLock sl (dataLock);
            [connection cancel];
        }

        stopThread (10000);
    }

    int read (char* dest, int numBytes)
    {
        int numDone = 0;

        while (numBytes > 0)
        {
            const int available = jmin (numBytes, (int) [data length]);

            if (available > 0)
            {
                const ScopedLock sl (dataLock);
                [data getBytes: dest length: (NSUInteger) available];
                [data replaceBytesInRange: NSMakeRange (0, (NSUInteger) available) withBytes: nil length: 0];

                numDone += available;
                numBytes -= available;
                dest += available;
            }
            else
            {
                if (hasFailed || hasFinished)
                    break;

                Thread::sleep (1);
            }
        }

        return numDone;
    }

    void didReceiveResponse (NSURLResponse* response)
    {
        {
            const ScopedLock sl (dataLock);
            [data setLength: 0];
        }

        contentLength = [response expectedContentLength];

        [headers release];
        headers = nil;

        if ([response isKindOfClass: [NSHTTPURLResponse class]])
        {
            NSHTTPURLResponse* httpResponse = (NSHTTPURLResponse*) response;
            headers = [[httpResponse allHeaderFields] retain];
            statusCode = (int) [httpResponse statusCode];
        }

        initialised = true;
    }

    NSURLRequest* willSendRequest (NSURLRequest* newRequest, NSURLResponse* redirectResponse)
    {
        if (redirectResponse != nullptr)
        {
            if (numRedirects >= numRedirectsToFollow)
                return nil;  // Cancel redirect and allow connection to continue

            ++numRedirects;
        }

        return newRequest;
    }

    void didFailWithError (NSError* error)
    {
        DBG (nsStringToJuce ([error description])); ignoreUnused (error);
        hasFailed = true;
        initialised = true;
        signalThreadShouldExit();
    }

    void didReceiveData (NSData* newData)
    {
        const ScopedLock sl (dataLock);
        [data appendData: newData];
        initialised = true;
    }

    void didSendBodyData (NSInteger totalBytesWritten, NSInteger /*totalBytesExpected*/)
    {
        latestTotalBytes = static_cast<int> (totalBytesWritten);
    }

    void finishedLoading()
    {
        hasFinished = true;
        initialised = true;
        signalThreadShouldExit();
    }

    void run() override
    {
        connection = [[NSURLConnection alloc] initWithRequest: request
                                                     delegate: delegate];
        while (! threadShouldExit())
        {
            JUCE_AUTORELEASEPOOL
            {
                [[NSRunLoop currentRunLoop] runUntilDate: [NSDate dateWithTimeIntervalSinceNow: 0.01]];
            }
        }
    }

    int64 contentLength;
    CriticalSection dataLock;
    NSObject* delegate;
    NSURLRequest* request;
    NSURLConnection* connection;
    NSMutableData* data;
    NSDictionary* headers;
    int statusCode;
    bool initialised, hasFailed, hasFinished;
    const int numRedirectsToFollow;
    int numRedirects;
    int latestTotalBytes;

private:
    //==============================================================================
    struct DelegateClass  : public ObjCClass<NSObject>
    {
        DelegateClass()  : ObjCClass<NSObject> ("JUCEAppDelegate_")
        {
            addIvar<URLConnectionState*> ("state");

            addMethod (@selector (connection:didReceiveResponse:), didReceiveResponse,            "v@:@@");
            addMethod (@selector (connection:didFailWithError:),   didFailWithError,              "v@:@@");
            addMethod (@selector (connection:didReceiveData:),     didReceiveData,                "v@:@@");
            addMethod (@selector (connection:didSendBodyData:totalBytesWritten:totalBytesExpectedToWrite:),
                                                                   connectionDidSendBodyData,     "v@:@iii");
            addMethod (@selector (connectionDidFinishLoading:),    connectionDidFinishLoading,    "v@:@");
            addMethod (@selector (connection:willSendRequest:redirectResponse:), willSendRequest, "@@:@@@");

            registerClass();
        }

        static void setState (id self, URLConnectionState* state)  { object_setInstanceVariable (self, "state", state); }
        static URLConnectionState* getState (id self)              { return getIvar<URLConnectionState*> (self, "state"); }

    private:
        static void didReceiveResponse (id self, SEL, NSURLConnection*, NSURLResponse* response)
        {
            getState (self)->didReceiveResponse (response);
        }

        static void didFailWithError (id self, SEL, NSURLConnection*, NSError* error)
        {
            getState (self)->didFailWithError (error);
        }

        static void didReceiveData (id self, SEL, NSURLConnection*, NSData* newData)
        {
            getState (self)->didReceiveData (newData);
        }

        static NSURLRequest* willSendRequest (id self, SEL, NSURLConnection*, NSURLRequest* request, NSURLResponse* response)
        {
            return getState (self)->willSendRequest (request, response);
        }

        static void connectionDidSendBodyData (id self, SEL, NSURLConnection*, NSInteger, NSInteger totalBytesWritten, NSInteger totalBytesExpected)
        {
            getState (self)->didSendBodyData (totalBytesWritten, totalBytesExpected);
        }

        static void connectionDidFinishLoading (id self, SEL, NSURLConnection*)
        {
            getState (self)->finishedLoading();
        }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (URLConnectionState)
};

#pragma clang diagnostic pop

#endif


//==============================================================================
class WebInputStream  : public InputStream
{
public:
    WebInputStream (const String& address_, bool isPost_, const MemoryBlock& postData_,
                    URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext,
                    const String& headers_, int timeOutMs_, StringPairArray* responseHeaders,
                    const int numRedirectsToFollow_, const String& httpRequestCmd_)
      : statusCode (0), address (address_), headers (headers_), postData (postData_), position (0),
        finished (false), isPost (isPost_), timeOutMs (timeOutMs_),
        numRedirectsToFollow (numRedirectsToFollow_), httpRequestCmd (httpRequestCmd_)
    {
        JUCE_AUTORELEASEPOOL
        {
            createConnection (progressCallback, progressCallbackContext);

            if (connection != nullptr && connection->headers != nil)
            {
                statusCode = connection->statusCode;

                if (responseHeaders != nullptr)
                {
                    NSEnumerator* enumerator = [connection->headers keyEnumerator];

                    while (NSString* key = [enumerator nextObject])
                        responseHeaders->set (nsStringToJuce (key),
                                              nsStringToJuce ((NSString*) [connection->headers objectForKey: key]));
                }
            }
        }
    }

    ~WebInputStream()
    {
        connection = nullptr;
    }

    //==============================================================================
    bool isError() const                { return (connection == nullptr || connection->headers == nullptr); }
    int64 getTotalLength() override     { return connection == nullptr ? -1 : connection->contentLength; }
    bool isExhausted() override         { return finished; }
    int64 getPosition() override        { return position; }

    int read (void* buffer, int bytesToRead) override
    {
        jassert (buffer != nullptr && bytesToRead >= 0);

        if (finished || isError())
            return 0;

        JUCE_AUTORELEASEPOOL
        {
            const int bytesRead = connection->read (static_cast<char*> (buffer), bytesToRead);
            position += bytesRead;

            if (bytesRead == 0)
                finished = true;

            return bytesRead;
        }
    }

    bool setPosition (int64 wantedPos) override
    {
        if (wantedPos != position)
        {
            finished = false;

            if (wantedPos < position)
            {
                connection = nullptr;
                position = 0;
                createConnection (0, 0);
            }

            skipNextBytes (wantedPos - position);
        }

        return true;
    }

    int statusCode;

private:
    ScopedPointer<URLConnectionState> connection;
    String address, headers;
    MemoryBlock postData;
    int64 position;
    bool finished;
    const bool isPost;
    const int timeOutMs;
    const int numRedirectsToFollow;
    String httpRequestCmd;

    void createConnection (URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext)
    {
        jassert (connection == nullptr);

        if (NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL: [NSURL URLWithString: juceStringToNS (address)]
                                                               cachePolicy: NSURLRequestReloadIgnoringLocalCacheData
                                                           timeoutInterval: timeOutMs <= 0 ? 60.0 : (timeOutMs / 1000.0)])
        {
            [req setHTTPMethod: [NSString stringWithUTF8String: httpRequestCmd.toRawUTF8()]];

            StringArray headerLines;
            headerLines.addLines (headers);
            headerLines.removeEmptyStrings (true);

            for (int i = 0; i < headerLines.size(); ++i)
            {
                String key   = headerLines[i].upToFirstOccurrenceOf (":", false, false).trim();
                String value = headerLines[i].fromFirstOccurrenceOf (":", false, false).trim();

                if (key.isNotEmpty() && value.isNotEmpty())
                    [req addValue: juceStringToNS (value) forHTTPHeaderField: juceStringToNS (key)];
            }

            if (isPost && postData.getSize() > 0)
                [req setHTTPBody: [NSData dataWithBytes: postData.getData()
                                                 length: postData.getSize()]];

            connection = new URLConnectionState (req, numRedirectsToFollow);

            if (! connection->start (progressCallback, progressCallbackContext))
                connection = nullptr;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WebInputStream)
};
