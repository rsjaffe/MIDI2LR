#ifndef MIDI2LR_MISC_H_INCLUDED
#define MIDI2LR_MISC_H_INCLUDED
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
//-V813_MINSIZE=13 /* warn if passing structure by value > 12 bytes (3*sizeof(int)) */

#include <chrono>
#include <exception>
#include <optional>
#ifdef __cpp_lib_source_location
#include <source_location>
#endif
#include <string>
#include <string_view>
#include <thread> /* sleep_for */

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <gsl/gsl>
#include <xmmintrin.h> /* for rounding intrinsics */

#include <juce_core/juce_core.h>

#ifdef NDEBUG /* asserts disabled */
static constexpr bool kNdebug {true};
#else
static constexpr bool kNdebug {false};
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MIDI2LR_FUNC __PRETTY_FUNCTION__
#else
#define MIDI2LR_FUNC __func__
#endif

#ifdef _WIN32
constexpr auto MSWindows {true};
constexpr auto MacOS {false};
#else
constexpr auto MSWindows {false};
constexpr auto MacOS {true};
#endif

namespace rsj {
   std::optional<juce::String> SystemFont();
   /*****************************************************************************/
   /**************Thread Labels**************************************************/
   /*****************************************************************************/
#ifndef NDEBUG
#ifdef _WIN32
   void LabelThread(gsl::cwzstring<> threadname) noexcept;
#else
   constexpr void LabelThread([[maybe_unused]] gsl::cwzstring<> threadname) noexcept {}
#endif
#else
   constexpr void LabelThread([[maybe_unused]] gsl::cwzstring<> threadname) noexcept {}
#endif
   /*****************************************************************************/
   /**************String Routines************************************************/
   /*****************************************************************************/
   [[nodiscard]] inline juce::String toString(std::string_view in)
   {
      return juce::String(juce::CharPointer_UTF8(in.data()), in.size());
   }
   [[nodiscard]] std::string ReplaceInvisibleChars(std::string_view in);
#ifdef __cpp_lib_starts_ends_with
   [[nodiscard]] constexpr inline bool EndsWith(std::string_view main_str, std::string_view to_match) noexcept
   {
      return main_str.ends_with(to_match);
   }
#else
   [[nodiscard]] bool EndsWith(std::string_view main_str, std::string_view to_match);
#endif
   [[nodiscard]] std::string ToLower(std::string_view in);
   void Trim(std::string_view& value) noexcept;
   void Trim(std::string_view&& value) = delete;
   void TrimL(std::string_view& value) noexcept;
   void TrimL(std::string_view&& value) = delete;
   /*****************************************************************************/
   /**************Error Logging**************************************************/
   /*****************************************************************************/
   /* typical call: rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e); */
   void ExceptionResponse(gsl::czstring<> id, gsl::czstring<> fu, const std::exception& e) noexcept;
   /* char* overloads here are to allow catch clauses to avoid a juce::String conversion at the
    * caller location, thus avoiding a potential exception in the catch clause. string_view
    * overloads not used because those are ambiguous with the String versions. */
#ifdef __cpp_lib_source_location
   void ExceptionResponse(const std::exception& e,
       const std::source_location& location = std::source_location::current()) noexcept;
   void LogAndAlertError(const juce::String& error_text,
       const std::source_location& location = std::source_location::current()) noexcept;
   void LogAndAlertError(const juce::String& alert_text, const juce::String& error_text,
       const std::source_location& location = std::source_location::current()) noexcept;
   void LogAndAlertError(gsl::czstring<> error_text,
       const std::source_location& location = std::source_location::current()) noexcept;
   void Log(const juce::String& info,
       const std::source_location& location = std::source_location::current()) noexcept;
   void Log(gsl::czstring<> info,
       const std::source_location& location = std::source_location::current()) noexcept;
#define MIDI2LR_E_RESPONSE   rsj::ExceptionResponse(e)
#define MIDI2LR_E_RESPONSE_F rsj::ExceptionResponse(e)
#else
   void LogAndAlertError(const juce::String& error_text) noexcept;
   void LogAndAlertError(const juce::String& alert_text, const juce::String& error_text) noexcept;
   void LogAndAlertError(gsl::czstring<> error_text) noexcept;
   void Log(const juce::String& info) noexcept;
   void Log(gsl::czstring<> info) noexcept;
#define MIDI2LR_E_RESPONSE   rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e)
#define MIDI2LR_E_RESPONSE_F rsj::ExceptionResponse(__func__, MIDI2LR_FUNC, e)
#endif
   /*****************************************************************************/
   /*************File Paths******************************************************/
   /*****************************************************************************/
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
#else
   [[nodiscard]] std::string AppDataFilePath(const std::string& file_name);
   [[nodiscard]] std::string AppLogFilePath(const std::string& file_name);
#endif
   /*****************************************************************************/
   /**************Reversed Iterator**********************************************/
   /*****************************************************************************/
   /* Reversed iterable SEE:https://stackoverflow.com/a/42221253/5699329 */
   template<class T> struct ReverseWrapper {
      T o;
      explicit ReverseWrapper(T&& i) noexcept : o(std::forward<T>(i)) {}
   };

   template<class T> auto begin(ReverseWrapper<T>& r) noexcept
   {
      using std::end;
      return std::make_reverse_iterator(end(r.o));
   }

   template<class T> auto end(ReverseWrapper<T>& r) noexcept
   {
      using std::begin;
      return std::make_reverse_iterator(begin(r.o));
   }

   template<class T> auto begin(ReverseWrapper<T> const& r) noexcept
   {
      using std::end;
      return std::make_reverse_iterator(end(r.o));
   }

   template<class T> auto end(ReverseWrapper<T> const& r) noexcept
   {
      using std::begin;
      return std::make_reverse_iterator(begin(r.o));
   }

   template<class T> auto Reverse(T&& ob) noexcept
   {
      return ReverseWrapper<T> {std::forward<T>(ob)};
   }
   /*****************************************************************************/
   /*******************Sleep Timed and Logged************************************/
   /*****************************************************************************/
   template<class Rep, class Period>
   [[nodiscard]] auto SleepTimed(const std::chrono::duration<Rep, Period> sleep_duration) //-V801
   {
      const auto start {std::chrono::steady_clock::now()};
      std::this_thread::sleep_for(sleep_duration);
      const auto end {std::chrono::steady_clock::now()};
      const std::chrono::duration<double, Period> elapsed {end - start};
      return elapsed;
   }

   template<class Rep, class Period>
   void SleepTimedLogged(
       std::string_view msg_prefix, const std::chrono::duration<Rep, Period> sleep_duration) //-V801
   {
      rsj::Log(fmt::format("{} thread slept for {}.", msg_prefix, SleepTimed(sleep_duration)));
   }
   /*****************************************************************************/
   /*******************Fast Floats***********************************************/
   /*****************************************************************************/
   inline int RoundToInt(float source)
   {
      return _mm_cvtss_si32(_mm_set_ss(source));
   }
   inline int RoundToInt(double source)
   {
      return _mm_cvtsd_si32(_mm_set_sd(source));
   }
   int RoundToInt(long double source) = delete;
   inline void IgnoreDenormals()
   { /* speed up floating point ops; we're not worried about precision of very small values */
      _mm_setcsr(_mm_getcsr() | 0x8040);
   }
} // namespace rsj

#endif