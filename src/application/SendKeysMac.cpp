/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _WIN32
#include <array>
#include <cstring>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#import <CoreGraphics/CoreGraphics.h>
#include <fmt/format.h>
#include <gsl/gsl>
#include <libproc.h> /* proc_ functions in GetPid */
#include <ww898/utf_converters.hpp>

#include <juce_core/juce_core.h>/*creates ambiguous reference to Point if included before Mac headers*/
#include <juce_events/juce_events.h>

#include "Misc.h"
#include "Ocpp.h"
#include "SendKeys.h"

namespace {
   std::once_flag of_check_permission;
   const auto kLrc {".app/Contents/MacOS/Adobe Lightroom Classic"};

   /* From Events.h in Carbon framework
    *  Summary:
    *    Virtual keycodes
    *
    *  Discussion:
    *    These constants are the virtual keycodes defined originally in
    *    Inside Mac Volume V, pg. V-191. They identify physical keys on a
    *    keyboard. Those constants with "ANSI" in the name are labeled
    *    according to the key position on an ANSI-standard US keyboard.
    *    For example, kVK_ANSI_A indicates the virtual keycode for the key
    *    with the letter 'A' in the US keyboard layout. Other keyboard
    *    layouts may have the 'A' key label on a different physical key;
    *    in this case, pressing 'A' will generate a different virtual
    *    keycode.
    */
   enum {
      kVK_ANSI_A = 0x00,
      kVK_ANSI_S = 0x01,
      kVK_ANSI_D = 0x02,
      kVK_ANSI_F = 0x03,
      kVK_ANSI_H = 0x04,
      kVK_ANSI_G = 0x05,
      kVK_ANSI_Z = 0x06,
      kVK_ANSI_X = 0x07,
      kVK_ANSI_C = 0x08,
      kVK_ANSI_V = 0x09,
      kVK_ANSI_B = 0x0B,
      kVK_ANSI_Q = 0x0C,
      kVK_ANSI_W = 0x0D,
      kVK_ANSI_E = 0x0E,
      kVK_ANSI_R = 0x0F,
      kVK_ANSI_Y = 0x10,
      kVK_ANSI_T = 0x11,
      kVK_ANSI_1 = 0x12,
      kVK_ANSI_2 = 0x13,
      kVK_ANSI_3 = 0x14,
      kVK_ANSI_4 = 0x15,
      kVK_ANSI_6 = 0x16,
      kVK_ANSI_5 = 0x17,
      kVK_ANSI_Equal = 0x18,
      kVK_ANSI_9 = 0x19,
      kVK_ANSI_7 = 0x1A,
      kVK_ANSI_Minus = 0x1B,
      kVK_ANSI_8 = 0x1C,
      kVK_ANSI_0 = 0x1D,
      kVK_ANSI_RightBracket = 0x1E,
      kVK_ANSI_O = 0x1F,
      kVK_ANSI_U = 0x20,
      kVK_ANSI_LeftBracket = 0x21,
      kVK_ANSI_I = 0x22,
      kVK_ANSI_P = 0x23,
      kVK_ANSI_L = 0x25,
      kVK_ANSI_J = 0x26,
      kVK_ANSI_Quote = 0x27,
      kVK_ANSI_K = 0x28,
      kVK_ANSI_Semicolon = 0x29,
      kVK_ANSI_Backslash = 0x2A,
      kVK_ANSI_Comma = 0x2B,
      kVK_ANSI_Slash = 0x2C,
      kVK_ANSI_N = 0x2D,
      kVK_ANSI_M = 0x2E,
      kVK_ANSI_Period = 0x2F,
      kVK_ANSI_Grave = 0x32,
      kVK_ANSI_KeypadDecimal = 0x41,
      kVK_ANSI_KeypadMultiply = 0x43,
      kVK_ANSI_KeypadPlus = 0x45,
      kVK_ANSI_KeypadClear = 0x47,
      kVK_ANSI_KeypadDivide = 0x4B,
      kVK_ANSI_KeypadEnter = 0x4C,
      kVK_ANSI_KeypadMinus = 0x4E,
      kVK_ANSI_KeypadEquals = 0x51,
      kVK_ANSI_Keypad0 = 0x52,
      kVK_ANSI_Keypad1 = 0x53,
      kVK_ANSI_Keypad2 = 0x54,
      kVK_ANSI_Keypad3 = 0x55,
      kVK_ANSI_Keypad4 = 0x56,
      kVK_ANSI_Keypad5 = 0x57,
      kVK_ANSI_Keypad6 = 0x58,
      kVK_ANSI_Keypad7 = 0x59,
      kVK_ANSI_Keypad8 = 0x5B,
      kVK_ANSI_Keypad9 = 0x5C
   };

