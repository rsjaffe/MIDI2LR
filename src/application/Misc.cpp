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
#include "Misc.h"

#include <algorithm>
#include <cctype>

#include <fmt/format.h>
#include <fmt/xchar.h>
#include <ww898/utf_converters.hpp>

#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#ifdef _WIN32
#include <ShlObj.h>
#include <wil/resource.h>
#else
#include "Ocpp.h"
#endif

/* XCode has issues with std:: in this file, using ::std:: to fix when necessary */
/*****************************************************************************/
/**************Thread Labels**************************************************/
/*****************************************************************************/
#ifdef _WIN32
void rsj::LabelThread(gsl::cwzstring<> threadname)
{
   LOG_IF_FAILED(SetThreadDescription(GetCurrentThread(), threadname));
}
#else
#include <pthread.h>
void rsj::LabelThread(gsl::czstring<> threadname)
{
   auto result {pthread_setname_np(threadname)};
   if (result) {
      rsj::Log(
          fmt::format(FMT_STRING("Label thread {} failed with {} error."), threadname, result));
   }
}
#endif
/*****************************************************************************/
/**************String Routines************************************************/
/*****************************************************************************/
namespace {
   constexpr ::std::array ascii_map {"\\x00", "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06",
       "\\a", "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "\\x0E", "\\x0F", "\\x10", "\\x11", "\\x12",
       "\\x13", "\\x14", "\\x15", "\\x16", "\\x17", "\\x18", "\\x19", "\\x1A", "\\x1B", "\\x1C",
       "\\x1D", "\\x1E", "\\x1F", " ", "!", "\\\""};
} // namespace

::std::string rsj::ReplaceInvisibleChars(::std::string_view in)
{
   try {
      ::std::string result {};
      result.reserve(in.size() * 3 / 2); /* midway between max and min final size */
      for (const auto a : in) {
         if (rsj::cmp_less(rsj::CharToInt(a), ascii_map.size())
             && rsj::cmp_greater_equal(rsj::CharToInt(a), 0)) {
#pragma warning(suppress : 26446 26482) /* false alarm, range checked by if statement */
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            result.append(ascii_map[gsl::narrow_cast<size_t>(a)]);
         }
         else if (rsj::cmp_equal(rsj::CharToInt(a), 127)) {
            result.append("\\x7F");
         }
         else if (a == '\\') {
            result.append("\\\\");
         }
         else {
            result.push_back(a);
         }
      }
      return result;
   }
   catch (const ::std::exception& e) {
      MIDI2LR_E_RESPONSE_F;
      throw;
   }
}

/* SEE:http://en.cppreference.com/w/cpp/string/byte/tolower */
::std::string rsj::ToLower(::std::string_view in)
{
   try {
      ::std::string result;
      result.resize(in.size());
      ::std::transform(in.begin(), in.end(), result.begin(), [](const unsigned char c) noexcept {
         return gsl::narrow_cast<unsigned char>(::std::tolower(c));
      });
      return result;
   }
   catch (const ::std::exception& e) {
      MIDI2LR_E_RESPONSE_F;
      throw;
   }
}

void rsj::Trim(::std::string_view& value) noexcept
{
   const auto first_not {value.find_first_not_of(" \t\n")};
   if (first_not != ::std::string_view::npos) { value.remove_prefix(first_not); }
   const auto last_not {value.find_last_not_of(" \t\n")};
   if (last_not != ::std::string_view::npos) { value.remove_suffix(value.size() - last_not - 1); }
}

void rsj::TrimL(::std::string_view& value) noexcept
{
   const auto first_not {value.find_first_not_of(" \t\n")};
   if (first_not != ::std::string_view::npos) { value.remove_prefix(first_not); }
}
/*****************************************************************************/
/**************Error Logging**************************************************/
/*****************************************************************************/
#ifdef __cpp_lib_source_location
void rsj::Log(const juce::String& info, const std::source_location& location) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger()) {
         juce::String localname {location.file_name()};
         localname = localname.substring(localname.lastIndexOfChar('\\') + 1);
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + ' ' + localname
                                  + '(' + juce::String(location.line()) + ") " + info);
      }
   }
   catch (...) { //-V565 //-V5002
   }
}

void rsj::Log(gsl::czstring<> info, const std::source_location& location) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger()) {
         juce::String localname {location.file_name()};
         localname = localname.substring(localname.lastIndexOfChar('\\') + 1);
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + ' ' + localname
                                  + '(' + juce::String(location.line()) + ") "
                                  + juce::String::fromUTF8(info));
      }
   }
   catch (...) { //-V565 //-V5002
   }
}

void rsj::Log(gsl::cwzstring<> info, const std::source_location& location) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger()) {
         juce::String localname {location.file_name()};
         localname = localname.substring(localname.lastIndexOfChar('\\') + 1);
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + ' ' + localname
                                  + '(' + juce::String(location.line()) + ") " + info);
      }
   }
   catch (...) { //-V565 //-V5002
   }
}

void rsj::LogAndAlertError(
    const juce::String& error_text, const std::source_location& location) noexcept
{
   try {
      {
         juce::MessageManager::callAsync([=] {
            juce::NativeMessageBox::showMessageBox(
                juce::AlertWindow::WarningIcon, juce::translate("Error"), error_text);
         });
      }
      rsj::Log(error_text, location);
   }
   catch (...) { //-V565 //-V5002
   }
}

