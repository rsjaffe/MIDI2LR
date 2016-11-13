/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#if JUCE_ALSA

// You can define these strings in your app if you want to override the default names:
#ifndef JUCE_ALSA_MIDI_INPUT_NAME
 #define JUCE_ALSA_MIDI_INPUT_NAME  "Juce Midi Input"
#endif

#ifndef JUCE_ALSA_MIDI_OUTPUT_NAME
 #define JUCE_ALSA_MIDI_OUTPUT_NAME "Juce Midi Output"
#endif

//==============================================================================
namespace
{

class AlsaPortAndCallback;

//==============================================================================
class AlsaClient : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<AlsaClient> Ptr;

    static Ptr getInstance (bool forInput)
    {
        AlsaClient*& instance = (forInput ? inInstance : outInstance);
        if (instance == nullptr)
            instance = new AlsaClient (forInput);

        return instance;
    }

    bool isInput() const noexcept    { return input; }

    void registerCallback (AlsaPortAndCallback* cb)
    {
        if (cb != nullptr)
        {
            {
                const ScopedLock sl (callbackLock);
                activeCallbacks.add (cb);

                if (inputThread == nullptr)
                    inputThread = new MidiInputThread (*this);
            }

            inputThread->startThread();
        }
    }

    void unregisterCallback (AlsaPortAndCallback* cb)
    {
        const ScopedLock sl (callbackLock);

        jassert (activeCallbacks.contains (cb));
        activeCallbacks.removeAllInstancesOf (cb);

        if (activeCallbacks.size() == 0 && inputThread->isThreadRunning())
            inputThread->signalThreadShouldExit();
    }

    void handleIncomingMidiMessage (snd_seq_event*, const MidiMessage&);
    void handlePartialSysexMessage (snd_seq_event*, const uint8*, int, double);

    snd_seq_t* get() const noexcept     { return handle; }

private:
    bool input;
    snd_seq_t* handle;

    Array<AlsaPortAndCallback*> activeCallbacks;
    CriticalSection callbackLock;

    static AlsaClient* inInstance;
    static AlsaClient* outInstance;

    //==============================================================================
    friend class ReferenceCountedObjectPtr<AlsaClient>;
    friend struct ContainerDeletePolicy<AlsaClient>;

    AlsaClient (bool forInput)
        : input (forInput), handle (nullptr)
    {
        AlsaClient*& instance = (input ? inInstance : outInstance);
        jassert (instance == nullptr);

        instance = this;

        snd_seq_open (&handle, "default", forInput ? SND_SEQ_OPEN_INPUT
                      : SND_SEQ_OPEN_OUTPUT, 0);

        snd_seq_set_client_name (handle, forInput ? JUCE_ALSA_MIDI_INPUT_NAME
                                 : JUCE_ALSA_MIDI_OUTPUT_NAME);
    }

    ~AlsaClient()
    {
        AlsaClient*& instance = (input ? inInstance : outInstance);
        jassert (instance != nullptr);

        instance = nullptr;

        if (handle != nullptr)
        {
            snd_seq_close (handle);
            handle = nullptr;
        }

        jassert (activeCallbacks.size() == 0);

        if (inputThread)
        {
            inputThread->stopThread (3000);
            inputThread = nullptr;
        }
    }

    //==============================================================================
    class MidiInputThread   : public Thread
    {
    public:
        MidiInputThread (AlsaClient& c)
            : Thread ("Juce MIDI Input"), client (c), concatenator (2048)
        {
            jassert (client.input && client.get() != nullptr);
        }

        void run() override
        {
            const int maxEventSize = 16 * 1024;
            snd_midi_event_t* midiParser;
            snd_seq_t* seqHandle = client.get();

            if (snd_midi_event_new (maxEventSize, &midiParser) >= 0)
            {
                const int numPfds = snd_seq_poll_descriptors_count (seqHandle, POLLIN);
                HeapBlock<pollfd> pfd ((size_t) numPfds);
                snd_seq_poll_descriptors (seqHandle, pfd, (unsigned int) numPfds, POLLIN);

                HeapBlock<uint8> buffer (maxEventSize);

                while (! threadShouldExit())
                {
                    if (poll (pfd, (nfds_t) numPfds, 100) > 0) // there was a "500" here which is a bit long when we exit the program and have to wait for a timeout on this poll call
                    {
                        if (threadShouldExit())
                            break;

                        snd_seq_nonblock (seqHandle, 1);

                        do
                        {
                            snd_seq_event_t* inputEvent = nullptr;

                            if (snd_seq_event_input (seqHandle, &inputEvent) >= 0)
                            {
                                // xxx what about SYSEXes that are too big for the buffer?
                                const long numBytes = snd_midi_event_decode (midiParser, buffer,
                                                                            maxEventSize, inputEvent);

                                snd_midi_event_reset_decode (midiParser);

                                concatenator.pushMidiData (buffer, (int) numBytes,
                                                           Time::getMillisecondCounter() * 0.001,
                                                           inputEvent, client);

                                snd_seq_free_event (inputEvent);
                            }
                        }
                        while (snd_seq_event_input_pending (seqHandle, 0) > 0);
                    }
                }

                snd_midi_event_free (midiParser);
            }
        };

