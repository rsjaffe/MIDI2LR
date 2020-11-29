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
#ifdef __cpp_lib_source_location
#include <source_location>
#endif
#include <string>
#include <string_view>
#include <thread> /* sleep_for */
#ifdef __cpp_lib_integer_comparison_functions
#include <utility>
#endif

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <gsl/gsl>
#ifndef __ARM_ARCH
#include <xmmintrin.h> /* for rounding intrinsics */
#else
#include <cmath>
#endif

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

#ifndef __ARM_ARCH
#define MIDI2LR_FAST_FLOATS _mm_setcsr(_mm_getcsr() | 0x8040)
#else
#define MIDI2LR_FPU_GETCW(fpcr) __asm__ __volatile__("mrs %0, fpcr" : "=r"(fpcr))
#define MIDI2LR_FPU_SETCW(fpcr) __asm__ __volatile__("msr fpcr, %0" : : "r"(fpcr))
#define MIDI2LR_FAST_FLOATS                                                                        \
   {                                                                                               \
      uint64_t status {};                                                                          \
      MIDI2LR_FPU_GETCW(status);                                                                   \
      status |= (1 << 24) | (1 << 19); /* FZ flag, FZ16 flag; flush denormals to zero  */          \
      MIDI2LR_FPU_SETCW(status);                                                                   \
   }                                                                                               \
   static_assert(true, "require semi-colon after macro with this assert")
#endif

namespace rsj {
   /*****************************************************************************/
   /**************Thread Labels**************************************************/
   /*****************************************************************************/
#if !defined(NDEBUG) && defined(_WIN32)
   void LabelThread(gsl::cwzstring<> threadname);
#else
   constexpr void LabelThread([[maybe_unused]] gsl::cwzstring<> threadname) noexcept {}
#endif
   /*****************************************************************************/
   /**************String Routines************************************************/
   /*****************************************************************************/
   [[nodiscard]] std::string ReplaceInvisibleChars(std::string_view in);
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
      rsj::Log(fmt::format(
          FMT_STRING("{} thread slept for {}."), msg_prefix, SleepTimed(sleep_duration)));
   }
   /*****************************************************************************/
   /*******************Fast Floats***********************************************/
   /*****************************************************************************/
#ifndef __ARM_ARCH
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
#else
   inline int RoundToInt(float source)
   {
      return gsl::narrow<int>(std::lround(source));
   }
   inline int RoundToInt(double source)
   {
      return gsl::narrow<int>(std::lround(source));
   }
#endif
/*****************************************************************************/
/**************Safe Integer Comparisons***************************************/
/*****************************************************************************/
#ifdef __cpp_lib_integer_comparison_functions
   using std::cmp_equal;
   using std::cmp_greater;
   using std::cmp_greater_equal;
   using std::cmp_less;
   using std::cmp_less_equal;
   using std::cmp_not_equal;
#else
   /* adapted from Microsoft <utility> Apache 2 license */
   template<class T, class... Ts>
   inline constexpr bool IsAnyOfV = std::disjunction_v<std::is_same<T, Ts>...>;

   template <class T>
   inline constexpr bool IsStandardInteger = std::is_integral_v<T> 
       && !IsAnyOfV<std::remove_cv_t<T>, bool, char, wchar_t,
#ifdef __cpp_char8_t
        char8_t,
#endif
        char16_t, char32_t>;

   template<class S, class T>
   [[nodiscard]] constexpr bool cmp_equal(const S left, const T right) noexcept
   {
      static_assert(IsStandardInteger<S> && IsStandardInteger<T>,
          "The integer comparison functions only accept standard and extended integer types.");
      if constexpr (std::is_signed_v<S> == std::is_signed_v<T>) {
         return left == right;
      }
      else if constexpr (std::is_signed_v<T>) {
         return left == static_cast<std::make_unsigned_t<T>>(right) && right >= 0;
      }
      else {
         return static_cast<std::make_unsigned_t<S>>(left) == right && left >= 0;
      }
   }

   template<class S, class T>
   [[nodiscard]] constexpr bool cmp_not_equal(const S left, const T right) noexcept
   {
      return !cmp_equal(left, right);
   }

   template<class S, class T>
   [[nodiscard]] constexpr bool cmp_less(const S left, const T right) noexcept
   {
      static_assert(IsStandardInteger<S> && IsStandardInteger<T>,
          "The integer comparison functions only accept standard and extended integer types.");
      if constexpr (std::is_signed_v<S> == std::is_signed_v<T>) {
         return left < right;
      }
      else if constexpr (std::is_signed_v<T>) {
         return right > 0 && left < static_cast<std::make_unsigned_t<T>>(right);
      }
      else {
         return left < 0 || static_cast<std::make_unsigned_t<S>>(left) < right;
      }
   }

   template<class S, class T>
   [[nodiscard]] constexpr bool cmp_greater(const S left, const T right) noexcept
   {
      return cmp_less(right, left);
   }

   template<class S, class T>
   [[nodiscard]] constexpr bool cmp_less_equal(const S left, const T right) noexcept
   {
      return !cmp_less(right, left);
   }

   template<class S, class T>
   [[nodiscard]] constexpr bool cmp_greater_equal(const S left, const T right) noexcept
   {
      return !cmp_less(left, right);
   }
#endif
   constexpr auto CharToInt(char in)
   {
      if constexpr (std::numeric_limits<char>::is_signed)
         return static_cast<int>(in);
      else
         return static_cast<unsigned int>(in);
   }
} // namespace rsj

#endif