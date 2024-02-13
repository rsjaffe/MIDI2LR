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
#include <source_location>
#include <string>
#include <string_view>

#include <gsl/gsl>

#include <juce_core/juce_core.h>

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
      uint64_t eE2Hsb4v {}; /* random name to avoid shadowing warnings */                          \
      MIDI2LR_FPU_GETCW(eE2Hsb4v);                                                                 \
      eE2Hsb4v |= (1 << 24) | (1 << 19); /* FZ flag, FZ16 flag; flush denormals to zero  */        \
      MIDI2LR_FPU_SETCW(eE2Hsb4v);                                                                 \
   }                                                                                               \
   static_assert(true, "require semi-colon after macro with this assert")
#endif

namespace rsj {
   /*****************************************************************************/
   /**************Thread Labels**************************************************/
   /*****************************************************************************/
#ifdef _WIN32
   void LabelThread(gsl::cwzstring threadname);
#else
   void LabelThread(gsl::czstring threadname);
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
   void ExceptionResponse(gsl::czstring id, gsl::czstring fu, const std::exception& e) noexcept;
   /* char* overloads here are to allow catch clauses to avoid a juce::String conversion at the
    * caller location, thus avoiding a potential exception in the catch clause. string_view
    * overloads not used because those are ambiguous with the String versions. */
   void ExceptionResponse(const std::exception& e,
       const std::source_location& location = std::source_location::current()) noexcept;
   void LogAndAlertError(const juce::String& error_text,
       const std::source_location& location = std::source_location::current()) noexcept;
   void LogAndAlertError(const juce::String& alert_text, const juce::String& error_text,
       const std::source_location& location = std::source_location::current()) noexcept;
   void LogAndAlertError(gsl::czstring error_text,
       const std::source_location& location = std::source_location::current()) noexcept;
   void Log(const juce::String& info,
       const std::source_location& location = std::source_location::current()) noexcept;
   void Log(gsl::czstring info,
       const std::source_location& location = std::source_location::current()) noexcept;
   void Log(gsl::cwzstring info,
       const std::source_location& location = std::source_location::current()) noexcept;

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