   /* keycodes for keys that are independent of keyboard layout*/
   enum {
      kVK_Return = 0x24,
      kVK_Tab = 0x30,
      kVK_Space = 0x31,
      kVK_Delete = 0x33,
      kVK_Escape = 0x35,
      kVK_Command = 0x37,
      kVK_Shift = 0x38,
      kVK_CapsLock = 0x39,
      kVK_Option = 0x3A,
      kVK_Control = 0x3B,
      kVK_RightCommand = 0x36,
      kVK_RightShift = 0x3C,
      kVK_RightOption = 0x3D,
      kVK_RightControl = 0x3E,
      kVK_Function = 0x3F,
      kVK_F17 = 0x40,
      kVK_VolumeUp = 0x48,
      kVK_VolumeDown = 0x49,
      kVK_Mute = 0x4A,
      kVK_F18 = 0x4F,
      kVK_F19 = 0x50,
      kVK_F20 = 0x5A,
      kVK_F5 = 0x60,
      kVK_F6 = 0x61,
      kVK_F7 = 0x62,
      kVK_F3 = 0x63,
      kVK_F8 = 0x64,
      kVK_F9 = 0x65,
      kVK_F11 = 0x67,
      kVK_F13 = 0x69,
      kVK_F16 = 0x6A,
      kVK_F14 = 0x6B,
      kVK_F10 = 0x6D,
      kVK_F12 = 0x6F,
      kVK_F15 = 0x71,
      kVK_Help = 0x72,
      kVK_Home = 0x73,
      kVK_PageUp = 0x74,
      kVK_ForwardDelete = 0x75,
      kVK_F4 = 0x76,
      kVK_End = 0x77,
      kVK_F2 = 0x78,
      kVK_PageDown = 0x79,
      kVK_F1 = 0x7A,
      kVK_LeftArrow = 0x7B,
      kVK_RightArrow = 0x7C,
      kVK_DownArrow = 0x7D,
      kVK_UpArrow = 0x7E
   };

   /* ISO keyboards only*/
   enum { kVK_ISO_Section = 0x0A };

   /* JIS keyboards only*/
   enum {
      kVK_JIS_Yen = 0x5D,
      kVK_JIS_Underscore = 0x5E,
      kVK_JIS_KeypadComma = 0x5F,
      kVK_JIS_Eisu = 0x66,
      kVK_JIS_Kana = 0x68
   };

   pid_t GetPid()
   {
      try {
         /* add 20 in case more processes show up */
         const auto number_processes {proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0) + 20};
         std::vector<pid_t> pids(number_processes, 0);
         proc_listpids(
             PROC_ALL_PIDS, 0, pids.data(), gsl::narrow_cast<int>(sizeof(pids[0]) * pids.size()));
         std::array<char, PROC_PIDPATHINFO_MAXSIZE> path_buffer {};
         for (const auto pid : pids) {
            if (pid == 0) { continue; }
            path_buffer.fill(0);
            proc_pidpath(pid, path_buffer.data(), path_buffer.size());
            if (path_buffer[0] && std::strstr(path_buffer.data(), kLrc)) { return pid; }
         }
         rsj::LogAndAlertError("Lightroom PID not found.");
         return 0;
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
   }

