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
#include "SendKeys.h"

#include <array>
#include <exception>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>
#include <gsl/gsl>
#include <ww898/utf_converters.hpp>

#include "Misc.h"
#ifdef _WIN32
#include <utility>

#include <Windows.h>
#include <wil/result.h>
#else
#include <cstring>
#include <memory>
#include <optional>
#include <type_traits>

#import <CoreGraphics/CoreGraphics.h>
#include <libproc.h> /* proc_ functions in GetPid */

#include <juce_core/juce_core.h>/*creates ambiguous reference to Point if included before Mac headers*/
#include <juce_events/juce_events.h>

#include "Ocpp.h"

#endif

namespace rsj {
   ActiveModifiers ActiveModifiers::FromWindows(const int from) noexcept
   {
      /* shift coded as follows: 1: shift, 2: ctrl, 4: alt, 8: hankaku */
      ActiveModifiers am {};
      am.alt_opt = from & 4;
      am.control = from & 2;
      am.hankaku = from & 8;
      am.shift = from & 1;
      return am;
   }

   ActiveModifiers ActiveModifiers::FromMidi2LR(const int from) noexcept
   {
      ActiveModifiers am {};
      am.alt_opt = from & 1;
      am.command = from & 8;
      am.control = from & 2;
      am.shift = from & 4;
      return am;
   }
} // namespace rsj

namespace {
#ifdef _WIN32

   HWND h_lr_wnd {nullptr};

   BOOL CALLBACK EnumWindowsProc(_In_ const HWND hwnd, [[maybe_unused]] _In_ const LPARAM lParam)
   {
      if (!IsWindowVisible(hwnd))
         return true;
      std::array<WCHAR, 500> buffer {};
      const auto length {GetWindowTextW(hwnd, buffer.data(), gsl::narrow_cast<int>(buffer.size()))};
      if (length) {
         /* check for issues with extra-long window titles and log them */
#ifdef __cpp_lib_integer_comparison_functions
         if (std::cmp_greater_equal(length + 1, buffer.size()))
#else
         if (length + 1 >= gsl::narrow_cast<int>(buffer.size()))
#endif
            rsj::Log(fmt::format(
                FMT_STRING(L"EnumWindowsProc window text length > {}, truncated text is {}."),
                buffer.size(), buffer.data())
                         .data());
         /* try to find Lightroom Classic. Use Lightroom as fallback */
         const auto title {std::wstring_view(buffer.data(), length)};
         const auto lr_start {title.find(L"Lightroom")};
         if (lr_start != std::string_view::npos) {
            h_lr_wnd = hwnd;
            if (title.find(L"Lightroom Classic", lr_start) != std::string_view::npos) {
               return false; /* found full title, stop EnumWindows */
            }
         }
      }
      return true;
   }

   HKL GetLanguage()
   {
      static std::once_flag of;
      std::call_once(of, [] {
         SetLastError(ERROR_SUCCESS);
         EnumWindows(&EnumWindowsProc, 0);
         LOG_IF_WIN32_ERROR(GetLastError());
         if (!h_lr_wnd)
            rsj::Log("Unable to find Lightroom in EnumWindows.");
      });
      if (h_lr_wnd) {
         /* get language that LR is using (if hLrWnd is found) */
         if (const auto thread_id {GetWindowThreadProcessId(h_lr_wnd, nullptr)})
            return GetKeyboardLayout(thread_id);
         h_lr_wnd = nullptr;
         rsj::Log("Unable to find lightroom thread id in GetLanguage.");
      }
      /* EnumWindows failed, use keyboard of MIDI2LR application */
      return GetKeyboardLayout(0);
   }