void rsj::LogAndAlertError(const juce::String& alert_text, const juce::String& error_text,
    const std::source_location& location) noexcept
{
   try {
      {
         juce::MessageManager::callAsync([=] {
            juce::NativeMessageBox::showMessageBox(
                juce::AlertWindow::WarningIcon, juce::translate("Error"), alert_text);
         });
      }
      rsj::Log(error_text, location);
   }
   catch (...) { //-V565 //-V5002
   }
}

void rsj::LogAndAlertError(
    gsl::czstring<> error_text, const std::source_location& location) noexcept
{
   try {
      {
         juce::MessageManager::callAsync([=] {
            juce::NativeMessageBox::showMessageBox(
                juce::AlertWindow::WarningIcon, juce::translate("Error"), error_text);
         });
      }
      rsj::Log(error_text, location);
   }
   catch (...) { //-V565 //-V5002
   }
}
void rsj::ExceptionResponse(const std::exception& e, const std::source_location& location) noexcept
{
   try {
      const auto alert_text {juce::translate("Exception ").toStdString() + e.what()};
      const auto error_text {std::string("Exception ") + e.what()};
      rsj::LogAndAlertError(alert_text, error_text, location);
   }
   catch (...) { //-V565 //-V5002
   }
}
#else
void rsj::Log(const juce::String& info) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger()) {
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + " " + info);
      }
   }
   catch (...) { //-V565
   }
}

void rsj::Log(gsl::czstring<> info) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger()) {
         juce::Logger::writeToLog(
             juce::Time::getCurrentTime().toISO8601(true) + " " + juce::String::fromUTF8(info));
      }
   }
   catch (...) { //-V565
   }
}

void rsj::Log(gsl::cwzstring<> info) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger()) {
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + " " + info);
      }
   }
   catch (...) { //-V565
   }
}

void rsj::LogAndAlertError(const juce::String& error_text) noexcept
{
   try {
      {
         juce::MessageManager::callAsync([=] {
            juce::NativeMessageBox::showMessageBox(
                juce::AlertWindow::WarningIcon, juce::translate("Error"), error_text);
         });
      }
      rsj::Log(error_text);
   }
   catch (...) { //-V565
   }
}

void rsj::LogAndAlertError(const juce::String& alert_text, const juce::String& error_text) noexcept
{
   try {
      {
         juce::MessageManager::callAsync([=] {
            juce::NativeMessageBox::showMessageBox(
                juce::AlertWindow::WarningIcon, juce::translate("Error"), alert_text);
         });
      }
      rsj::Log(error_text);
   }
   catch (...) { //-V565
   }
}

void rsj::LogAndAlertError(gsl::czstring<> error_text) noexcept
{
   try {
      {
         juce::MessageManager::callAsync([=] {
            juce::NativeMessageBox::showMessageBox(
                juce::AlertWindow::WarningIcon, juce::translate("Error"), error_text);
         });
      }
      rsj::Log(error_text);
   }
   catch (...) { //-V565
   }
}
#endif
#pragma warning(push)
#pragma warning(disable : 26447)
#if defined(__GNUC__) || defined(__clang__)
void rsj::ExceptionResponse(
    [[maybe_unused]] gsl::czstring<> id, gsl::czstring<> fu, const ::std::exception& e) noexcept
{
   try {
      const auto alert_text {
          fmt::format(juce::translate("Exception ").toStdString() + "{} {}.", e.what(), fu)};
      const auto error_text {fmt::format(FMT_STRING("Exception {} {}."), e.what(), fu)};
      rsj::LogAndAlertError(alert_text, error_text);
   }
   catch (...) { //-V565
   }
}
#else
/* Use typeid(this).name() for first argument to add class information. Typical call:
 * rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e); */
void rsj::ExceptionResponse(
    gsl::czstring<> id, gsl::czstring<> fu, const ::std::exception& e) noexcept
{
   try {
      const auto alert_text {fmt::format(
          juce::translate("Exception ").toStdString() + "{} {}::{}.", e.what(), id, fu)};
      const auto error_text {fmt::format(FMT_STRING("Exception {} {}::{}."), e.what(), id, fu)};
      rsj::LogAndAlertError(alert_text, error_text);
   }
   catch (...) { //-V565 //-V5002
   }
}
#endif
#pragma warning(pop)

/*****************************************************************************/
/*************File Paths******************************************************/
/*****************************************************************************/
#ifdef _WIN32
std::wstring rsj::AppDataFilePath(std::wstring_view file_name)
{
   wil::unique_cotaskmem_string pathptr {nullptr};
   if (SUCCEEDED_LOG(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathptr))) {
      return fmt::format(FMT_STRING(L"{}\\MIDI2LR\\{}"), pathptr.get(), file_name);
   }
   return std::wstring(file_name);
}

std::wstring rsj::AppDataFilePath(std::string_view file_name)
{
   return rsj::AppDataFilePath(ww898::utf::conv<wchar_t>(file_name));
}
#else
::std::string rsj::AppDataFilePath(const ::std::string& file_name)
{
   return rsj::AppDataMac() + "/MIDI2LR/" + file_name;
}
::std::string rsj::AppLogFilePath(const ::std::string& file_name)
{
   return rsj::AppLogMac() + "/MIDI2LR/" + file_name;
}

#endif