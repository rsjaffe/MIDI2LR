#pragma once
#include <mutex>
#include "../JuceLibraryCode/JuceHeader.h"
class SendKeys
{
public:
    SendKeys()
    {};
    ~SendKeys()
    {};
    void handleShortCutKeyDownUp(KeyPress key);
private:
    static std::mutex m_mtxSending;
};

