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
#ifdef _WIN32
#include <array>
#include <exception>
#include <mutex>
#include <ranges>
#include <string_view>
#include <system_error>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Windows.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#include <gsl/gsl>
#include <wil/result.h>
#include <ww898/utf_converters.hpp>

#include "Misc.h"
#include "SendKeys.h"

namespace {
   std::mutex mutex_sending {};
   HWND h_lr_wnd {nullptr};

   BOOL CALLBACK EnumWindowsProc(const _In_ HWND hwnd, [[maybe_unused]] const _In_ LPARAM l_param)
   {
      if (!IsWindowVisible(hwnd)) { return TRUE; }
      std::array<WCHAR, 500> buffer {};
      const auto length {GetWindowTextW(hwnd, buffer.data(), gsl::narrow_cast<int>(buffer.size()))};
      if (length) {
         /* check for issues with extra-long window titles and log them */
         if (std::cmp_greater_equal(length + 1, buffer.size())) {
            rsj::Log(fmt::format(L"EnumWindowsProc window text length > {}, truncated text is {}.",
                         buffer.size(), buffer.data())
                         .data(),
                std::source_location::current());
         } /* try to find Lightroom Classic. Use Lightroom as fallback */
         const auto title {std::wstring_view(buffer.data(), length)};
         const auto lr_start {title.find(L"Lightroom")};
         if (lr_start != std::wstring_view::npos) {
            h_lr_wnd = hwnd;
            if (title.find(L"Adobe Photoshop Lightroom Classic", lr_start)
                != std::wstring_view::npos) {
               return FALSE; /* found full title, stop EnumWindows */
            }
         }
      }
      return TRUE;
   }

   HKL GetLanguage()
   {
      constinit static std::once_flag of_getlanguage;
      std::call_once(of_getlanguage, [] {
         EnumWindows(&EnumWindowsProc, 0);
         if (!h_lr_wnd) {
            rsj::Log("Unable to find Lightroom in EnumWindows.", std::source_location::current());
         }
      });
      if (h_lr_wnd) { /* get language that LR is using (if hLrWnd is found) */
         if (const auto thread_id {GetWindowThreadProcessId(h_lr_wnd, nullptr)}) {
            return GetKeyboardLayout(thread_id);
         }
         h_lr_wnd = nullptr;
         rsj::Log("Unable to find lightroom thread id in GetLanguage.",
             std::source_location::current());
      }
      /* EnumWindows failed, use keyboard of MIDI2LR application */
      return GetKeyboardLayout(0);
   }

   std::pair<BYTE, rsj::ActiveModifiers> KeyToVk(std::string_view key)
   {
      try {
         const auto wstr = ww898::utf::conv<wchar_t>(key);
         if (wstr.empty()) { throw std::runtime_error("KeyToVk: empty key after conversion"); }
         const auto uc = wstr.front();
         static const auto kLanguageId {GetLanguage()};
         const SHORT vk_code_and_shift = VkKeyScanExW(uc, kLanguageId);
         if (vk_code_and_shift == static_cast<SHORT>(-1)) {
            // no translation for this character in the current layout
            throw std::system_error(std::error_code(GetLastError(), std::system_category()),
                "VkKeyScanExW failed to map character");
         }
         const BYTE vk = LOBYTE(vk_code_and_shift);
         const BYTE shiftState = HIBYTE(vk_code_and_shift);
         return {vk, rsj::ActiveModifiers::FromWindows(shiftState)};
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
         throw;
      }
      catch (...) {
         std::ignore = LOG_CAUGHT_EXCEPTION();
         THROW_NORMALIZED_CAUGHT_EXCEPTION();
      }
   }

