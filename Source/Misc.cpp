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

#ifdef _WIN32
#include "WinDef.h"
#undef NOCTLMGR
#undef NOMSG
#undef NONLS
#undef NOUSER
#undef NOWINMESSAGES
#include <ShlObj.h>
#include <Windows.h>
#else
#include "Ocpp.h"
#endif

/* XCode has issues with std:: in this file, using ::std:: to fix when necessary */

/*****************************************************************************/
/**************String Routines************************************************/
namespace {
   ::std::array ascii_map {"\\x00", "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06", "\\a",
       "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "\\x0E", "\\x0F", "\\x10", "\\x11", "\\x12",
       "\\x13", "\\x14", "\\x15", "\\x16", "\\x17", "\\x18", "\\x19", "\\x1A", "\\x1B", "\\x1C",
       "\\x1D", "\\x1E", "\\x1F", " ", "!", "\\\""};
} // namespace

::std::string rsj::ReplaceInvisibleChars(::std::string_view in)
{
   try {
      ::std::string result {};
      result.reserve(in.size()); /* minimum final size */
      for (const auto& a : in) {
         if (gsl::narrow_cast<size_t>(a) < ascii_map.size())
#pragma warning(suppress : 26446 26482) /* false alarm, range checked by if statement */
            result.append(ascii_map[gsl::narrow_cast<size_t>(a)]);
         else if (a == 127)
            result.append("\\x7F");
         else if (a == '\\')
            result.append("\\\\");
         else
            result.push_back(a);
      }
      return result;
   }
   catch (const ::std::exception& e) {
      rsj::ExceptionResponse(__func__, MIDI2LR_FUNC, e);
      throw;
   }
}

/* SEE:http://en.cppreference.com/w/cpp/string/byte/tolower */
::std::string rsj::ToLower(::std::string_view in)
{
   try {
      ::std::string result;
      result.resize(in.size());
      ::std::transform(in.begin(), in.end(), result.begin(), [](unsigned char c) noexcept {
         return gsl::narrow_cast<unsigned char>(::std::tolower(c));
      });
      return result;
   }
   catch (const ::std::exception& e) {
      rsj::ExceptionResponse("rsj", MIDI2LR_FUNC, e);
      throw;
   }
}

void rsj::Trim(::std::string_view& value) noexcept
{
   const auto first_not {value.find_first_not_of(" \t\n")};
   if (first_not != ::std::string_view::npos)
      value.remove_prefix(first_not);
   const auto last_not {value.find_last_not_of(" \t\n")};
   if (last_not != ::std::string_view::npos)
      value.remove_suffix(value.size() - last_not - 1);
}

void rsj::TrimL(::std::string_view& value) noexcept
{
   const auto first_not {value.find_first_not_of(" \t\n")};
   if (first_not != ::std::string_view::npos)
      value.remove_prefix(first_not);
}

/* note: C++20 will have ends_with */
bool rsj::EndsWith(::std::string_view main_str, ::std::string_view to_match)
{
   try {
      return main_str.size() >= to_match.size()
             && main_str.compare(main_str.size() - to_match.size(), to_match.size(), to_match) == 0;
   }
   catch (const ::std::exception& e) {
      rsj::ExceptionResponse("rsj", MIDI2LR_FUNC, e);
      throw;
   }
}

/*****************************************************************************/
/**************Error Logging**************************************************/
/* from http://www.cplusplus.com/forum/beginner/175177 and
 * https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/cxxabi.h#L156 */

void rsj::Log(const juce::String& info) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger())
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + ": " + info);
   }
   catch (...) { //-V565
   }
}

void rsj::Log(gsl::czstring<> info) noexcept
{
   try {
      if (juce::Logger::getCurrentLogger())
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + ": " + info);
   }
   catch (...) { //-V565
   }
}

void rsj::LogAndAlertError(const juce::String& error_text) noexcept
{
   try {
      {
         const juce::MessageManagerLock mmLock; /* this may be unnecessary */
         juce::NativeMessageBox::showMessageBox(
             juce::AlertWindow::WarningIcon, juce::translate("Error"), error_text);
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
         const juce::MessageManagerLock mmLock; /* this may be unnecessary */
         juce::NativeMessageBox::showMessageBox(
             juce::AlertWindow::WarningIcon, juce::translate("Error"), alert_text);
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
         const juce::MessageManagerLock mmLock; /* this may be unnecessary */
         juce::NativeMessageBox::showMessageBox(
             juce::AlertWindow::WarningIcon, juce::translate("Error"), error_text);
      }
      rsj::Log(error_text);
   }
   catch (...) { //-V565
   }
}

