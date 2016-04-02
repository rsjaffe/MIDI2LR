/*
==============================================================================

SendKeys.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
==============================================================================
*/
#include "SendKeys.h"
#ifdef _WIN32
#include "Windows.h"
#else
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#endif

/**********************************************************************************************//**
 * @property    std::mutex SendKeys::m_mtxSending
 *
 * @brief   Gets the mtx sending.
 *
 * @return  The m mtx sending.
 **************************************************************************************************/

std::mutex SendKeys::m_mtxSending{};

/**********************************************************************************************//**
 * @fn  void SendKeys::SendKeyDownUp(const KeyPress& key) const
 *
 * @brief   Sends a key down up.
 *
 *
 *
 * @param   key The key.
 **************************************************************************************************/

void SendKeys::SendKeyDownUp(const KeyPress& key) const
{
    std::lock_guard< decltype(m_mtxSending) > lock(m_mtxSending);
#ifdef _WIN32
    //Lightroom handle
    const auto hLRWnd = ::FindWindow(NULL, "Lightroom");
    const auto mk = key.getModifiers();
    HKL languageID;
    // Bring Lightroom to foreground if it isn't already there
    if (hLRWnd)
    {
        ::SetForegroundWindow(hLRWnd);
        // get language that LR is using (if hLrWnd is found)
        const auto threadId = GetWindowThreadProcessId(hLRWnd, NULL);
        languageID = GetKeyboardLayout(threadId);
    }
    else
    {   // use keyboard of MIDI2LR app
        languageID = GetKeyboardLayout(0);
    }
    // Translate key code to keyboard-dependent scan code
    const auto vkCodeAndShift = VkKeyScanExW(static_cast<WCHAR>(key.getKeyCode()), languageID);
    const auto vk = LOBYTE(vkCodeAndShift);
    const auto vk_modifiers = HIBYTE(vkCodeAndShift);

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
    uint64_t flags = CGEventGetFlags(d); //in case KeyCode has associated flag
    if (mk.isCommandDown()) flags |= kCGEventFlagMaskCommand;
    if (mk.isAltDown()) flags |= kCGEventFlagMaskAlternate;
    if (mk.isShiftDown()) flags |= kCGEventFlagMaskShift;
    if (flags != UINT64_C(0))
    {
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