    private:
        AlsaClient& client;
        MidiDataConcatenator concatenator;
    };

    ScopedPointer<MidiInputThread> inputThread;
};

AlsaClient* AlsaClient::inInstance  = nullptr;
AlsaClient* AlsaClient::outInstance = nullptr;

//==============================================================================
// represents an input or output port of the supplied AlsaClient
class AlsaPort
{
public:
    AlsaPort() noexcept  : portId (-1)  {}
    AlsaPort (const AlsaClient::Ptr& c, int port) noexcept  : client (c), portId (port) {}

    void createPort (const AlsaClient::Ptr& c, const String& name, bool forInput)
    {
        client = c;

        if (snd_seq_t* handle = client->get())
            portId = snd_seq_create_simple_port (handle, name.toUTF8(),
                                                 forInput ? (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE)
                                                          : (SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ),
                                                 SND_SEQ_PORT_TYPE_MIDI_GENERIC);
    }

    void deletePort()
    {
        if (isValid())
        {
            snd_seq_delete_simple_port (client->get(), portId);
            portId = -1;
        }
    }

    void connectWith (int sourceClient, int sourcePort)
    {
        if (client->isInput())
            snd_seq_connect_from (client->get(), portId, sourceClient, sourcePort);
        else
            snd_seq_connect_to (client->get(), portId, sourceClient, sourcePort);
    }

    bool isValid() const noexcept
    {
        return client != nullptr && client->get() != nullptr && portId >= 0;
    }

    AlsaClient::Ptr client;
    int portId;
};

//==============================================================================
class AlsaPortAndCallback
{
public:
    AlsaPortAndCallback (AlsaPort p, MidiInput* in, MidiInputCallback* cb)
        : port (p), midiInput (in), callback (cb), callbackEnabled (false)
    {
    }

    ~AlsaPortAndCallback()
    {
        enableCallback (false);
        port.deletePort();
    }

    void enableCallback (bool enable)
    {
        if (callbackEnabled != enable)
        {
            callbackEnabled = enable;

            if (enable)
                port.client->registerCallback (this);
            else
                port.client->unregisterCallback (this);
        }
    }

    void handleIncomingMidiMessage (const MidiMessage& message) const
    {
        callback->handleIncomingMidiMessage (midiInput, message);
    }

    void handlePartialSysexMessage (const uint8* messageData, int numBytesSoFar, double timeStamp)
    {
        callback->handlePartialSysexMessage (midiInput, messageData, numBytesSoFar, timeStamp);
    }

private:
    AlsaPort port;
    MidiInput* midiInput;
    MidiInputCallback* callback;
    bool callbackEnabled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlsaPortAndCallback)
};

void AlsaClient::handleIncomingMidiMessage (snd_seq_event_t* event, const MidiMessage& message)
{
    const ScopedLock sl (callbackLock);

    if (AlsaPortAndCallback* const cb = activeCallbacks[event->dest.port])
        cb->handleIncomingMidiMessage (message);
}

void AlsaClient::handlePartialSysexMessage (snd_seq_event* event, const uint8* messageData, int numBytesSoFar, double timeStamp)
{
    const ScopedLock sl (callbackLock);

    if (AlsaPortAndCallback* const cb = activeCallbacks[event->dest.port])
        cb->handlePartialSysexMessage (messageData, numBytesSoFar, timeStamp);
}