   /* Returns key code for given character. Bool represents shift, option (AltGr) keys */
   std::optional<rsj::KeyData> KeyCodeForChar(const UniChar c)
   {
      try {
         static const std::unordered_map<UniChar, rsj::KeyData> char_code_map {rsj::GetKeyMap()};
         const auto result {char_code_map.find(c)};
         if (result != char_code_map.end()) { return result->second; }
         else {
            return {};
         }
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
   }

   void MacKeyDownUp(const pid_t lr_pid, const CGKeyCode vk, const CGEventFlags flags)
   {
      try {
         rsj::CFAutoRelease<CGEventRef> d {CGEventCreateKeyboardEvent(nullptr, vk, true)};
         rsj::CFAutoRelease<CGEventRef> u {CGEventCreateKeyboardEvent(nullptr, vk, false)};
         CGEventSetFlags(d.get(), flags);
         CGEventSetFlags(u.get(), flags);
         /* mutex unnecessary, as always called on main message thread */
         CGEventPostToPid(lr_pid, d.get());
         CGEventPostToPid(lr_pid, u.get());
         std::call_once(of_check_permission, [lr_pid]() { rsj::CheckPermission(lr_pid); });
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
   }

   const std::unordered_map<std::string, unsigned char> kKeyMap {
       {      "backspace",              kVK_Delete},
       {    "cursor down",           kVK_DownArrow},
       {    "cursor left",           kVK_LeftArrow},
       {   "cursor right",          kVK_RightArrow},
       {      "cursor up",             kVK_UpArrow},
       {         "delete",       kVK_ForwardDelete},
       {            "end",                 kVK_End},
       {         "escape",              kVK_Escape},
       {           "home",                kVK_Home},
       {      "page down",            kVK_PageDown},
       {        "page up",              kVK_PageUp},
       {         "return",              kVK_Return},
       {          "space",               kVK_Space},
       {            "tab",                 kVK_Tab},
       {             "f1",                  kVK_F1},
       {             "f2",                  kVK_F2},
       {             "f3",                  kVK_F3},
       {             "f4",                  kVK_F4},
       {             "f5",                  kVK_F5},
       {             "f6",                  kVK_F6},
       {             "f7",                  kVK_F7},
       {             "f8",                  kVK_F8},
       {             "f9",                  kVK_F9},
       {            "f10",                 kVK_F10},
       {            "f11",                 kVK_F11},
       {            "f12",                 kVK_F12},
       {            "f13",                 kVK_F13},
       {            "f14",                 kVK_F14},
       {            "f15",                 kVK_F15},
       {            "f16",                 kVK_F16},
       {            "f17",                 kVK_F17},
       {            "f18",                 kVK_F18},
       {            "f19",                 kVK_F19},
       {            "f20",                 kVK_F20},
 /* using ANSI layout codes for keypad, may cause problems in some languages */
       {       "numpad 0",        kVK_ANSI_Keypad0},
       {       "numpad 1",        kVK_ANSI_Keypad1},
       {       "numpad 2",        kVK_ANSI_Keypad2},
       {       "numpad 3",        kVK_ANSI_Keypad3},
       {       "numpad 4",        kVK_ANSI_Keypad4},
       {       "numpad 5",        kVK_ANSI_Keypad5},
       {       "numpad 6",        kVK_ANSI_Keypad6},
       {       "numpad 7",        kVK_ANSI_Keypad7},
       {       "numpad 8",        kVK_ANSI_Keypad8},
       {       "numpad 9",        kVK_ANSI_Keypad9},
       {     "numpad add",     kVK_ANSI_KeypadPlus},
       {"numpad subtract",    kVK_ANSI_KeypadMinus},
       {"numpad multiply", kVK_ANSI_KeypadMultiply},
       {  "numpad divide",   kVK_ANSI_KeypadDivide},
       { "numpad decimal",  kVK_ANSI_KeypadDecimal}
   };

   const std::unordered_map<std::string, unsigned char> kANSIKeyMap {
       { "-",        kVK_ANSI_Minus},
       { ",",        kVK_ANSI_Comma},
       { ".",       kVK_ANSI_Period},
       { "/",        kVK_ANSI_Slash},
       { ";",    kVK_ANSI_Semicolon},
       { "[",  kVK_ANSI_LeftBracket},
       { "'",        kVK_ANSI_Quote},
       {"\\",    kVK_ANSI_Backslash},
       { "]", kVK_ANSI_RightBracket},
       { "`",        kVK_ANSI_Grave},
       { "=",        kVK_ANSI_Equal},
       { "0",            kVK_ANSI_0},
       { "1",            kVK_ANSI_1},
       { "2",            kVK_ANSI_2},
       { "3",            kVK_ANSI_3},
       { "4",            kVK_ANSI_4},
       { "5",            kVK_ANSI_5},
       { "6",            kVK_ANSI_6},
       { "7",            kVK_ANSI_7},
       { "8",            kVK_ANSI_8},
       { "9",            kVK_ANSI_9},
       { "a",            kVK_ANSI_A},
       { "b",            kVK_ANSI_B},
       { "c",            kVK_ANSI_C},
       { "d",            kVK_ANSI_D},
       { "e",            kVK_ANSI_E},
       { "f",            kVK_ANSI_F},
       { "g",            kVK_ANSI_G},
       { "h",            kVK_ANSI_H},
       { "i",            kVK_ANSI_I},
       { "j",            kVK_ANSI_J},
       { "k",            kVK_ANSI_K},
       { "l",            kVK_ANSI_L},
       { "m",            kVK_ANSI_M},
       { "n",            kVK_ANSI_N},
       { "o",            kVK_ANSI_O},
       { "p",            kVK_ANSI_P},
       { "q",            kVK_ANSI_Q},
       { "r",            kVK_ANSI_R},
       { "s",            kVK_ANSI_S},
       { "t",            kVK_ANSI_T},
       { "u",            kVK_ANSI_U},
       { "v",            kVK_ANSI_V},
       { "w",            kVK_ANSI_W},
       { "x",            kVK_ANSI_X},
       { "y",            kVK_ANSI_Y},
       { "z",            kVK_ANSI_Z}
   };

   const std::unordered_map<std::string, unsigned char> kANSIKeyMapShifted {
       { "_",        kVK_ANSI_Minus},
       { "<",        kVK_ANSI_Comma},
       { ">",       kVK_ANSI_Period},
       { "?",        kVK_ANSI_Slash},
       { ":",    kVK_ANSI_Semicolon},
       { "{",  kVK_ANSI_LeftBracket},
       {"\"",        kVK_ANSI_Quote},
       { "|",    kVK_ANSI_Backslash},
       { "}", kVK_ANSI_RightBracket},
       { "~",        kVK_ANSI_Grave},
       { "+",        kVK_ANSI_Equal},
       { ")",            kVK_ANSI_0},
       { "!",            kVK_ANSI_1},
       { "@",            kVK_ANSI_2},
       { "#",            kVK_ANSI_3},
       { "$",            kVK_ANSI_4},
       { "%",            kVK_ANSI_5},
       { "^",            kVK_ANSI_6},
       { "&",            kVK_ANSI_7},
       { "*",            kVK_ANSI_8},
       { "(",            kVK_ANSI_9}
   };
} // namespace

void rsj::SendKeyDownUp(const std::string& key, const rsj::ActiveModifiers& mods) noexcept
{
   try {
      Expects(!key.empty());
      static const pid_t lr_pid {GetPid()};
      if (!lr_pid) {
         rsj::LogAndAlertError("Unable to obtain PID for Lightroom in SendKeys.cpp.");
         return;
      }
      CGKeyCode vk {0};
      CGEventFlags flags {0};
      if (const auto mapped_key {kKeyMap.find(rsj::ToLower(key))}; mapped_key != kKeyMap.end()) {
         vk = mapped_key->second;
      }
      else {
         const UniChar uc {ww898::utf::conv<char16_t>(key).front()};
         const auto key_code_result {KeyCodeForChar(uc)};
         if (key_code_result) {
            const auto k_data {*key_code_result};
            vk = k_data.keycode;
            if (k_data.shift) { flags |= kCGEventFlagMaskShift; }
            if (k_data.option) { flags |= kCGEventFlagMaskAlternate; }
         }
         else {
            if (const auto mapped_unshifted_key {kANSIKeyMap.find(rsj::ToLower(key))};
                mapped_unshifted_key != kANSIKeyMap.end()) {
               vk = mapped_unshifted_key->second;
               if (gsl::narrow_cast<char>(std::tolower(static_cast<unsigned char>(key.front())))
                   != key.front())
                  flags |= kCGEventFlagMaskShift;
            }
            else if (const auto mapped_shifted_key {kANSIKeyMapShifted.find(key)};
                     mapped_shifted_key != kANSIKeyMapShifted.end()) {
               vk = mapped_shifted_key->second;
               flags |= kCGEventFlagMaskShift;
            }
            else {
               rsj::LogAndAlertError(fmt::format(
                   FMT_STRING("Unsupported character was used: \"{}\", no ANSI equivalent."), key));
               return;
            }
         }
      }
      if (mods.alt_opt) { flags |= kCGEventFlagMaskAlternate; }
      if (mods.command) { flags |= kCGEventFlagMaskCommand; }
      if (mods.control) { flags |= kCGEventFlagMaskControl; }
      if (mods.shift) { flags |= kCGEventFlagMaskShift; }
      if (!juce::MessageManager::callAsync([vk, flags] { MacKeyDownUp(lr_pid, vk, flags); })) {
         rsj::Log("Unable to post keystroke to message queue.");
      }
   }
   catch (const std::exception& e) {
      rsj::LogAndAlertError(fmt::format(
          FMT_STRING("Exception in key sending function for key: \"{}\". Exception: {}."), key,
          e.what()));
   }
   catch (...) {
      rsj::LogAndAlertError(fmt::format(
          FMT_STRING("Non-standard exception in key sending function for key: \"{}\"."), key));
   }
}
#endif