#pragma once
#include <mutex>
#include "../JuceLibraryCode/JuceHeader.h"

/**********************************************************************************************//**
* @class   SendKeys
*
* @brief   Send keystrokes to Lightroom.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

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

