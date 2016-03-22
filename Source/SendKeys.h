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
    void SendKeyDownUp(const KeyPress key) const;
private:
    static std::mutex m_mtxSending;
};