#pragma warning(push)
#pragma warning(disable : 26447)
#if defined(__GNUC__) || defined(__clang__)
void rsj::ExceptionResponse(
    [[maybe_unused]] gsl::czstring<> id, gsl::czstring<> fu, const ::std::exception& e) noexcept
{
   try {
      const auto alert_text {
          fmt::format(juce::translate("Exception ").toStdString() + "{} {}.", e.what(), fu)};
      const auto error_text {fmt::format("Exception {} {}.", e.what(), fu)};
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
      const auto error_text {fmt::format("Exception {} {}::{}.", e.what(), id, fu)};
      rsj::LogAndAlertError(alert_text, error_text);
   }
   catch (...) { //-V565
   }
}
#endif
#pragma warning(pop)

/*****************************************************************************/
/*************File Paths******************************************************/
#ifdef _WIN32
std::wstring rsj::AppDataFilePath(std::wstring_view file_name)
{
   wchar_t* pathptr {nullptr};
   auto dp {gsl::finally([&pathptr] {
      if (pathptr)
         CoTaskMemFree(pathptr);
   })};
   const auto hr {SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathptr)};
   if (SUCCEEDED(hr))
      return std::wstring(pathptr) + L"\\MIDI2LR\\" + std::wstring(file_name);
   return std::wstring(file_name);
}

std::wstring rsj::AppDataFilePath(std::string_view file_name)
{
   return rsj::AppDataFilePath(rsj::Utf8ToWide(file_name));
}

namespace {
   int MultiByteToWideCharErrorChecked(_In_ UINT CodePage, _In_ DWORD dwFlags,
       _In_NLS_string_(cbMultiByte) LPCCH lpMultiByteStr, _In_ int cbMultiByte,
       _Out_writes_to_opt_(cchWideChar, return ) LPWSTR lpWideCharStr, _In_ int cchWideChar)
   {
      try {
         const auto ret {MultiByteToWideChar(
             CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar)};
         if (!ret)
            throw std::runtime_error(
                fmt::format("MultiByteToWideChar failed with error code: {}.", GetLastError()));
         return ret;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, MIDI2LR_FUNC, e);
         throw;
      }
   }

   int WideCharToMultiByteErrorChecked(_In_ UINT CodePage, _In_ DWORD dwFlags,
       _In_NLS_string_(cchWideChar) LPCWCH lpWideCharStr, _In_ int cchWideChar,
       _Out_writes_bytes_to_opt_(cbMultiByte, return ) LPSTR lpMultiByteStr, _In_ int cbMultiByte,
       _In_opt_ LPCCH lpDefaultChar, _Out_opt_ LPBOOL lpUsedDefaultChar)
   {
      try {
         const auto ret {WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar,
             lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar)};
         if (!ret)
            throw std::runtime_error(
                fmt::format("WideCharToMultiByte failed with error code: {}.", GetLastError()));
         return ret;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, MIDI2LR_FUNC, e);
         throw;
      }
   }
} // namespace

std::wstring rsj::Utf8ToWide(std::string_view in)
{
   /* add terminating null */
   try {
      const auto buffer_size {MultiByteToWideCharErrorChecked(CP_UTF8, 0, in.data(),
                                  gsl::narrow_cast<int>(in.size()), nullptr, 0)
                              + 1};
      std::vector<wchar_t> buffer(buffer_size, 0); /* all zero */
      MultiByteToWideCharErrorChecked(CP_UTF8, 0, in.data(), gsl::narrow_cast<int>(in.size()),
          buffer.data(), gsl::narrow_cast<int>(buffer.size()));
      return buffer.data();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse("rsj", MIDI2LR_FUNC, e);
      throw;
   }
}

std::string rsj::WideToUtf8(std::wstring_view in)
{
   /* add terminating null */
   try {
      const auto buffer_size {WideCharToMultiByteErrorChecked(CP_UTF8, 0, in.data(),
                                  gsl::narrow_cast<int>(in.size()), nullptr, 0, nullptr, nullptr)
                              + 1};
      std::vector<char> buffer(buffer_size, 0);
      WideCharToMultiByteErrorChecked(CP_UTF8, 0, in.data(), gsl::narrow_cast<int>(in.size()),
          buffer.data(), gsl::narrow_cast<int>(buffer.size()), nullptr, nullptr);
      return buffer.data();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse("rsj", MIDI2LR_FUNC, e);
      throw;
   }
}
#else
::std::string rsj::AppDataFilePath(const ::std::string& file_name)
{
   return rsj::AppDataMac() + '/' + file_name;
}
::std::string rsj::AppLogFilePath(const ::std::string& file_name)
{
   return rsj::AppLogMac() + '/' + file_name;
}

#endif