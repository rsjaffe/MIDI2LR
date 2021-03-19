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

#include <exception>
#include <limits>
#include <string>
#include <string_view>
#include <version>

#include <gsl/gsl>

#include <juce_core/juce_core.h>

#ifdef __cpp_lib_source_location
#include <source_location>
#endif
#ifdef __cpp_lib_integer_comparison_functions
#include <utility>
#else
#include <type_traits>
#endif

#ifdef NDEBUG /* asserts disabled */
constexpr bool kNdebug {true};
#else
constexpr bool kNdebug {false};
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MIDI2LR_FUNC __PRETTY_FUNCTION__
#else
#define MIDI2LR_FUNC __func__
#endif

#ifdef _WIN32
constexpr auto MSWindows {true};
constexpr auto MacOS {false};
#define MIDI2LR_UC_LITERAL(text) L##text
#else
constexpr auto MSWindows {false};
constexpr auto MacOS {true};
#define MIDI2LR_UC_LITERAL(text) text
#endif

#ifndef __ARM_ARCH
extern "C" {
   extern unsigned int _mm_getcsr();
   extern void _mm_setcsr(unsigned int);
}
#define MIDI2LR_FAST_FLOATS _mm_setcsr(_mm_getcsr() | 0x8040U)
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
#ifdef _WIN32
   void LabelThread(gsl::cwzstring<> threadname);
#else
   void LabelThread(gsl::czstring<> threadname);
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
   void Log(gsl::cwzstring<> info,
       const std::source_location& location = std::source_location::current()) noexcept;
#define MIDI2LR_E_RESPONSE   rsj::ExceptionResponse(e)
#define MIDI2LR_E_RESPONSE_F rsj::ExceptionResponse(e)
#else
   void LogAndAlertError(const juce::String& error_text) noexcept;
   void LogAndAlertError(const juce::String& alert_text, const juce::String& error_text) noexcept;
   void LogAndAlertError(gsl::czstring<> error_text) noexcept;
   void Log(const juce::String& info) noexcept;
   void Log(gsl::czstring<> info) noexcept;
   void Log(gsl::cwzstring<> info) noexcept;
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
   [[nodiscard]] constexpr bool IsAnyOfV = std::disjunction_v<std::is_same<T, Ts>...>;

   template <class T>
   [[nodiscard]] constexpr bool IsStandardInteger = std::is_integral_v<T>
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
      if constexpr (std::is_signed_v<S> == std::is_signed_v<T>) { return left == right; }
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
      if constexpr (std::is_signed_v<S> == std::is_signed_v<T>) { return left < right; }
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
   template<class T> auto CharToInt(T t) = delete;
   [[nodiscard]] constexpr auto CharToInt(const char in) noexcept
   {
      if constexpr (std::numeric_limits<char>::is_signed) { return static_cast<int>(in); }
      else {
         return static_cast<unsigned int>(in);
      }
   }
} // namespace rsj

#endif