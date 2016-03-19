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
    const WORD vk = LOBYTE(vkCodeAndShift);
    const WORD vk_modifiers = HIBYTE(vkCodeAndShift);

    // input event.
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = 0; // 0 for key press
    ip.ki.time = 0;
    ip.ki.wScan = 0;
    if ((vk_modifiers & 0x06) == 0x06) //using AltGr key
    {
        ip.ki.wVk = VK_RMENU;
        SendInput(1, &ip, sizeof(INPUT));
        if (mk.isShiftDown() || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        //press the key
        ip.ki.wVk = vk;
        SendInput(1, &ip, sizeof(INPUT));
        //add 30 msec between press and release
        Sleep(30);
        // Release the key
        ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
        SendInput(1, &ip, sizeof(INPUT));
        if (mk.isShiftDown() || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        ip.ki.wVk = VK_RMENU;
        SendInput(1, &ip, sizeof(INPUT));
    }
    else //not using AltGr key
    {
        if (mk.isCtrlDown() || (vk_modifiers & 0x2))
        {
            ip.ki.wVk = VK_CONTROL;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (mk.isShiftDown() || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (mk.isAltDown() || (vk_modifiers & 0x4))
        {
            ip.ki.wVk = VK_MENU;
            SendInput(1, &ip, sizeof(INPUT));
        }
        //press the key
        ip.ki.wVk = vk;
        SendInput(1, &ip, sizeof(INPUT));
        //add 30 msec between press and release
        Sleep(30);
        // Release the key
        ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
        SendInput(1, &ip, sizeof(INPUT));
        if (mk.isCtrlDown() || (vk_modifiers & 0x2))
        {
            ip.ki.wVk = VK_CONTROL;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (mk.isShiftDown() || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (mk.isAltDown() || (vk_modifiers & 0x4))
        {
            ip.ki.wVk = VK_MENU;
            SendInput(1, &ip, sizeof(INPUT));
        }
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