//==============================================================================
static AlsaPort iterateMidiClient (const AlsaClient::Ptr& seq,
                                   snd_seq_client_info_t* clientInfo,
                                   const bool forInput,
                                   StringArray& deviceNamesFound,
                                   const int deviceIndexToOpen)
{
    AlsaPort port;

    snd_seq_t* seqHandle = seq->get();
    snd_seq_port_info_t* portInfo = nullptr;

    if (snd_seq_port_info_malloc (&portInfo) == 0)
    {
        int numPorts = snd_seq_client_info_get_num_ports (clientInfo);
        const int client = snd_seq_client_info_get_client (clientInfo);

        snd_seq_port_info_set_client (portInfo, client);
        snd_seq_port_info_set_port (portInfo, -1);

        while (--numPorts >= 0)
        {
            if (snd_seq_query_next_port (seqHandle, portInfo) == 0
                && (snd_seq_port_info_get_capability (portInfo) & (forInput ? SND_SEQ_PORT_CAP_READ
                                                                            : SND_SEQ_PORT_CAP_WRITE)) != 0)
            {
                const String clientName = snd_seq_client_info_get_name (clientInfo);
                const String portName = snd_seq_port_info_get_name(portInfo);

                if (clientName == portName)
                  deviceNamesFound.add (clientName);
                else
                  deviceNamesFound.add (clientName + ": " + portName);

                if (deviceNamesFound.size() == deviceIndexToOpen + 1)
                {
                    const int sourcePort   = snd_seq_port_info_get_port (portInfo);

                    if (sourcePort != -1)
                    {
                        const int sourceClient = snd_seq_client_info_get_client (clientInfo);

                        port.createPort (seq, portName, forInput);
                        port.connectWith (sourceClient, sourcePort);
                    }
                }
            }
        }

        snd_seq_port_info_free (portInfo);
    }

    return port;
}

static AlsaPort iterateMidiDevices (const bool forInput,
                                    StringArray& deviceNamesFound,
                                    const int deviceIndexToOpen)
{
    AlsaPort port;
    const AlsaClient::Ptr client (AlsaClient::getInstance (forInput));

    if (snd_seq_t* const seqHandle = client->get())
    {
        snd_seq_system_info_t* systemInfo = nullptr;
        snd_seq_client_info_t* clientInfo = nullptr;

        if (snd_seq_system_info_malloc (&systemInfo) == 0)
        {
            if (snd_seq_system_info (seqHandle, systemInfo) == 0
                 && snd_seq_client_info_malloc (&clientInfo) == 0)
            {
                int numClients = snd_seq_system_info_get_cur_clients (systemInfo);

                while (--numClients >= 0 && ! port.isValid())
                    if (snd_seq_query_next_client (seqHandle, clientInfo) == 0)
                        port = iterateMidiClient (client, clientInfo, forInput,
                                                  deviceNamesFound, deviceIndexToOpen);

                snd_seq_client_info_free (clientInfo);
            }

            snd_seq_system_info_free (systemInfo);
        }

    }

    deviceNamesFound.appendNumbersToDuplicates (true, true);

    return port;
}


//==============================================================================
class MidiOutputDevice
{
public:
    MidiOutputDevice (MidiOutput* const output, const AlsaPort& p)
        : midiOutput (output), port (p),
          maxEventSize (16 * 1024)
    {
        jassert (port.isValid() && midiOutput != nullptr);
        snd_midi_event_new ((size_t) maxEventSize, &midiParser);
    }

    ~MidiOutputDevice()
    {
        snd_midi_event_free (midiParser);
        port.deletePort();
    }

    bool sendMessageNow (const MidiMessage& message)
    {
        if (message.getRawDataSize() > maxEventSize)
        {
            maxEventSize = message.getRawDataSize();
            snd_midi_event_free (midiParser);
            snd_midi_event_new ((size_t) maxEventSize, &midiParser);
        }

        snd_seq_event_t event;
        snd_seq_ev_clear (&event);

        long numBytes = (long) message.getRawDataSize();
        const uint8* data = message.getRawData();

        snd_seq_t* seqHandle = port.client->get();
        bool success = true;

        while (numBytes > 0)
        {
            const long numSent = snd_midi_event_encode (midiParser, data, numBytes, &event);

            if (numSent <= 0)
            {
                success = numSent == 0;
                break;
            }

            numBytes -= numSent;
            data += numSent;

            snd_seq_ev_set_source (&event, port.portId);
            snd_seq_ev_set_subs (&event);
            snd_seq_ev_set_direct (&event);

            if (snd_seq_event_output_direct (seqHandle, &event) < 0)
            {
                success = false;
                break;
            }
        }

        snd_midi_event_reset_encode (midiParser);
        return success;
    }

private:
    MidiOutput* const midiOutput;
    AlsaPort port;
    snd_midi_event_t* midiParser;
    int maxEventSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiOutputDevice)
};

} // namespace

