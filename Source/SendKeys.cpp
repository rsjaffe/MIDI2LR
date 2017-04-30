// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

SendKeys.cpp

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
==============================================================================
*/
#include "SendKeys.h"
#include <algorithm>
#include <cctype>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#ifdef _WIN32
#include "Windows.h"
#else
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Carbon/Carbon.h>
#include <cassert>
#include <libproc.h>
#include <thread>
#endif
namespace {
#ifndef _WIN32

    pid_t GetPID()
    {
        std::string LR{"Adobe Lightroom.app/Contents/MacOS/Adobe Lightroom"};
        int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
        pid_t pids[numberOfProcesses];
        proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
        char pathBuffer[PROC_PIDPATHINFO_MAXSIZE];
        std::size_t found;
        for (int i = 0; i < numberOfProcesses; ++i) {
            if (pids[i] == 0) {
                continue;
            }
            bzero(pathBuffer, PROC_PIDPATHINFO_MAXSIZE);
            proc_pidpath(pids[i], pathBuffer, sizeof(pathBuffer));
            if (strlen(pathBuffer) > 0) {
                found = std::string(pathBuffer).find(LR);
                if (found != std::string::npos) {
                    return pids[i];
                }
            }
        }
        return 0;
    }

    /* From: https://stackoverflow.com/questions/1918841/how-to-convert-ascii-character-to-cgkeycode/1971027#1971027
     *
     * Returns string representation of key, if it is printable.
     * Ownership follows the Create Rule; that is, it is the caller's
     * responsibility to release the returned object. */
    CFStringRef createStringForKey(CGKeyCode keyCode)
    {
        TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
        CFDataRef layoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
        const UCKeyboardLayout *keyboardLayout =
            (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);

        UInt32 keysDown = 0;
        UniChar chars[4];
        UniCharCount realLength;

        UCKeyTranslate(keyboardLayout,
            keyCode,
            kUCKeyActionDisplay,
            0,
            LMGetKbdType(),
            kUCKeyTranslateNoDeadKeysBit,
            &keysDown,
            sizeof(chars) / sizeof(chars[0]),
            &realLength,
            chars);
        CFRelease(currentKeyboard);

        return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
    }

    /* From: https://stackoverflow.com/questions/1918841/how-to-convert-ascii-character-to-cgkeycode/1971027#1971027
     *
     * Returns key code for given character via the above function, or UINT16_MAX
     * on error. */
    CGKeyCode keyCodeForChar(const char c)
    {
        static CFMutableDictionaryRef charToCodeDict = NULL;
        CGKeyCode code;
        UniChar character = c;
        CFStringRef charStr = NULL;

        /* Generate table of keycodes and characters. */
        if (charToCodeDict == NULL) {
            size_t i;
            charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                128,
                &kCFCopyStringDictionaryKeyCallBacks,
                NULL);
            if (charToCodeDict == NULL) return UINT16_MAX;

            /* Loop through every keycode (0 - 127) to find its current mapping. */
            for (i = 0; i < 128; ++i) {
                CFStringRef string = createStringForKey((CGKeyCode)i);
                if (string != NULL) {
                    CFDictionaryAddValue(charToCodeDict, string, (const void *)i);
                    CFRelease(string);
                }
            }
        }

        charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &character, 1);

        /* Our values may be NULL (0), so we need to use this function. */
        if (!CFDictionaryGetValueIfPresent(charToCodeDict, charStr,
            (const void **)&code)) {
            code = UINT16_MAX;
        }

        CFRelease(charStr);
        return code;
    }

