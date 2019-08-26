/*
==============================================================================

SendKeys.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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

#include <exception>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Misc.h"
#ifdef _WIN32
#include <utility>

#include "WinDef.h"
#undef NOUSER
#undef NOVIRTUALKEYCODES
#include <Windows.h>
#else
#include <optional>

#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Carbon/Carbon.h>
#include <libproc.h> //proc_ functions in GetPid
#include "Ocpp.h"
#include <JuceLibraryCode/JuceHeader.h> //creates ambiguous reference to Point if included before Mac headers
#include <gsl/gsl>
#endif

namespace rsj {
   ActiveModifiers ActiveModifiers::FromWindows(int from) noexcept
   { // shift coded as follows:
      // 1: shift, 2: ctrl, 4: alt, 8: hankaku
      ActiveModifiers am{};
      am.alt_opt = from & 4;
      am.control = from & 2;
      am.hankaku = from & 8;
      am.shift = from & 1;
      return am;
   }

   ActiveModifiers ActiveModifiers::FromMidi2LR(int from) noexcept
   {
      ActiveModifiers am{};
      am.alt_opt = from & 1;
      am.command = from & 8;
      am.control = from & 2;
      am.shift = from & 4;
      return am;
   }
} // namespace rsj

namespace {
#ifdef _WIN32
   HKL GetLanguage(const std::string& program_name)
   {
      const auto h_lr_wnd = FindWindowA(nullptr, program_name.c_str());
      if (h_lr_wnd) { // get language that LR is using (if hLrWnd is found)
         const auto thread_id = GetWindowThreadProcessId(h_lr_wnd, nullptr);
         return GetKeyboardLayout(thread_id);
      }
      // FindWindowA failed
      const auto error_msg =
          "FindWindowA failed with error code: " + std::to_string(GetLastError());
      rsj::Log(error_msg);
      // use keyboard of MIDI2LR application
      return GetKeyboardLayout(0);
   }

   SHORT VkKeyScanExWErrorChecked(_In_ WCHAR ch, _In_ HKL dwhkl)
   {
      try {
         const auto vk_code_and_shift = VkKeyScanExW(ch, dwhkl);
         if (vk_code_and_shift == 0xffff) { //-V547
            const auto error_msg =
                "VkKeyScanExW failed with error code: " + std::to_string(GetLastError());
            throw std::runtime_error(error_msg.c_str());
         }
         return vk_code_and_shift;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   std::pair<BYTE, rsj::ActiveModifiers> KeyToVk(std::string_view key)
   {
      try {
         const auto uc{rsj::Utf8ToWide(key).at(0)};
         static const auto kLanguageId = GetLanguage("Lightroom");
         const auto vk_code_and_shift = VkKeyScanExWErrorChecked(uc, kLanguageId);
         return {LOBYTE(vk_code_and_shift),
             rsj::ActiveModifiers::FromWindows(HIBYTE(vk_code_and_shift))};
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   UINT SendInputErrorChecked(
       _In_ UINT cinputs, _In_reads_(cinputs) LPINPUT pinputs, _In_ int cbSize)
   {
      try {
         const auto result = SendInput(cinputs, pinputs, cbSize);
         if (result == 0) {
            const auto error_msg =
                "SendInput failed with error code: " + std::to_string(GetLastError());
            throw std::runtime_error(error_msg.c_str());
         }
         return result;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   // expects key first, followed by modifiers
   void WinSendKeyStrokes(const std::vector<WORD>& strokes)
   {
      try {
         // construct input event.
         INPUT ip{};
         constexpr int size_ip = sizeof ip;
         ip.type = INPUT_KEYBOARD;
         // ki: wVk, wScan, dwFlags, time, dwExtraInfo
         ip.ki = {0, 0, 0, 0, 0};

         // send key down strokes
         static std::mutex mutex_sending{};
         auto lock = std::scoped_lock(mutex_sending);
         for (const auto it : rsj::Reverse(strokes)) {
            ip.ki.wVk = it;
            SendInputErrorChecked(1, &ip, size_ip);
         }
         // send key up strokes
         ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
         for (const auto it : strokes) {
            ip.ki.wVk = it;
            SendInputErrorChecked(1, &ip, size_ip);
         }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }
#else
   pid_t GetPid()
   {
      try {
         static const std::string kLr{".app/Contents/MacOS/Adobe Lightroom"};
         static const std::string kLrc{".app/Contents/MacOS/Adobe Lightroom Classic"};
         const int number_processes{proc_listpids(PROC_ALL_PIDS, 0, NULL, 0) + 20};
         std::vector<pid_t> pids(number_processes, 0); // add a few in case more processes show up
         proc_listpids(
             PROC_ALL_PIDS, 0, pids.data(), gsl::narrow_cast<int>(sizeof(pids[0]) * pids.size()));
         char path_buffer[PROC_PIDPATHINFO_MAXSIZE];
         for (const auto pid : pids) {
            if (pid == 0)
               continue;
            memset(path_buffer, 0, sizeof(path_buffer));
            proc_pidpath(pid, path_buffer, sizeof(path_buffer));
            if (strlen(path_buffer) > 0
                && (rsj::EndsWith(path_buffer, kLr) || rsj::EndsWith(path_buffer, kLrc)))
               return pid;
         }
         rsj::LogAndAlertError("Lightroom PID not found.");
         return 0;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   // See https://github.com/Microsoft/node-native-keymap src/keyboard_mac.mm
   // for issue with Japanese keyboards
   const UCKeyboardLayout* GetKeyboardData()
   {
      TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
      CFDataRef data =
          (CFDataRef)TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData);
      if (source)
         CFRelease(source);
      if (!data) { // returns null with Japanese keyboard layout
         source = TISCopyCurrentKeyboardLayoutInputSource();
         data = (CFDataRef)TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData);
         if (source)
            CFRelease(source);
         if (!data)
            return nullptr;
      }
      return (const UCKeyboardLayout*)CFDataGetBytePtr(data);
   }

   /* Altered significantly, originally from:
    * https://stackoverflow.com/questions/1918841/how-to-convert-ascii-character-to-cgkeycode/1971027#1971027
    *
    * Returns unshifted and shifted UniChar (UTF-16) for each key code
    * Zero return for character indicates it should be ignored (duplicate code or error)
    * Ignores characters requiring dead key */
   std::pair<UniChar, UniChar> CreateStringForKey(CGKeyCode key_code)
   {
      try {
         static const UCKeyboardLayout* keyboard_layout{GetKeyboardData()};
         if (!keyboard_layout) {
            static auto notwarned{true};
            if (notwarned) {
               notwarned = false;
               rsj::LogAndAlertError("Keyboard layout is null. Cannot send keystrokes.");
            }
            return {0, 0};
         }
         UInt32 keys_down = 0;
         UniChar chars[4];
         // unshifted
         UniCharCount real_length;
         UCKeyTranslate(keyboard_layout, key_code, kUCKeyActionDown, 0, LMGetKbdType(),
             kUCKeyTranslateNoDeadKeysMask, &keys_down, sizeof(chars) / sizeof(chars[0]),
             &real_length, chars);
         if (real_length != 1) {
            if (real_length > 1)
               rsj::Log(juce::String("For key code ") + juce::String(key_code)
                        + juce::String(", Unicode character is ") + juce::String(real_length)
                        + juce::String(" long. It starts with ") + juce::String(chars[0])
                        + juce::String("."));
            chars[0] = 0;
         }
         // shifted
         UInt32 s_keys_down = 0;
         UniChar s_chars[4];
         UniCharCount s_real_length;
         // 2 == left shift key, 4 == right shift key
         UCKeyTranslate(keyboard_layout, key_code, kUCKeyActionDown, 2, LMGetKbdType(),
             kUCKeyTranslateNoDeadKeysMask, &s_keys_down, sizeof(s_chars) / sizeof(s_chars[0]),
             &s_real_length, s_chars);
         if (s_real_length != 1) {
            if (s_real_length > 1)
               rsj::Log(juce::String("For shifted key code ") + juce::String(key_code)
                        + juce::String(", Unicode character is ") + juce::String(s_real_length)
                        + juce::String(" long. It starts with ") + juce::String(s_chars[0])
                        + juce::String("."));
            s_chars[0] = 0;
         }
         if (chars[0] == s_chars[0])
            s_chars[0] = 0; // if unshifted and shifted same, only return unshifted
         return {chars[0], s_chars[0]};
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   // initializes unordered map for KeyCodeForChar
   // UCKeyTranslate returns the same UniChar for several different (typically unused) key codes, so
   // will only add first one
   std::unordered_map<UniChar, std::pair<size_t, bool>> MakeMap()
   {
      try {
         std::unordered_map<UniChar, std::pair<size_t, bool>> temp_map{};
         for (size_t i = 0; i < 128; ++i) {
            const auto uc = CreateStringForKey((CGKeyCode)i);
            if (uc.first && !temp_map.count(uc.first))
               temp_map[uc.first] = {i, false};
            if (uc.second && !temp_map.count(uc.second))
               temp_map[uc.second] = {i, true};
         }
         return temp_map;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   /*
    * Returns key code for given character via the above function.
    * Bool in pair represents shift key
    */
   std::optional<std::pair<CGKeyCode, bool>> KeyCodeForChar(UniChar c)
   {
      try {
         static const std::unordered_map<UniChar, std::pair<size_t, bool>> char_code_map{MakeMap()};
         const auto result = char_code_map.find(c);
         if (result != char_code_map.end())
            return result->second;
         else
            return {};
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   void MacKeyDownUp(pid_t lr_pid, CGKeyCode vk, CGEventFlags flags)
   {
      try {
         CGEventRef d = CGEventCreateKeyboardEvent(NULL, vk, true);
         auto dd = gsl::finally([&d] {
            if (d)
               CFRelease(d);
         });
         CGEventRef u = CGEventCreateKeyboardEvent(NULL, vk, false);
         auto du = gsl::finally([&u] {
            if (u)
               CFRelease(u);
         });
         CGEventSetFlags(d, flags);
         CGEventSetFlags(u, flags);
         { // scope for the mutex
            static std::mutex mtx{};
            auto lock = std::scoped_lock(mtx);
            CGEventPostToPid(lr_pid, d);
            CGEventPostToPid(lr_pid, u);
         }
         static std::once_flag of;
         std::call_once(of, [lr_pid]() { rsj::CheckPermission(lr_pid); });
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }
#endif

#pragma warning(suppress : 4244 26426) // assigned to char intentionally
   const std::unordered_map<std::string, unsigned char> kKeyMap = {
#ifdef _WIN32
       {"backspace", VK_BACK}, {"cursor down", VK_DOWN}, {"cursor left", VK_LEFT},
       {"cursor right", VK_RIGHT}, {"cursor up", VK_UP}, {"delete", VK_DELETE}, {"end", VK_END},
       {"escape", VK_ESCAPE}, {"home", VK_HOME}, {"page down", VK_NEXT}, {"page up", VK_PRIOR},
       {"return", VK_RETURN}, {"space", VK_SPACE}, {"tab", VK_TAB}, {"f1", VK_F1}, {"f2", VK_F2},
       {"f3", VK_F3}, {"f4", VK_F4}, {"f5", VK_F5}, {"f6", VK_F6}, {"f7", VK_F7}, {"f8", VK_F8},
       {"f9", VK_F9}, {"f10", VK_F10}, {"f11", VK_F11}, {"f12", VK_F12}, {"f13", VK_F13},
       {"f14", VK_F14}, {"f15", VK_F15}, {"f16", VK_F16}, {"f17", VK_F17}, {"f18", VK_F18},
       {"f19", VK_F19}, {"f20", VK_F20}, {"numpad 0", VK_NUMPAD0}, {"numpad 1", VK_NUMPAD1},
       {"numpad 2", VK_NUMPAD2}, {"numpad 3", VK_NUMPAD3}, {"numpad 4", VK_NUMPAD4},
       {"numpad 5", VK_NUMPAD5}, {"numpad 6", VK_NUMPAD6}, {"numpad 7", VK_NUMPAD7},
       {"numpad 8", VK_NUMPAD8}, {"numpad 9", VK_NUMPAD9}, {"numpad add", VK_ADD},
       {"numpad subtract", VK_SUBTRACT}, {"numpad multiply", VK_MULTIPLY},
       {"numpad divide", VK_DIVIDE}, {"numpad decimal", VK_DECIMAL}
#else
       {"backspace", kVK_Delete}, {"cursor down", kVK_DownArrow}, {"cursor left", kVK_LeftArrow},
       {"cursor right", kVK_RightArrow}, {"cursor up", kVK_UpArrow}, {"delete", kVK_ForwardDelete},
       {"end", kVK_End}, {"escape", kVK_Escape}, {"home", kVK_Home}, {"page down", kVK_PageDown},
       {"page up", kVK_PageUp}, {"return", kVK_Return}, {"space", kVK_Space}, {"tab", kVK_Tab},
       {"f1", kVK_F1}, {"f2", kVK_F2}, {"f3", kVK_F3}, {"f4", kVK_F4}, {"f5", kVK_F5},
       {"f6", kVK_F6}, {"f7", kVK_F7}, {"f8", kVK_F8}, {"f9", kVK_F9}, {"f10", kVK_F10},
       {"f11", kVK_F11}, {"f12", kVK_F12}, {"f13", kVK_F13}, {"f14", kVK_F14}, {"f15", kVK_F15},
       {"f16", kVK_F16}, {"f17", kVK_F17}, {"f18", kVK_F18}, {"f19", kVK_F19}, {"f20", kVK_F20},
       // using ANSI layout codes for keypad, may cause problems in some languages
       {"numpad 0", kVK_ANSI_Keypad0}, {"numpad 1", kVK_ANSI_Keypad1},
       {"numpad 2", kVK_ANSI_Keypad2}, {"numpad 3", kVK_ANSI_Keypad3},
       {"numpad 4", kVK_ANSI_Keypad4}, {"numpad 5", kVK_ANSI_Keypad5},
       {"numpad 6", kVK_ANSI_Keypad6}, {"numpad 7", kVK_ANSI_Keypad7},
       {"numpad 8", kVK_ANSI_Keypad8}, {"numpad 9", kVK_ANSI_Keypad9},
       {"numpad add", kVK_ANSI_KeypadPlus}, {"numpad subtract", kVK_ANSI_KeypadMinus},
       {"numpad multiply", kVK_ANSI_KeypadMultiply}, {"numpad divide", kVK_ANSI_KeypadDivide},
       {"numpad decimal", kVK_ANSI_KeypadDecimal}
#endif
   };
} // namespace

#pragma warning(push)
#pragma warning(disable : 26447) // all exceptions caught and suppressed
void rsj::SendKeyDownUp(const std::string& key, rsj::ActiveModifiers mods) noexcept
{
   try {
      const auto mapped_key = kKeyMap.find(rsj::ToLower(key));
      const auto in_keymap = mapped_key != kKeyMap.end();
#ifdef _WIN32
      BYTE vk = 0;
      rsj::ActiveModifiers vk_mod{};
      if (in_keymap)
         vk = mapped_key->second;
      else // Translate key code to keyboard-dependent scan code, may be UTF-8
         std::tie(vk, vk_mod) = KeyToVk(key);
      // construct virtual keystroke sequence
      std::vector<WORD> strokes{vk}; // start with actual key, then mods
      if (mods.shift || vk_mod.shift)
         strokes.push_back(VK_SHIFT);
      if (vk_mod.control && vk_mod.alt_opt) {
         strokes.push_back(VK_RMENU); // AltGr
         if (mods.alt_opt)
            strokes.push_back(VK_MENU);
         if (mods.control)
            strokes.push_back(VK_CONTROL);
      }
      else {
         if (mods.alt_opt || vk_mod.alt_opt)
            strokes.push_back(VK_MENU);
         if (mods.control || vk_mod.control)
            strokes.push_back(VK_CONTROL);
      }
      /* ignored for now
      // see https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values
      if (vk_mod.hankaku)
         strokes.push_back(VK_OEM_AUTO);*/
      WinSendKeyStrokes(strokes);
#else
      static const pid_t lr_pid{GetPid()};
      if (!lr_pid) {
         rsj::LogAndAlertError("Unable to obtain PID for Lightroom in SendKeys.cpp");
         return;
      }
      CGKeyCode vk{0};
      CGEventFlags flags{0};
      if (in_keymap) {
         vk = mapped_key->second;
      }
      else {
         const UniChar uc{rsj::Utf8ToUtf16(key)};
         const auto key_code_result = KeyCodeForChar(uc);
         if (!key_code_result) {
            rsj::LogAndAlertError("Unsupported character was used: " + key);
            return;
         }
         vk = key_code_result->first;
         if (key_code_result->second)
            flags |= kCGEventFlagMaskShift;
      }
      if (mods.alt_opt)
         flags |= kCGEventFlagMaskAlternate;
      if (mods.command)
         flags |= kCGEventFlagMaskCommand;
      if (mods.control)
         flags |= kCGEventFlagMaskControl;
      if (mods.shift)
         flags |= kCGEventFlagMaskShift;
      MacKeyDownUp(lr_pid, vk, flags);
#endif
   }
   catch (const std::exception& e) {
      rsj::LogAndAlertError("Exception in key sending function for key: " + key + ". " + e.what());
   }
   catch (...) {
      rsj::LogAndAlertError("Non-standard exception in key sending function for key: " + key + ".");
   }
}
#pragma warning(pop)