StringArray MidiOutput::getDevices()
{
    StringArray devices;
    iterateMidiDevices (false, devices, -1);
    return devices;
}

int MidiOutput::getDefaultDeviceIndex()
{
    return 0;
}

MidiOutput* MidiOutput::openDevice (int deviceIndex)
{
    MidiOutput* newDevice = nullptr;

    StringArray devices;
    AlsaPort port (iterateMidiDevices (false, devices, deviceIndex));

    if (port.isValid())
    {
        newDevice = new MidiOutput (devices [deviceIndex]);
        newDevice->internal = new MidiOutputDevice (newDevice, port);
    }

    return newDevice;
}

MidiOutput* MidiOutput::createNewDevice (const String& deviceName)
{
    MidiOutput* newDevice = nullptr;
    AlsaPort port;

    const AlsaClient::Ptr client (AlsaClient::getInstance (false));

    port.createPort (client, deviceName, false);

    if (port.isValid())
    {
        newDevice = new MidiOutput (deviceName);
        newDevice->internal = new MidiOutputDevice (newDevice, port);
    }

    return newDevice;
}

MidiOutput::~MidiOutput()
{
    stopBackgroundThread();

    delete static_cast<MidiOutputDevice*> (internal);
}

void MidiOutput::sendMessageNow (const MidiMessage& message)
{
    static_cast<MidiOutputDevice*> (internal)->sendMessageNow (message);
}

//==============================================================================
MidiInput::MidiInput (const String& nm)
    : name (nm), internal (nullptr)
{
}

MidiInput::~MidiInput()
{
    stop();
    delete static_cast<AlsaPortAndCallback*> (internal);
}

void MidiInput::start()
{
    static_cast<AlsaPortAndCallback*> (internal)->enableCallback (true);
}

void MidiInput::stop()
{
    static_cast<AlsaPortAndCallback*> (internal)->enableCallback (false);
}

int MidiInput::getDefaultDeviceIndex()
{
    return 0;
}

StringArray MidiInput::getDevices()
{
    StringArray devices;
    iterateMidiDevices (true, devices, -1);
    return devices;
}

MidiInput* MidiInput::openDevice (int deviceIndex, MidiInputCallback* callback)
{
    MidiInput* newDevice = nullptr;

    StringArray devices;
    AlsaPort port (iterateMidiDevices (true, devices, deviceIndex));

    if (port.isValid())
    {
        newDevice = new MidiInput (devices [deviceIndex]);
        newDevice->internal = new AlsaPortAndCallback (port, newDevice, callback);
    }

    return newDevice;
}

MidiInput* MidiInput::createNewDevice (const String& deviceName, MidiInputCallback* callback)
{
    MidiInput* newDevice = nullptr;
    AlsaPort port;

    const AlsaClient::Ptr client (AlsaClient::getInstance (true));

    port.createPort (client, deviceName, true);

    if (port.isValid())
    {
        newDevice = new MidiInput (deviceName);
        newDevice->internal = new AlsaPortAndCallback (port, newDevice, callback);
    }

    return newDevice;
}


//==============================================================================
#else

// (These are just stub functions if ALSA is unavailable...)

StringArray MidiOutput::getDevices()                                { return StringArray(); }
int MidiOutput::getDefaultDeviceIndex()                             { return 0; }
MidiOutput* MidiOutput::openDevice (int)                            { return nullptr; }
MidiOutput* MidiOutput::createNewDevice (const String&)             { return nullptr; }
MidiOutput::~MidiOutput()   {}
void MidiOutput::sendMessageNow (const MidiMessage&)    {}

MidiInput::MidiInput (const String& nm) : name (nm), internal (nullptr)  {}
MidiInput::~MidiInput() {}
void MidiInput::start() {}
void MidiInput::stop()  {}
int MidiInput::getDefaultDeviceIndex()      { return 0; }
StringArray MidiInput::getDevices()         { return StringArray(); }
MidiInput* MidiInput::openDevice (int, MidiInputCallback*)                  { return nullptr; }
MidiInput* MidiInput::createNewDevice (const String&, MidiInputCallback*)   { return nullptr; }

#endif
