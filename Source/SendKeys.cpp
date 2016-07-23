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
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <vector>
#ifdef _WIN32
#include "Windows.h"
#else
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Carbon/Carbon.h>
#include <thread>
#endif
namespace {
  std::string to_lower(const std::string& in) {
    auto s = in;
    std::transform(s.begin(), s.end(), s.begin(), std::tolower);
    return s;
  }

#ifdef _WIN32
  wchar_t MBtoWChar(const std::string& key) {
    wchar_t full_character;
    const auto return_value = MultiByteToWideChar(CP_UTF8, 0, key.data(),
      key.size(), &full_character, 1);
    if (return_value == 0) {
      const auto er = GetLastError();
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

  HKL GetLanguage(std::string program_name) {
    const auto hLRWnd = FindWindow(NULL, program_name.c_str());
    if (hLRWnd) {
      // get language that LR is using (if hLrWnd is found)
      const auto thread_id = GetWindowThreadProcessId(hLRWnd, NULL);
      return GetKeyboardLayout(thread_id);
    }
    else {   // use keyboard of MIDI2LR application
      return GetKeyboardLayout(0);
    }
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
{"backspace",     VK_BACK},
{"cursor down",	  VK_DOWN},
{"cursor left",	  VK_LEFT},
{"cursor right",	VK_RIGHT},
{"cursor up",   	VK_UP},
{"delete",	      VK_DELETE},
{"end",         	VK_END},
{"escape",        VK_ESCAPE},
{"home",	        VK_HOME},
{"page down",     VK_NEXT},
{"page up",	      VK_PRIOR},
{"return",	      VK_RETURN},
{"space",         VK_SPACE},
{"tab",	          VK_TAB},
{"f1",	VK_F1},
{"f2",	VK_F2},
{"f3",	VK_F3},
{"f4",	VK_F4},
{"f5",	VK_F5},
{"f6",	VK_F6},
{"f7",	VK_F7},
{"f8",	VK_F8},
{"f9",	VK_F9},
{"f10",	VK_F10},
{"f11",	VK_F11},
{"f12",	VK_F12},
{"f13",	VK_F13},
{"f14",	VK_F14},
{"f15",	VK_F15},
{"f16",	VK_F16},
{"f17",	VK_F17},
{"f18",	VK_F18},
{"f19",	VK_F19},
{"f20",	VK_F20}
#else
{
"backspace",    kVK_Delete
},
{"cursor down",	 kVK_DownArrow},
{"cursor left",  kVK_LeftArrow},
{"cursor right", kVK_RightArrow},
{"cursor up",    kVK_UpArrow},
{"delete",       kVK_ForwardDelete},
{"end",	         kVK_End},
{"escape",       kVK_Escape},
{"home",         kVK_Home},
{"page down",    kVK_PageDown},
{"page up",      kVK_PageUp},
{"return",       kVK_Return},
{"space",        kVK_Space},
{"tab",	         kVK_Tab},
{"f1", kVK_F1},
{"f2", kVK_F2},
{"f3", kVK_F3},
{"f4", kVK_F4},
{"f5", kVK_F5},
{"f6", kVK_F6},
{"f7", kVK_F7},
{"f8", kVK_F8},
{"f9", kVK_F9},
{"f10",	kVK_F10},
{"f11",	kVK_F11},
{"f12",	kVK_F12},
{"f13",	kVK_F13},
{"f14",	kVK_F14},
{"f15",	kVK_F15},
{"f16",	kVK_F16},
{"f17",	kVK_F17},
{"f18",	kVK_F18},
{"f19",	kVK_F19},
{"f20",	kVK_F20}
#endif
};

std::mutex SendKeys::mutex_sending_{};

void SendKeys::SendKeyDownUp(const std::string& key, const bool alt_opt,
  const bool control_cmd, const bool shift) const {

  const auto mapped_key = SendKeys::key_map_.find(to_lower(key));
  const auto in_keymap = mapped_key != SendKeys::key_map_.end();

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
  std::vector<unsigned int> strokes{vk}; // start with actual key, then mods
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
  ip.ki = {0,0,0,0,0};

  //send key down strokes
  std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
  for (auto it = strokes.crbegin(); it != strokes.crend(); ++it) {
    ip.ki.wVk = static_cast<WORD>(*it);
    SendInput(1, &ip, size_ip);
  }
  //send key up strokes
  ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
  for (const auto it : strokes) {
    ip.ki.wVk = static_cast<WORD>(it);
    SendInput(1, &ip, size_ip);
  }
#else
  const CGEventSourceRef source =
    CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

  CGEventRef d;
  CGEventRef u;

  CGEventRef cmdd = CGEventCreateKeyboardEvent(source, kVK_Command, true);
  CGEventRef cmdu = CGEventCreateKeyboardEvent(source, kVK_Command, false);

  uint64_t flags = 0;

  if (in_keymap) {
    const auto vk = mapped_key->second;
    d = CGEventCreateKeyboardEvent(source, vk, true);
    u = CGEventCreateKeyboardEvent(source, vk, false);
  }
  else {
    const UniChar key_character(static_cast<UniChar>(utf8_to_utf16(key)[0]));
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

  constexpr CGEventTapLocation loc = kCGSessionEventTap;
  if (flags & kCGEventFlagMaskCommand) {
    std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
    CGEventPost(loc, cmdd);
    CGEventPost(loc, d);
    CGEventPost(loc, u);
    CGEventPost(loc, cmdu);
  }
  else {
    std::lock_guard<decltype(mutex_sending_)> lock(mutex_sending_);
    CGEventPost(loc, d);
    CGEventPost(loc, u);
  }

  CFRelease(d);
  CFRelease(u);
  CFRelease(cmdd);
  CFRelease(cmdu);
  CFRelease(source);
#endif
}