   std::pair<BYTE, rsj::ActiveModifiers> KeyToVk(std::string_view key)
   {
      try {
         const auto uc {ww898::utf::conv<wchar_t>(key).front()};
         static const auto kLanguageId {GetLanguage()};
         const auto vk_code_and_shift {VkKeyScanExW(uc, kLanguageId)};
         THROW_LAST_ERROR_IF(
             LOBYTE(vk_code_and_shift) == 0xFF && HIBYTE(vk_code_and_shift) == 0xFF);
         return {LOBYTE(vk_code_and_shift),
             rsj::ActiveModifiers::FromWindows(HIBYTE(vk_code_and_shift))};
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
      catch (...) {
         LOG_CAUGHT_EXCEPTION();
         THROW_NORMALIZED_CAUGHT_EXCEPTION();
      }
   }

   /* expects key first, followed by modifiers */
   void WinSendKeyStrokes(const std::vector<WORD>& strokes)
   {
      try {
         /* construct input event. */
         INPUT ip {.type = INPUT_KEYBOARD, .ki = {0, 0, 0, 0, 0}};
         std::vector<INPUT> stroke_vector {};
         const auto push_stroke {[&](const auto stroke) {
            ip.ki.wVk = stroke;
            stroke_vector.push_back(ip);
         }};
         /* down strokes in reverse order from up strokes */
         std::for_each(strokes.crbegin(), strokes.crend(), push_stroke);
         ip.ki.dwFlags = KEYEVENTF_KEYUP;
         std::for_each(strokes.cbegin(), strokes.cend(), push_stroke);
         /* send strokes */
         static std::mutex mutex_sending {};
         auto lock {std::scoped_lock(mutex_sending)};
         THROW_LAST_ERROR_IF(!SendInput(
             gsl::narrow_cast<UINT>(stroke_vector.size()), stroke_vector.data(), sizeof ip));
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
      catch (...) {
         LOG_CAUGHT_EXCEPTION();
         THROW_NORMALIZED_CAUGHT_EXCEPTION();
      }
   }
#else
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

   template<typename T> struct CFDeleter {
      void operator()(T* p)
      {
         if (p)
            ::CFRelease(p);
      }
   };

   template<typename T>
   using CFAutoRelease = std::unique_ptr<typename std::remove_pointer<T>::type,
       CFDeleter<typename std::remove_pointer<T>::type>>;

   pid_t GetPid()
   {
      try {
         static const auto kLrc {".app/Contents/MacOS/Adobe Lightroom Classic"};
         /* add 20 in case more processes show up */
         const int number_processes {proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0) + 20};
         std::vector<pid_t> pids(number_processes, 0);
         proc_listpids(
             PROC_ALL_PIDS, 0, pids.data(), gsl::narrow_cast<int>(sizeof(pids[0]) * pids.size()));
         std::array<char, PROC_PIDPATHINFO_MAXSIZE> path_buffer {};
         for (const auto pid : pids) {
            if (pid == 0)
               continue;
            std::memset(path_buffer.data(), 0, path_buffer.size());
            proc_pidpath(pid, path_buffer.data(), path_buffer.size());
            if (path_buffer[0] && std::strstr(path_buffer.data(), kLrc))
               return pid;
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
         if (result != char_code_map.end())
            return result->second;
         else
            return {};
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
   }

   void MacKeyDownUp(const pid_t lr_pid, const CGKeyCode vk, const CGEventFlags flags)
   {
      try {
         CFAutoRelease<CGEventRef> d {CGEventCreateKeyboardEvent(nullptr, vk, true)};
         CFAutoRelease<CGEventRef> u {CGEventCreateKeyboardEvent(nullptr, vk, false)};
         CGEventSetFlags(d.get(), flags);
         CGEventSetFlags(u.get(), flags);
         CGEventPostToPid(lr_pid, d.get());
         CGEventPostToPid(lr_pid, u.get());
         static std::once_flag of;
         std::call_once(of, [lr_pid]() { rsj::CheckPermission(lr_pid); });
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE_F;
         throw;
      }
   }
#endif

#pragma warning(suppress : 4244 26426) /* assigned to char intentionally */
   const std::unordered_map<std::string, unsigned char> kKeyMap {
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
       /* using ANSI layout codes for keypad, may cause problems in some languages */
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
#pragma warning(disable : 26447) /* all exceptions caught and suppressed */
void rsj::SendKeyDownUp(const std::string& key, const rsj::ActiveModifiers& mods) noexcept
{
   try {
      Expects(!key.empty());
      const auto mapped_key {kKeyMap.find(rsj::ToLower(key))};
      const auto in_keymap {mapped_key != kKeyMap.end()};
#ifdef _WIN32
      BYTE vk {0};
      rsj::ActiveModifiers vk_mod {};
      if (in_keymap)
         vk = mapped_key->second;
      else
         /* Translate key code to keyboard-dependent scan code, may be UTF-8 */
         std::tie(vk, vk_mod) = KeyToVk(key);
      /* construct virtual keystroke sequence */
      std::vector<WORD> strokes {vk};
      /* start with actual key, then mods */
      if (mods.shift || vk_mod.shift)
         strokes.push_back(VK_SHIFT);
      if (vk_mod.control && vk_mod.alt_opt) {
         strokes.push_back(VK_RMENU); /* AltGr */
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
       * SEE:https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values */
      /* if (vk_mod.hankaku)
         strokes.push_back(VK_OEM_AUTO); */
      WinSendKeyStrokes(strokes);
#else
      static const pid_t lr_pid {GetPid()};
      if (!lr_pid) {
         rsj::LogAndAlertError("Unable to obtain PID for Lightroom in SendKeys.cpp.");
         return;
      }
      CGKeyCode vk {0};
      CGEventFlags flags {0};
      if (in_keymap) {
         vk = mapped_key->second;
      }
      else {
         const UniChar uc {ww898::utf::conv<char16_t>(key).front()};
         const auto key_code_result {KeyCodeForChar(uc)};
         if (!key_code_result) {
            rsj::LogAndAlertError(
                fmt::format(FMT_STRING("Unsupported character was used: \"{}\"."), key));
            return;
         }
         const auto k_data {*key_code_result};
         vk = k_data.keycode;
         if (k_data.shift)
            flags |= kCGEventFlagMaskShift;
         if (k_data.option)
            flags |= kCGEventFlagMaskAlternate;
      }
      if (mods.alt_opt)
         flags |= kCGEventFlagMaskAlternate;
      if (mods.command)
         flags |= kCGEventFlagMaskCommand;
      if (mods.control)
         flags |= kCGEventFlagMaskControl;
      if (mods.shift)
         flags |= kCGEventFlagMaskShift;
      if (!juce::MessageManager::callAsync([vk, flags] { MacKeyDownUp(lr_pid, vk, flags); }))
         rsj::Log("Unable to post keystroke to message queue.");
#endif
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
#pragma warning(pop)