   /* expects key first, followed by modifiers */
   void WinSendKeyStrokes(const std::vector<WORD>& strokes)
   {
      try {
         if (strokes.empty()) { return; }

         /* construct INPUT template */
         INPUT ip {};
         ip.type = INPUT_KEYBOARD;
         ip.ki.wScan = 0;
         ip.ki.time = 0;
         ip.ki.dwExtraInfo = 0;

         std::vector<INPUT> stroke_vector;
         stroke_vector.reserve(strokes.size() * 2);

         const auto push_stroke = [&](WORD stroke, DWORD flags) {
            INPUT entry = ip;
            entry.ki.wVk = stroke;
            entry.ki.dwFlags = flags;
            stroke_vector.push_back(entry);
         };

         /* down strokes in reverse order from up strokes (modifiers pressed before key) */
         for (const auto s : std::ranges::reverse_view(strokes)) { push_stroke(s, 0); }

         /* up strokes: release primary key first, then modifiers (forward order) */
         for (const auto s : strokes) { push_stroke(s, KEYEVENTF_KEYUP); }

         /* send strokes */
         auto lock {std::scoped_lock(mutex_sending)};
         THROW_LAST_ERROR_IF(SendInput(gsl::narrow_cast<UINT>(stroke_vector.size()),
                                 stroke_vector.data(), sizeof(INPUT))
                             == 0);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
         throw;
      }
      catch (...) {
         std::ignore = LOG_CAUGHT_EXCEPTION();
         THROW_NORMALIZED_CAUGHT_EXCEPTION();
      }
   }

#pragma warning(suppress : 4244 26426) /* assigned to char intentionally */
   const std::unordered_map<std::string, unsigned char> kKeyMap {
       {      "backspace",     static_cast<unsigned char>(VK_BACK)},
       {    "cursor down",     static_cast<unsigned char>(VK_DOWN)},
       {    "cursor left",     static_cast<unsigned char>(VK_LEFT)},
       {   "cursor right",    static_cast<unsigned char>(VK_RIGHT)},
       {      "cursor up",       static_cast<unsigned char>(VK_UP)},
       {         "delete",   static_cast<unsigned char>(VK_DELETE)},
       {            "end",      static_cast<unsigned char>(VK_END)},
       {         "escape",   static_cast<unsigned char>(VK_ESCAPE)},
       {           "home",     static_cast<unsigned char>(VK_HOME)},
       {      "page down",     static_cast<unsigned char>(VK_NEXT)},
       {        "page up",    static_cast<unsigned char>(VK_PRIOR)},
       {         "return",   static_cast<unsigned char>(VK_RETURN)},
       {          "space",    static_cast<unsigned char>(VK_SPACE)},
       {            "tab",      static_cast<unsigned char>(VK_TAB)},
       {             "f1",       static_cast<unsigned char>(VK_F1)},
       {             "f2",       static_cast<unsigned char>(VK_F2)},
       {             "f3",       static_cast<unsigned char>(VK_F3)},
       {             "f4",       static_cast<unsigned char>(VK_F4)},
       {             "f5",       static_cast<unsigned char>(VK_F5)},
       {             "f6",       static_cast<unsigned char>(VK_F6)},
       {             "f7",       static_cast<unsigned char>(VK_F7)},
       {             "f8",       static_cast<unsigned char>(VK_F8)},
       {             "f9",       static_cast<unsigned char>(VK_F9)},
       {            "f10",      static_cast<unsigned char>(VK_F10)},
       {            "f11",      static_cast<unsigned char>(VK_F11)},
       {            "f12",      static_cast<unsigned char>(VK_F12)},
       {            "f13",      static_cast<unsigned char>(VK_F13)},
       {            "f14",      static_cast<unsigned char>(VK_F14)},
       {            "f15",      static_cast<unsigned char>(VK_F15)},
       {            "f16",      static_cast<unsigned char>(VK_F16)},
       {            "f17",      static_cast<unsigned char>(VK_F17)},
       {            "f18",      static_cast<unsigned char>(VK_F18)},
       {            "f19",      static_cast<unsigned char>(VK_F19)},
       {            "f20",      static_cast<unsigned char>(VK_F20)},
       {       "numpad 0",  static_cast<unsigned char>(VK_NUMPAD0)},
       {       "numpad 1",  static_cast<unsigned char>(VK_NUMPAD1)},
       {       "numpad 2",  static_cast<unsigned char>(VK_NUMPAD2)},
       {       "numpad 3",  static_cast<unsigned char>(VK_NUMPAD3)},
       {       "numpad 4",  static_cast<unsigned char>(VK_NUMPAD4)},
       {       "numpad 5",  static_cast<unsigned char>(VK_NUMPAD5)},
       {       "numpad 6",  static_cast<unsigned char>(VK_NUMPAD6)},
       {       "numpad 7",  static_cast<unsigned char>(VK_NUMPAD7)},
       {       "numpad 8",  static_cast<unsigned char>(VK_NUMPAD8)},
       {       "numpad 9",  static_cast<unsigned char>(VK_NUMPAD9)},
       {     "numpad add",      static_cast<unsigned char>(VK_ADD)},
       {"numpad subtract", static_cast<unsigned char>(VK_SUBTRACT)},
       {"numpad multiply", static_cast<unsigned char>(VK_MULTIPLY)},
       {  "numpad divide",   static_cast<unsigned char>(VK_DIVIDE)},
       { "numpad decimal",  static_cast<unsigned char>(VK_DECIMAL)}
   };
} // namespace

#pragma warning(push)
#pragma warning(disable : 26447) /* all exceptions caught and suppressed */

void rsj::SendKeyDownUp(const std::string& key, const rsj::ActiveModifiers mods) noexcept
{
   try {
      Expects(!key.empty());
      BYTE vk {0};
      rsj::ActiveModifiers vk_mod {};
      if (const auto mapped_key {kKeyMap.find(rsj::ToLower(key))}; mapped_key != kKeyMap.end()) {
         vk = mapped_key->second;
      }
      else { /* Translate key code to keyboard-dependent scan code, may be UTF-8 */
         std::tie(vk, vk_mod) = KeyToVk(key);
      } /* construct virtual keystroke sequence */
      std::vector<WORD> strokes {vk};
      strokes.reserve(5); /* maximum possible size */
      /* start with actual key, then mods */
      if (mods.shift || vk_mod.shift) { strokes.push_back(VK_SHIFT); }
      if (vk_mod.control && vk_mod.alt_opt) {
         strokes.push_back(VK_RMENU); /* AltGr */
         if (mods.alt_opt) { strokes.push_back(VK_MENU); }
         if (mods.control) { strokes.push_back(VK_CONTROL); }
      }
      else {
         if (mods.alt_opt || vk_mod.alt_opt) { strokes.push_back(VK_MENU); }
         if (mods.control || vk_mod.control) { strokes.push_back(VK_CONTROL); }
      }
      /* ignored for now
       * SEE:https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values */
      /* if (vk_mod.hankaku)
         strokes.push_back(VK_OEM_AUTO); */
      WinSendKeyStrokes(strokes);
   }
   catch (const std::exception& e) {
      rsj::LogAndAlertError(fmt::format("Exception in key sending function for key: \"{}\". "
                                        "Exception: {}.",
                                key, e.what()),
          std::source_location::current());
   }
   catch (...) {
      rsj::LogAndAlertError(
          fmt::format("Non-standard exception in key sending function for key: \"{}\".", key),
          std::source_location::current());
   }
}

#pragma warning(pop)
#endif