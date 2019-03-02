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
#include "../JuceLibraryCode/JuceHeader.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define NOATOM -Atom Manager routines
#define NOCLIPBOARD -Clipboard routines
#define NOCOLOR -Screen colors
#define NOCOMM -COMM driver routines
#define NODEFERWINDOWPOS -DeferWindowPos routines
#define NODRAWTEXT -DrawText() and DT_*
#define NOGDI -All GDI defines and routines
#define NOGDICAPMASKS -CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOHELP -Help engine interface.
#define NOICONS -IDI_*
#define NOKANJI -Kanji support stuff.
#define NOKERNEL -All KERNEL defines and routines
#define NOKEYSTATES -MK_*
#define NOMB -MB_*and MessageBox()
#define NOMCX -Modem Configuration Extensions
#define NOMEMMGR -GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMENUS -MF_*
#define NOMETAFILE -typedef METAFILEPICT
#define NOMINMAX -Macros min(a, b) and max(a, b)
#define NONLS -All NLS defines and routines
#define NOOPENFILE -OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOPROFILER -Profiler interface.
#define NORASTEROPS -Binary and Tertiary raster ops
#define NOSCROLL -SB_*and scrolling routines
#define NOSERVICE -All Service Controller routines, SERVICE_ equates, etc.
#define NOSHOWWINDOW -SW_*
#define NOSOUND -Sound driver routines
#define NOSYSCOMMANDS -SC_*
#define NOSYSMETRICS -SM_*
#define NOTEXTMETRIC -typedef TEXTMETRIC and associated routines
#define NOVIRTUALKEYCODES -VK_*
#define NOWH -SetWindowsHook and WH_*
#define NOWINOFFSETS -GWL_*, GCL_*, associated routines
#define NOWINSTYLES -WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define OEMRESOURCE -OEM Resource values
#define WIN32_LEAN_AND_MEAN
//#define NOCTLMGR -Control and Dialog routines
//#define NOMSG -typedef MSG and associated routines
//#define NOUSER            - All USER defines and routines
//#define NOWINMESSAGES     - WM_*, EM_*, LB_*, CB_*
#endif
#undef NOCTLMGR
#undef NOMSG
#undef NOUSER
#undef NOWINMESSAGES
#include <gsl/gsl_util>
#include <ShlObj.h>
#include <Windows.h>
#endif

// from http://www.cplusplus.com/forum/beginner/175177 and
// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/cxxabi.h#L156

#ifdef __GNUG__ // gnu C++ compiler
#include <cxxabi.h>
#include <memory>
#include <type_traits>
template<typename T>
[[nodiscard]] T Demangle(const char* mangled_name) {
   static_assert(std::is_pointer<T>() == false, "Result must be copied as __cxa_demagle returns "
                                                "pointer to temporary. Cannot use pointer type for "
                                                "this template.");
   std::size_t len = 0;
   int status = 0;
   std::unique_ptr<char, decltype(&std::free)> ptr(
       abi::__cxa_demangle(mangled_name, nullptr, &len, &status), &std::free);
   if (status)
      return mangled_name;
   return ptr.get();
}
#else  // ndef _GNUG_
template<typename T>
[[nodiscard]] T Demangle(const char* mangled_name) { return mangled_name; }
#endif // _GNUG_
void rsj::Log(const juce::String& info)
{
   if (juce::Logger::getCurrentLogger())
      juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(false) + ": " + info);
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
void rsj::ExceptionResponse(const char* id, const char* fu, const std::exception& e) noexcept
{
   try {
      const auto error_text{juce::String("Exception ") + e.what() + ' ' + Demangle<juce::String>(id)
                            + "::" + fu + " Version " + ProjectInfo::versionString};
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

std::wstring rsj::Utf8ToWide(std::string_view input)
{
   auto buffersize = MultiByteToWideChar(CP_UTF8, 0, input.data(), input.size(), nullptr, 0)
                     + 1;                      // add terminating null
   std::vector<wchar_t> buffer(buffersize, 0); // all zero
   auto retval =
       MultiByteToWideChar(CP_UTF8, 0, input.data(), input.size(), buffer.data(), buffer.size());
   if (retval == 0) {
      const std::string errorMsg = "UTF8 to UTF16 failed with error code: " + GetLastError();
      throw std::runtime_error(errorMsg.c_str());
   }
   return buffer.data();
}
#endif