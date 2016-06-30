/*
==============================================================================

SendKeys.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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
#include <cctype>
#ifdef _WIN32
#include "Windows.h"
#else
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#include <vector>
#include <string>
#include <thread>
#endif
namespace {
#ifdef _WIN32
  wchar_t MBtoWChar(const std::string& key) {
    wchar_t full_character;
    const auto return_value = MultiByteToWideChar(CP_UTF8, 0, key.data(),
      key.size(), &full_character, 1);
    if (return_value == 0) {
      auto er = GetLastError();
      if (er == ERROR_INVALID_FLAGS || er == ERROR_INVALID_PARAMETER)
        throw std::invalid_argument("Bad argument to MultiByteToWideChar.");
      if (er == ERROR_INSUFFICIENT_BUFFER)
        throw std::length_error("Insufficient buffer for MultiByteToWideChar.");
      if (er == ERROR_NO_UNICODE_TRANSLATION)
        throw std::domain_error("Unable to translate: MultiByteToWideChar.");
      throw std::runtime_error("Unknown error: MultiByteToWideChar.");
    }
    return full_character;
  }

  HKL GetLanguageAndFGApp(std::string program_name) {
    const auto hLRWnd = FindWindow(NULL, program_name.c_str());
    HKL language_id;
    // Bring Lightroom to foreground if it isn't already there
    if (hLRWnd) {
      SetForegroundWindow(hLRWnd);
      // get language that LR is using (if hLrWnd is found)
      const auto thread_id = GetWindowThreadProcessId(hLRWnd, NULL);
      language_id = GetKeyboardLayout(thread_id);
    }
    else {   // use keyboard of MIDI2LR application
      language_id = GetKeyboardLayout(0);
    }
    return language_id;
  }
#else
  std::wstring utf8_to_utf16(const std::string& utf8) {
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size()) {
      unsigned long uni;
      size_t todo;
      bool error = false;
      unsigned char ch = utf8[i++];
      if (ch <= 0x7F) {
        uni = ch;
        todo = 0;
      }
      else if (ch <= 0xBF) {
        throw std::domain_error("not a UTF-8 string");
      }
      else if (ch <= 0xDF) {
        uni = ch & 0x1F;
        todo = 1;
      }
      else if (ch <= 0xEF) {
        uni = ch & 0x0F;
        todo = 2;
      }
      else if (ch <= 0xF7) {
        uni = ch & 0x07;
        todo = 3;
      }
      else {
        throw std::domain_error("not a UTF-8 string");
      }
      for (size_t j = 0; j < todo; ++j) {
        if (i == utf8.size())
          throw std::domain_error("not a UTF-8 string");
        unsigned char ch = utf8[i++];
        if (ch < 0x80 || ch > 0xBF)
          throw std::domain_error("not a UTF-8 string");
        uni <<= 6;
        uni += ch & 0x3F;
      }
      if (uni >= 0xD800 && uni <= 0xDFFF)
        throw std::domain_error("not a UTF-8 string");
      if (uni > 0x10FFFF)
        throw std::domain_error("not a UTF-8 string");
      unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i) {
      unsigned long uni = unicode[i];
      if (uni <= 0xFFFF) {
        utf16 += (wchar_t)uni;
      }
      else {
        uni -= 0x10000;
        utf16 += (wchar_t)((uni >> 10) + 0xD800);
        utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
      }
    }
    return utf16;
  }
#endif
}

const std::unordered_map<std::string, unsigned char> SendKeys::key_map_ = {
#ifdef _WIN32
{"space", 0x20},
{"backspace", 0x08},
{"page up",	0x21},
{"page down",0x22},
{ "end",	0x23 },
{"home",	0x24},
{"cursor left",	0x25},
{"cursor up",	0x26},
{"cursor right",	0x27},
{"cursor down",	0x28},
{"delete",	0x2E},
{"return",	0x0D},
{"tab",	0x09},
{"escape", 0x1B},
{"f1",	0x70},
{"f2",	0x71},
{"f3",	0x72},
{"f4",	0x73},
{"f5",	0x74},
{"f6",	0x75},
{"f7",	0x76},
{"f8",	0x77},
{"f9",	0x78},
{"f10",	0x79},
{"f11",	0x7A},
{"f12",	0x7B},
{"f13",	0x7C},
{"f14",	0x7D},
{"f15",	0x7E},
{"f16",	0x7F},
{"f17",	0x80},
{"f18",	0x81},
{"f19",	0x82},
{"f20",	0x83}
#else
{
"space", 0x31
},
{"backspace", 0x33},
{"page up", 0x74 },
{"page down", 0x79 },
{"end",	0x77 },
{"home", 0x73 },
{"cursor left",	0x7B },
{"cursor up", 0x7E },
{"cursor right", 0x7C },
{"cursor down",	0x7D },
{"delete", 0x75 },
{"return", 0x24 },
{"tab",	0x30 },
{"escape", 0x35},
{"f1", 0x7A },
{"f2", 0x78 },
{"f3", 0x63 },
{"f4", 0x76 },
{"f5", 0x60 },
{"f6", 0x61 },
{"f7", 0x62 },
{"f8", 0x64 },
{"f9", 0x65 },
{"f10",	0x6D },
{"f11",	0x67 },
{"f12",	0x6F },
{"f13",	0x69 },
{"f14",	0x6B },
{"f15",	0x71 },
{"f16",	0x6A },
{"f17",	0x40 },
{"f18",	0x4F },
{"f19",	0x50 },
{"f20",	0x5A }
#endif
};

std::mutex SendKeys::mutex_sending_{};

void SendKeys::SendKeyDownUp(const std::string& key, const bool alt_opt,
  const bool control_cmd, const bool shift) const {
  std::string lower_string; //used for matching with key names
  for (const auto& c : key)
    lower_string.push_back(static_cast<char>(std::tolower(c))); //c is char but tolower returns int
#ifdef _WIN32
    //Lightroom handle
  HKL language_id = GetLanguageAndFGApp("Lightroom");
  BYTE vk = 0;
  BYTE vk_modifiers = 0;
  if (SendKeys::key_map_.count(lower_string))
    vk = SendKeys::key_map_.at(lower_string);
  else {// Translate key code to keyboard-dependent scan code, may be UTF-8
    const auto vk_code_and_shift = VkKeyScanExW(MBtoWChar(key), language_id);
    vk = LOBYTE(vk_code_and_shift);
    vk_modifiers = HIBYTE(vk_code_and_shift);
  }

  //collect keystrokes
  std::vector<unsigned int> strokes{vk};
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
  ip.type = INPUT_KEYBOARD;
  //ki: wVk, wScan, dwFlags, time, dwExtraInfo
  ip.ki = {0,0,0,0,0};
  //send keystrokes
  std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
  for (auto it = strokes.crbegin(); it != strokes.crend(); ++it) {
    ip.ki.wVk = *it;
    SendInput(1, &ip, sizeof(INPUT));
  }
  // Release the key
  ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
  for (const auto it : strokes) {
    ip.ki.wVk = it;
    SendInput(1, &ip, sizeof(INPUT));
  }
#else
  const CGEventSourceRef source =
    CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  CGEventRef d;
  CGEventRef u;
  uint64_t flags = 0;

  if (SendKeys::key_map_.count(lower_string)) {
    auto vk = SendKeys::key_map_.at(lower_string);
    d = CGEventCreateKeyboardEvent(source, vk, true);
    u = CGEventCreateKeyboardEvent(source, vk, false);
  }
  else {
    const UniChar key_character{utf8_to_utf16(key)[0]};
    d = CGEventCreateKeyboardEvent(source, 0, true);
    u = CGEventCreateKeyboardEvent(source, 0, false);
    CGEventKeyboardSetUnicodeString(d, 1, &key_character);
    CGEventKeyboardSetUnicodeString(u, 1, &key_character);
    flags = CGEventGetFlags(d); //in case KeyCode has associated flag
  }

  if (control_cmd) flags |= kCGEventFlagMaskCommand;
  if (alt_opt) flags |= kCGEventFlagMaskAlternate;
  if (shift) flags |= kCGEventFlagMaskShift;
  if (flags) {
    CGEventSetFlags(d, static_cast<CGEventFlags>(flags));
    CGEventSetFlags(u, static_cast<CGEventFlags>(flags));
  }
  CGEventRef cmdd = CGEventCreateKeyboardEvent(source, 0x37, true);
  CGEventRef cmdu = CGEventCreateKeyboardEvent(source, 0x37, false);

  {   //restrict scope for mutex lock
    constexpr CGEventTapLocation loc = kCGHIDEventTap; // kCGSessionEventTap also works
    std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
    if (flags & kCGEventFlagMaskCommand) {
      CGEventPost(loc, cmdd);
    }
    CGEventPost(loc, d);
    CGEventPost(loc, u);
    if (flags & kCGEventFlagMaskCommand) {
      CGEventPost(loc, cmdu);
    }
  }

  CFRelease(d);
  CFRelease(u);
  CFRelease(cmdd);
  CFRelease(cmdu);
  CFRelease(source);
#endif
}