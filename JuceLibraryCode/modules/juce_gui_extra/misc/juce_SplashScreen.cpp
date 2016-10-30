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

SplashScreen::SplashScreen (const String& title, const Image& image, bool useDropShadow)
    : Component (title),
      backgroundImage (image),
      clickCountToDelete (0)
{
    // You must supply a valid image here!
    jassert (backgroundImage.isValid());

    setOpaque (! backgroundImage.hasAlphaChannel());

   #if JUCE_IOS || JUCE_ANDROID
    const bool useFullScreen = true;
   #else
    const bool useFullScreen = false;
   #endif

    makeVisible (image.getWidth(), image.getHeight(), useDropShadow, useFullScreen);
}

SplashScreen::SplashScreen (const String& title, int width, int height, bool useDropShadow)
    : Component (title),
      clickCountToDelete (0)
{
    makeVisible (width, height, useDropShadow, false);
}

void SplashScreen::makeVisible (int w, int h, bool useDropShadow, bool fullscreen)
{
    clickCountToDelete = Desktop::getInstance().getMouseButtonClickCounter();
    creationTime = Time::getCurrentTime();

    const Rectangle<int> screenSize = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    const int width  = (fullscreen ? screenSize.getWidth()   : w);
    const int height = (fullscreen ? screenSize.getHeight()  : h);

    setAlwaysOnTop (true);
    setVisible (true);
    centreWithSize (width, height);
    addToDesktop (useDropShadow ? ComponentPeer::windowHasDropShadow : 0);

    if (fullscreen)
        getPeer()->setFullScreen (true);

    toFront (false);
}

SplashScreen::~SplashScreen() {}

void SplashScreen::deleteAfterDelay (RelativeTime timeout, bool removeOnMouseClick)
{
    // Note that this method must be safe to call from non-GUI threads
    if (! removeOnMouseClick)
        clickCountToDelete = std::numeric_limits<int>::max();

    minimumVisibleTime = timeout;

    startTimer (50);
}

void SplashScreen::paint (Graphics& g)
{
    g.setOpacity (1.0f);
    g.drawImage (backgroundImage, getLocalBounds().toFloat(), RectanglePlacement (RectanglePlacement::fillDestination));
}

void SplashScreen::timerCallback()
{
    if (Time::getCurrentTime() > creationTime + minimumVisibleTime
         || Desktop::getInstance().getMouseButtonClickCounter() > clickCountToDelete)
        delete this;
}
