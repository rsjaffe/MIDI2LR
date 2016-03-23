#pragma once
#include <mutex>
#include "../JuceLibraryCode/JuceHeader.h"

/**********************************************************************************************//**
 * @class   SendKeys
 *
 * @brief   A send keys.
 *
 * @date    3/22/2016
 **************************************************************************************************/

class SendKeys
{
public:
    SendKeys()
    {};
    ~SendKeys()
    {};
    void SendKeyDownUp(const KeyPress& key) const;
private:
    static std::mutex m_mtxSending;
};

