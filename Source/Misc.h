#ifndef MIDI2LR_MISC_H_INCLUDED
#define MIDI2LR_MISC_H_INCLUDED
/*
==============================================================================

Misc.h

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
#include <array>
#include <charconv>
#include <chrono>
#include <exception>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <string_view>
#include <system_error>
#include <thread>   //sleep_for
#include <typeinfo> //for typeid, used in calls to ExceptionResponse

#include <JuceLibraryCode/JuceHeader.h>

#ifdef NDEBUG // asserts disabled
static constexpr bool kNdebug = true;
#else // asserts enabled
static constexpr bool kNdebug = false;
#endif

#ifdef _WIN32
constexpr auto MSWindows{true};
constexpr auto OSX{false};
#else
constexpr auto MSWindows{false};
constexpr auto OSX{true};
// Microsoft SAL annotation
#define _In_z_
#endif

namespace rsj {
   [[nodiscard]] std::string ReplaceInvisibleChars(std::string_view input);
   [[nodiscard]] bool EndsWith(std::string_view main_str, std::string_view to_match);
   // typical call: rsj::ExceptionResponse(typeid(this).name(), __func__, e);
   void ExceptionResponse(
       _In_z_ const char* id, _In_z_ const char* fu, const std::exception& e) noexcept;
   void LogAndAlertError(const juce::String& error_text);
   void Log(const juce::String& info);
   [[nodiscard]] std::string ToLower(std::string_view in);
#ifdef _WIN32
   [[nodiscard]] std::wstring AppDataFilePath(std::wstring_view file_name);
   [[nodiscard]] std::wstring AppDataFilePath(std::string_view file_name);
   [[nodiscard]] inline std::wstring AppLogFilePath(const std::wstring& file_name)
   {
      return AppDataFilePath(file_name);
   }
   [[nodiscard]] inline std::wstring AppLogFilePath(const std::string& file_name)
   {
      return AppDataFilePath(file_name);
   }
   [[nodiscard]] std::wstring Utf8ToWide(std::string_view input);
   [[nodiscard]] std::string WideToUtf8(std::wstring_view wstr);
#else
   [[nodiscard]] std::string AppDataFilePath(const std::string& file_name);
   [[nodiscard]] std::string AppLogFilePath(const std::string& file_name);
#endif // def _WIN32

   /* additional classes/templates in Misc.h
    * Reverse
    * RatioToPrefix
    * SleepTimed
    * SleepTimedLogged
    * NumToChars
    * */



   // -------------------------------------------------------------------
   // --- Reversed iterable
   // https://stackoverflow.com/a/28139075/5699329
   // Note that this won't properly capture an rvalue container (temporary)
   // see https://stackoverflow.com/a/42221253/5699329 for that solution

   // ReSharper disable once CppImplicitDefaultConstructorNotAvailable
   template<typename T> struct ReversionWrapper {
      T& iterable;
   };

   template<typename T> auto begin(ReversionWrapper<T> w)
   {
      return std::rbegin(w.iterable);
   }

   template<typename T> auto end(ReversionWrapper<T> w)
   {
      return std::rend(w.iterable);
   }

   template<typename T> ReversionWrapper<T> Reverse(T&& iterable)
   {
      return {iterable};
   }

#pragma warning(push)
#pragma warning(disable : 4127) // constant conditional expression
   // zepto yocto zetta and yotta too large/small to be represented by intmax_t
   // TODO: change to consteval, find way to convert digit to string for unexpected
   // values, so return could be, e.g., "23425/125557 ", instead of error message
   template<class R>[[nodiscard]] constexpr auto RatioToPrefix()
   {
      if (R::num == 1) {
         switch (R::den) {
         case 1:
            return "";
         case 10:
            return "deci";
         case 100:
            return "centi";
         case 1000:
            return "milli";
         case 1000000:
            return "micro";
         case 1000000000:
            return "nano";
         case 1000000000000:
            return "pico";
         case 1000000000000000:
            return "femto";
         case 1000000000000000000:
            return "atto";
         default:
             /* empty */;
         }
      }
      if (R::den == 1) {
         switch (R::num) {
         case 10:
            return "deca";
         case 100:
            return "hecto";
         case 1000:
            return "kilo";
         case 1000000:
            return "mega";
         case 1000000000:
            return "giga";
         case 1000000000000:
            return "tera";
         case 1000000000000000:
            return "peta";
         case 1000000000000000000:
            return "exa";
         default:
             /* empty */;
         }
      }
      return "unexpected ratio encountered ";
   }
#pragma warning(pop)

   template<class Rep, class Period>
   auto SleepTimed(const std::chrono::duration<Rep, Period> sleep_duration) //-V801
   {
      const auto start = std::chrono::high_resolution_clock::now();
      std::this_thread::sleep_for(sleep_duration);
      const auto end = std::chrono::high_resolution_clock::now();
      const std::chrono::duration<double, Period> elapsed = end - start;
      return elapsed;
   }

   template<class Rep, class Period>
   void SleepTimedLogged(
       std::string_view msg_prefix, const std::chrono::duration<Rep, Period> sleep_duration) //-V801
   {
      const auto elapsed = SleepTimed(sleep_duration);
      rsj::Log(juce::String(msg_prefix.data(), msg_prefix.size()) + " thread slept for "
               + juce::String(elapsed.count()) + ' ' + RatioToPrefix<Period>() + "seconds.");
   }

   template<class T>[[nodiscard]] std::string NumToChars(T number)
   {
      std::array<char, 10> str{};
      auto [p, ec] = std::to_chars(str.data(), str.data() + str.size(), number);
      if (ec == std::errc())
         return std::string(str.data(), p - str.data());
      return "Number conversion error " + std::make_error_condition(ec).message();
   }



} // namespace rsj

#endif // MISC_H_INCLUDED
