#include "SendKeys.h"
#ifdef _WIN32
#include "Windows.h"
#else
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#endif

std::mutex SendKeys::m_mtxSending{};

void SendKeys::handleShortCutKeyDownUp(KeyPress key)
{
    std::lock_guard< std::mutex > lock(SendKeys::m_mtxSending);
#ifdef _WIN32
    //Lightroom handle
    const HWND hLRWnd = ::FindWindow(NULL, "Lightroom");
    const ModifierKeys mk = key.getModifiers();
    HKL languageID;
    // Bring Lightroom to foreground if it isn't already there
    if (hLRWnd)
    {
        ::SetForegroundWindow(hLRWnd);
        // get language that LR is using (if hLrWnd is found)
        DWORD threadId = GetWindowThreadProcessId(hLRWnd, NULL);
        languageID = GetKeyboardLayout(threadId);
    }
    else
    {   // use keyboard of MIDI2LR app
        languageID = GetKeyboardLayout(0);
    }
    // Translate key code to keyboard-dependent scan code
    const SHORT vkCodeAndShift = VkKeyScanExW(static_cast<WCHAR>(key.getKeyCode()), languageID);
    // input event.
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = 0; // 0 for key press
    ip.ki.time = 0;
    ip.ki.wScan = 0;
    if (mk.isCtrlDown() || (vkCodeAndShift & 0x200))
    {
        ip.ki.wVk = VK_CONTROL;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isShiftDown() || (vkCodeAndShift & 0x100))
    {
        ip.ki.wVk = VK_SHIFT;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isAltDown() || (vkCodeAndShift & 0x400))
    {
        ip.ki.wVk = VK_MENU;
        SendInput(1, &ip, sizeof(INPUT));
    }
    //press the key
    ip.ki.wVk = static_cast<WORD>(vkCodeAndShift & 0xFF);
    SendInput(1, &ip, sizeof(INPUT));
    //add 30 msec between press and release
    Sleep(30);
    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
    if (mk.isCtrlDown() || (vkCodeAndShift & 0x200))
    {
        ip.ki.wVk = VK_CONTROL;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isShiftDown() || (vkCodeAndShift & 0x100))
    {
        ip.ki.wVk = VK_SHIFT;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isAltDown() || (vkCodeAndShift & 0x400))
    {
        ip.ki.wVk = VK_MENU;
        SendInput(1, &ip, sizeof(INPUT));
    }
#else
    const ModifierKeys mk = key.getModifiers();
    const UniChar KeyCode = static_cast<UniChar>(key.getKeyCode());
    const CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
    CGEventRef d = CGEventCreateKeyboardEvent(source, 0, true);
    CGEventRef u = CGEventCreateKeyboardEvent(source, 0, false);
    CGEventKeyboardSetUnicodeString(d, 1, &KeyCode);
    CGEventKeyboardSetUnicodeString(u, 1, &KeyCode);
    uint64_t flags = UINT64_C(0);
    if (mk.isCommandDown()) flags |= kCGEventFlagMaskCommand;
    if (mk.isAltDown()) flags |= kCGEventFlagMaskAlternate;
    if (mk.isShiftDown()) flags |= kCGEventFlagMaskShift;
    if (flags != UINT64_C(0))
    {
        flags |= CGEventGetFlags(d); //in case KeyCode has associated flag
        CGEventSetFlags(d, static_cast<CGEventFlags>(flags));
        CGEventSetFlags(u, static_cast<CGEventFlags>(flags));
    }

    CGEventPost(kCGHIDEventTap, d);
    CGEventPost(kCGHIDEventTap, u);

    CFRelease(d);
    CFRelease(u);
    CFRelease(source);
#endif
}