#endif

    std::string to_lower(const std::string& in)
    {
        auto s = in;
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

#ifdef _WIN32
    wchar_t MBtoWChar(const std::string& key)
    {
        wchar_t full_character;
        const auto return_value = MultiByteToWideChar(CP_UTF8, 0, key.data(),
            static_cast<int>(key.size()), &full_character, 1);
        if (return_value == 0) {
            const auto er = GetLastError();
            switch (er) {
            case ERROR_INVALID_FLAGS:
            case ERROR_INVALID_PARAMETER:
                throw std::invalid_argument("Bad argument to MultiByteToWideChar.");
            case ERROR_INSUFFICIENT_BUFFER:
                throw std::length_error("Insufficient buffer for MultiByteToWideChar.");
            case ERROR_NO_UNICODE_TRANSLATION:
                throw std::domain_error("Unable to translate: MultiByteToWideChar.");
            default:
                throw std::runtime_error("Unknown error: MultiByteToWideChar.");
            }
        }
        return full_character;
    }

    HKL GetLanguage(const std::string& program_name)
    {
        const auto hLRWnd = FindWindow(nullptr, program_name.c_str());
        if (hLRWnd) {
            // get language that LR is using (if hLrWnd is found)
            const auto thread_id = GetWindowThreadProcessId(hLRWnd, nullptr);
            return GetKeyboardLayout(thread_id);
        }
        else {   // use keyboard of MIDI2LR application
            return GetKeyboardLayout(0);
        }
    }
#endif

    static const std::unordered_map<std::string, unsigned char> key_map_ = {
#ifdef _WIN32
        {"backspace", VK_BACK},
        {"cursor down", VK_DOWN},
        {"cursor left", VK_LEFT},
        {"cursor right", VK_RIGHT},
        {"cursor up", VK_UP},
        {"delete", VK_DELETE},
        {"end", VK_END},
        {"escape", VK_ESCAPE},
        {"home", VK_HOME},
        {"page down", VK_NEXT},
        {"page up", VK_PRIOR},
        {"return", VK_RETURN},
        {"space", VK_SPACE},
        {"tab", VK_TAB},
        {"f1", VK_F1},
        {"f2", VK_F2},
        {"f3", VK_F3},
        {"f4", VK_F4},
        {"f5", VK_F5},
        {"f6", VK_F6},
        {"f7", VK_F7},
        {"f8", VK_F8},
        {"f9", VK_F9},
        {"f10", VK_F10},
        {"f11", VK_F11},
        {"f12", VK_F12},
        {"f13", VK_F13},
        {"f14", VK_F14},
        {"f15", VK_F15},
        {"f16", VK_F16},
        {"f17", VK_F17},
        {"f18", VK_F18},
        {"f19", VK_F19},
        {"f20", VK_F20},
        {"numpad 0", VK_NUMPAD0},
        {"numpad 1", VK_NUMPAD1},
        {"numpad 2", VK_NUMPAD2},
        {"numpad 3", VK_NUMPAD3},
        {"numpad 4", VK_NUMPAD4},
        {"numpad 5", VK_NUMPAD5},
        {"numpad 6", VK_NUMPAD6},
        {"numpad 7", VK_NUMPAD7},
        {"numpad 8", VK_NUMPAD8},
        {"numpad 9", VK_NUMPAD9},
        {"numpad add", VK_ADD},
        {"numpad subtract", VK_SUBTRACT},
        {"numpad multiply", VK_MULTIPLY},
        {"numpad divide", VK_DIVIDE},
        {"numpad decimal", VK_DECIMAL}
#else
    {
        "backspace", kVK_Delete
    },
    {"cursor down", kVK_DownArrow},
    {"cursor left", kVK_LeftArrow},
    {"cursor right", kVK_RightArrow},
    {"cursor up", kVK_UpArrow},
    {"delete", kVK_ForwardDelete},
    {"end", kVK_End},
    {"escape", kVK_Escape},
    {"home", kVK_Home},
    {"page down", kVK_PageDown},
    {"page up", kVK_PageUp},
    {"return", kVK_Return},
    {"space", kVK_Space},
    {"tab", kVK_Tab},
    {"f1", kVK_F1},
    {"f2", kVK_F2},
    {"f3", kVK_F3},
    {"f4", kVK_F4},
    {"f5", kVK_F5},
    {"f6", kVK_F6},
    {"f7", kVK_F7},
    {"f8", kVK_F8},
    {"f9", kVK_F9},
    {"f10", kVK_F10},
    {"f11", kVK_F11},
    {"f12", kVK_F12},
    {"f13", kVK_F13},
    {"f14", kVK_F14},
    {"f15", kVK_F15},
    {"f16", kVK_F16},
    {"f17", kVK_F17},
    {"f18", kVK_F18},
    {"f19", kVK_F19},
    {"f20", kVK_F20},
            //using ANSI layout codes for keypad, may cause problems in some languages
    {"numpad 0", kVK_ANSI_Keypad0},
    {"numpad 1", kVK_ANSI_Keypad1},
    {"numpad 2", kVK_ANSI_Keypad2},
    {"numpad 3", kVK_ANSI_Keypad3},
    {"numpad 4", kVK_ANSI_Keypad4},
    {"numpad 5", kVK_ANSI_Keypad5},
    {"numpad 6", kVK_ANSI_Keypad6},
    {"numpad 7", kVK_ANSI_Keypad7},
    {"numpad 8", kVK_ANSI_Keypad8},
    {"numpad 9", kVK_ANSI_Keypad9},
    {"numpad add", kVK_ANSI_KeypadPlus},
    {"numpad subtract", kVK_ANSI_KeypadMinus},
    {"numpad multiply", kVK_ANSI_KeypadMultiply},
    {"numpad divide", kVK_ANSI_KeypadDivide},
    {"numpad decimal", kVK_ANSI_KeypadDecimal}
#endif
    };

    static std::mutex mutex_sending_{};
}

