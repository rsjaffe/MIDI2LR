/*
  ==============================================================================

    Misc.cpp

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
#include <gsl/gsl>

[[nodiscard]] std::string rsj::ReplaceInvisibleChars(std::string_view input)
{
   try {
      std::array ascii_map{"\\x00", "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06", "\\a",
          "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "\\x0E", "\\x0F", "\\x10", "\\x11", "\\x12",
          "\\x13", "\\x14", "\\x15", "\\x16", "\\x17", "\\x18", "\\x19", "\\x1A", "\\x1B", "\\x1C",
          "\\x1D", "\\x1E", "\\x1F", " ", "!", "\\\""};
      std::string result{};
      result.reserve(input.size());
      for (const auto& a : input) {
         if (static_cast<decltype(ascii_map)::size_type>(a) < ascii_map.size())
#pragma warning(suppress : 26446 26482) // false alarm, range checked by if statement
            result.append(ascii_map[a]);
         else if (a == 127)
            result.append("\\x7F");
         else if (a == '\\')
            result.append("\\\\");
         else
            result.push_back(a);
      }
      return result;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}

// using transform as specified in http://en.cppreference.com/w/cpp/string/byte/tolower
std::string rsj::ToLower(std::string_view in)
{
   try {
      std::string s;
      s.resize(in.size());
      std::transform(in.begin(), in.end(), s.begin(), [
      ](unsigned char c) noexcept { return gsl::narrow_cast<unsigned char>(std::tolower(c)); });
      return s;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse("rsj", __func__, e);
      throw;
   }
}

// note: C++20 will have ends_with
bool rsj::EndsWith(std::string_view main_str, std::string_view to_match)
{
   try {
      return main_str.size() >= to_match.size()
             && main_str.compare(main_str.size() - to_match.size(), to_match.size(), to_match) == 0;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse("rsj", __func__, e);
      throw;
   }
}

// from http://www.cplusplus.com/forum/beginner/175177 and
// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/cxxabi.h#L156

#ifdef __GNUG__ // gnu C++ compiler
#include <cxxabi.h>
#include <memory>
#include <type_traits>
[[nodiscard]] juce::String Demangle(const char* mangled_name)
{
   std::size_t len = 0;
   int status = 0;
   std::unique_ptr<char, decltype(&std::free)> ptr(
       abi::__cxa_demangle(mangled_name, nullptr, &len, &status), &std::free);
   if (status)
      return mangled_name;
   return juce::String(juce::CharPointer_UTF8(ptr.get()));
}
#else  // ndef _GNUG_
[[nodiscard]] juce::String Demangle(_In_z_ const char* mangled_name)
{
   return juce::String(juce::CharPointer_UTF8(mangled_name));
}
#endif // _GNUG_

void rsj::Log(const juce::String& info)
{
   if (juce::Logger::getCurrentLogger())
      juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(true) + ": " + info);
}

void rsj::LogAndAlertError(const juce::String& error_text)
{
   juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, "Error", error_text);
   rsj::Log(error_text);
}

#pragma warning(push)
#pragma warning(disable : 26447)
// use typeid(this).name() for first argument to add class information
// typical call: rsj::ExceptionResponse(typeid(this).name(), __func__, e);
void rsj::ExceptionResponse(
    _In_z_ const char* id, _In_z_ const char* fu, const std::exception& e) noexcept
{
   try {
      const auto error_text{juce::String("Exception ") + e.what() + ' ' + Demangle(id) + "::" + fu
                            + " Version " + ProjectInfo::versionString};
      rsj::LogAndAlertError(error_text);
   }
   catch (...) { //-V565
   }
}
#pragma warning(pop)

#ifdef _WIN32
std::wstring rsj::AppDataFilePath(std::wstring_view file_name)
{
   wchar_t* pathptr{nullptr};
   auto dp = gsl::finally([&pathptr] {
      if (pathptr)
         CoTaskMemFree(pathptr);
   });
   const auto hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathptr);
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
         const auto ret = MultiByteToWideChar(
             CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
         if (!ret) {
            const auto error_msg =
                "MultiByteToWideChar failed with error code: " + rsj::NumToChars(GetLastError());
            throw std::runtime_error(error_msg.c_str());
         }
         return ret;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }

   int WideCharToMultiByteErrorChecked(_In_ UINT CodePage, _In_ DWORD dwFlags,
       _In_NLS_string_(cchWideChar) LPCWCH lpWideCharStr, _In_ int cchWideChar,
       _Out_writes_bytes_to_opt_(cbMultiByte, return ) LPSTR lpMultiByteStr, _In_ int cbMultiByte,
       _In_opt_ LPCCH lpDefaultChar, _Out_opt_ LPBOOL lpUsedDefaultChar)
   {
      try {
         const auto ret = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar,
             lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
         if (!ret) {
            const auto error_msg =
                "WideCharToMultiByte failed with error code: " + rsj::NumToChars(GetLastError());
            throw std::runtime_error(error_msg.c_str());
         }
         return ret;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(__func__, __func__, e);
         throw;
      }
   }
} // namespace

std::wstring rsj::Utf8ToWide(std::string_view input)
{ // add terminating null
   try {
      const auto buffersize = MultiByteToWideCharErrorChecked(CP_UTF8, 0, input.data(),
                                  gsl::narrow_cast<int>(input.size()), nullptr, 0)
                              + 1;
      std::vector<wchar_t> buffer(buffersize, 0); // all zero
      MultiByteToWideCharErrorChecked(CP_UTF8, 0, input.data(), gsl::narrow_cast<int>(input.size()),
          buffer.data(), gsl::narrow_cast<int>(buffer.size()));
      return buffer.data();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse("rsj", __func__, e);
      throw;
   }
}

std::string rsj::WideToUtf8(std::wstring_view wstr)
{ // add terminating null
   try {
      const auto buffersize = WideCharToMultiByteErrorChecked(CP_UTF8, 0, wstr.data(),
                                  gsl::narrow_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr)
                              + 1;
      std::vector<char> buffer(buffersize, 0);
      WideCharToMultiByteErrorChecked(CP_UTF8, 0, wstr.data(), gsl::narrow_cast<int>(wstr.size()),
          buffer.data(), gsl::narrow_cast<int>(buffer.size()), nullptr, nullptr);
      return buffer.data();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse("rsj", __func__, e);
      throw;
   }
}
#else
std::string rsj::AppDataFilePath(const std::string& file_name)
{
   return rsj::AppDataMac() + '/' + file_name;
}
std::string rsj::AppLogFilePath(const std::string& file_name)
{
   return rsj::AppLogMac() + '/' + file_name;
}

#endif