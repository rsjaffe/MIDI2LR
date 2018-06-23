// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include <exception>
#include "../JuceLibraryCode/JuceHeader.h"

namespace rsj {
// from http://www.cplusplus.com/forum/beginner/175177 and
// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/cxxabi.h#L156

#ifdef __GNUG__ // gnu C++ compiler
#include <cxxabi.h>
#include <memory>
#include <type_traits>
   template<typename T> T Demangle(const char* mangled_name) noexcept
   {
      static_assert(::std::is_pointer<T>() == false,
          "Result must be copied as __cxa_demagle returns "
          "pointer to temporary. Cannot use pointer type "
          "for this template.");
      ::std::size_t len = 0;
      int status = 0;
      ::std::unique_ptr<char, decltype(&::std::free)> ptr(
          abi::__cxa_demangle(mangled_name, nullptr, &len, &status), &::std::free);
      if (status)
         return mangled_name;
      return ptr.get();
   }
#else  // ndef _GNUG_
   template<typename T> T Demangle(const char* mangled_name) noexcept
   {
      return mangled_name;
   }
#endif // _GNUG_
   void Log(const juce::String& info)
   {
      if (juce::Logger::getCurrentLogger())
         juce::Logger::writeToLog(juce::Time::getCurrentTime().toISO8601(false) + ": " + info);
   }

   void LogAndAlertError(const juce::String& error_text)
   {
      juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, "Error", error_text);
      Log(error_text);
   }
   // use typeid(this).name() for first argument to add class information
   // typical call: rsj::ExceptionResponse(typeid(this).name(), __func__, e);
   void ExceptionResponse(const char* id, const char* fu, const ::std::exception& e) noexcept
   {
      try {
         const juce::String error_text{juce::String("Exception ") + e.what() + ' '
                                       + Demangle<juce::String>(id) + "::" + fu + " Version "
                                       + ProjectInfo::versionString};
         LogAndAlertError(error_text);
      }
      catch (...) {
      }
   }
} // namespace rsj