void RSJ::SendKeyDownUp(const std::string& key, const bool alt_opt,
    const bool control_cmd, const bool shift)
{
    const auto mapped_key = key_map_.find(to_lower(key));
    const auto in_keymap = mapped_key != key_map_.end();

#ifdef _WIN32

    BYTE vk = 0;
    BYTE vk_modifiers = 0;
    if (in_keymap)
        vk = mapped_key->second;
    else {// Translate key code to keyboard-dependent scan code, may be UTF-8
        const auto language_id = GetLanguage("Lightroom");
        const auto vk_code_and_shift = VkKeyScanExW(MBtoWChar(key), language_id);
        vk = LOBYTE(vk_code_and_shift);
        vk_modifiers = HIBYTE(vk_code_and_shift);
    }

    //construct virtual keystroke sequence
    std::vector<WORD> strokes{vk}; // start with actual key, then mods
    if (shift || (vk_modifiers & 0x1)) {
        strokes.push_back(VK_SHIFT);
    }
    if ((vk_modifiers & 0x06) == 0x06) {
        strokes.push_back(VK_RMENU); //AltGr
        if (control_cmd)
            strokes.push_back(VK_CONTROL);
        if (alt_opt)
            strokes.push_back(VK_MENU);
    }
    else {
        if (control_cmd || (vk_modifiers & 0x2)) {
            strokes.push_back(VK_CONTROL);
        }
        if (alt_opt || (vk_modifiers & 0x4)) {
            strokes.push_back(VK_MENU);
        }
    }

    // construct input event.
    INPUT ip;
    constexpr auto size_ip = sizeof(ip);
    ip.type = INPUT_KEYBOARD;
    //ki: wVk, wScan, dwFlags, time, dwExtraInfo
    ip.ki = {0, 0, 0, 0, 0};

    //send key down strokes
    std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
    for (auto it = strokes.crbegin(); it != strokes.crend(); ++it) {
        ip.ki.wVk = *it;
        SendInput(1, &ip, size_ip);
    }
    //send key up strokes
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    for (const auto it : strokes) {
        ip.ki.wVk = it;
        SendInput(1, &ip, size_ip);
    }
#else
    static ProcessSerialNumber psn{0};
    static pid_t lr_pid{0};
    if (lr_pid == 0) {
        lr_pid = GetPID();
        if (lr_pid) {
            GetProcessForPID(lr_pid, &psn); //first deprecated in macOS 10.9, but no good replacement yet
        }
        else {
            lr_pid = -1; // cannot find LR pid, to try to find the forground process
            GetFrontProcess(&psn); //first deprecated in macOS 10.9, but no good replacement yet
        }
    }

    CGEventRef d;
    CGEventRef u;
    uint64_t flags = 0;

    if (in_keymap) {
        const auto vk = mapped_key->second;
        d = CGEventCreateKeyboardEvent(NULL, vk, true);
        u = CGEventCreateKeyboardEvent(NULL, vk, false);
    }
    else {
        const CGKeyCode keyCode = keyCodeForChar(key[0]);
        d = CGEventCreateKeyboardEvent(NULL, keyCode, true);
        u = CGEventCreateKeyboardEvent(NULL, keyCode, false);
        flags = CGEventGetFlags(d); //in case KeyCode has associated flag
    }

    if (control_cmd) flags |= kCGEventFlagMaskCommand;
    if (alt_opt) flags |= kCGEventFlagMaskAlternate;
    if (shift) flags |= kCGEventFlagMaskShift;
    if (flags) {
        CGEventSetFlags(d, static_cast<CGEventFlags>(flags));
        CGEventSetFlags(u, static_cast<CGEventFlags>(flags));
    }

    {
        std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
        CGEventPostToPSN(&psn, d);
        CGEventPostToPSN(&psn, u);
    }

    CFRelease(d);
    CFRelease(u);
#endif
}