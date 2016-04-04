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
std::wstring utf8_to_utf16(const std::string& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            throw std::logic_error("not a UTF-8 string");
        }
        else if (ch <= 0xDF)
        {
            uni = ch & 0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch & 0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch & 0x07;
            todo = 3;
        }
        else
        {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}
/** @brief   The send keys keymap. */
const std::unordered_map<std::string, unsigned char> SendKeys::keymap = {
#ifdef _WIN32
{"Page Up",	0x21},
{"Page Down",0x22},
{ "End",	0x23 },
{"Home",	0x24},
{"Left Arrow",	0x25},
{"Up Arrow",	0x26},
{"Right Arrow",	0x27},
{"Down Arrow",	0x28},
{"Delete",	0x2E},
{"Enter",	0x0D},
{"Tab",	0x09},
{"F1",	0x70},
{"F2",	0x71},
{"F3",	0x72},
{"F4",	0x73},
{"F5",	0x74},
{"F6",	0x75},
{"F7",	0x76},
{"F8",	0x77},
{"F9",	0x78},
{"F10",	0x79},
{"F11",	0x7A},
{"F12",	0x7B},
{"F13",	0x7C},
{"F14",	0x7D},
{"F15",	0x7E},
{"F16",	0x7F},
{"F17",	0x80},
{"F18",	0x81},
{"F19",	0x82},
{"F20",	0x83}

#else
{ "Page Up",	0x74 },
{"Page Down",	0x79 },
{"End",	0x77 },
{"Home",	0x73 },
{"Left Arrow",	0x7B },
{"Up Arrow",	0x7E },
{"Right Arrow",	0x7C },
{"Down Arrow",	0x7D },
{"Delete",	0x33 },
{"Enter",	0x24 },
{"Tab",	0x30 },
{"F1",	0x7A },
{"F2",	0x78 },
{"F3",	0x63 },
{"F4",	0x76 },
{"F5",	0x60 },
{"F6",	0x61 },
{"F7",	0x62 },
{"F8",	0x64 },
{"F9",	0x65 },
{"F10",	0x6D },
{"F11",	0x67 },
{"F12",	0x6F },
{"F13",	0x69 },
{"F14",	0x6B },
{"F15",	0x71 },
{"F16",	0x6A },
{"F17",	0x40 },
{"F18",	0x4F },
{"F19",	0x50 },
{"F20",	0x5A }
#endif
};

/**********************************************************************************************//**
 * @property    std::mutex SendKeys::m_mtxSending
 *
 * @brief   Gets the mtx sending.
 *
 * @return  The m mtx sending.
 **************************************************************************************************/

std::mutex SendKeys::m_mtxSending{};

/**********************************************************************************************//**
 * @fn  void SendKeys::SendKeyDownUp(const std::string& key, bool Alt, bool Control, bool Shift) const
 *
 * @brief   Sends a key down up.
 *
 * @param   key     The key.
 * @param   Alt     true to alternate.
 * @param   Control true to control.
 * @param   Shift   true to shift.
 **************************************************************************************************/

void SendKeys::SendKeyDownUp(const std::string& key, bool Alt, bool Control, bool Shift) const
{
    std::lock_guard< decltype(m_mtxSending) > lock(m_mtxSending);
#ifdef _WIN32
    //Lightroom handle
    const auto hLRWnd = ::FindWindow(NULL, "Lightroom");
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
    BYTE vk = 0;
    BYTE vk_modifiers = 0;
    if (SendKeys::keymap.count(key))
        vk = SendKeys::keymap.at(key);
    else
    {// Translate key code to keyboard-dependent scan code, may be UTF-8
        wchar_t fullchar;
        const auto retval = MultiByteToWideChar(CP_UTF8, 0, key.data(), key.size(), &fullchar, 1);
        if (retval == 0)
            throw GetLastError();
        const auto vkCodeAndShift = VkKeyScanExW(fullchar, languageID);
        vk = LOBYTE(vkCodeAndShift);
        vk_modifiers = HIBYTE(vkCodeAndShift);
    }

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
        if (Shift || (vk_modifiers & 0x1))
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
        if (Shift || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        ip.ki.wVk = VK_RMENU;
        SendInput(1, &ip, sizeof(INPUT));
    }
    else //not using AltGr key
    {
        if (Control || (vk_modifiers & 0x2))
        {
            ip.ki.wVk = VK_CONTROL;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (Shift || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (Alt || (vk_modifiers & 0x4))
        {
            ip.ki.wVk = VK_MENU;
            SendInput(1, &ip, sizeof(INPUT));
        }
        //press the key
        ip.ki.wVk = vk;
        SendInput(1, &ip, sizeof(INPUT));
        //add 30 msec between press and release
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        // Release the key
        ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
        SendInput(1, &ip, sizeof(INPUT));
        if (Control || (vk_modifiers & 0x2))
        {
            ip.ki.wVk = VK_CONTROL;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (Shift || (vk_modifiers & 0x1))
        {
            ip.ki.wVk = VK_SHIFT;
            SendInput(1, &ip, sizeof(INPUT));
        }
        if (Alt || (vk_modifiers & 0x4))
        {
            ip.ki.wVk = VK_MENU;
            SendInput(1, &ip, sizeof(INPUT));
        }
    }
#else
    const CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
    CGEventRef d;
    CGEventRef u;
    uint64_t flags = 0;
    if (SendKeys::keymap.count(key))
    {
        auto vk = SendKeys::keymap.at(key);
        d = CGEventCreateKeyboardEvent(source, vk, true);
        u = CGEventCreateKeyboardEvent(source, vk, false);
        if (Control) flags |= kCGEventFlagMaskCommand;
        if (Alt) flags |= kCGEventFlagMaskAlternate;
        if (Shift) flags |= kCGEventFlagMaskShift;
        if (flags != UINT64_C(0))
        {
            CGEventSetFlags(d, static_cast<CGEventFlags>(flags));
            CGEventSetFlags(u, static_cast<CGEventFlags>(flags));
        }

    }
    else
    {
        const std::wstring utf16str{ utf8_to_utf16(key) };
        d = CGEventCreateKeyboardEvent(source, 0, true);
        u = CGEventCreateKeyboardEvent(source, 0, false);
        CGEventKeyboardSetUnicodeString(d, utf16str.length(), utf16str.data());
        CGEventKeyboardSetUnicodeString(u, utf16str.length(), utf16str.data());
        flags = CGEventGetFlags(d); //in case KeyCode has associated flag
        if (Control) flags |= kCGEventFlagMaskCommand;
        if (Alt) flags |= kCGEventFlagMaskAlternate;
        if (Shift) flags |= kCGEventFlagMaskShift;
        if (flags != UINT64_C(0))
        {
            CGEventSetFlags(d, static_cast<CGEventFlags>(flags));
            CGEventSetFlags(u, static_cast<CGEventFlags>(flags));
        }

    }


    CGEventPost(kCGHIDEventTap, d);
    CGEventPost(kCGHIDEventTap, u);

    CFRelease(d);
    CFRelease(u);
    CFRelease(source);
#endif
}

