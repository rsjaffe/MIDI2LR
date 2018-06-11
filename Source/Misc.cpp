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
// from http://www.cplusplus.com/forum/beginner/175177/
// use as rsj::Demangle(typeid(*this).name()) to add class information to exception message
#ifdef __GNUG__ // gnu C++ compiler
#include <cxxabi.h>
#include <memory>
   template<typename T> T Demangle(const char* mangled_name)
   {
      ::std::size_t len = 0;
      int status = 0;
      ::std::unique_ptr<char, decltype(&::std::free)> ptr(
          __cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status), &::std::free);
      return ptr.get();
   }
#else
   template<typename T> T Demangle(const char* mangled_name)
   {
      return mangled_name;
   }
#endif // _GNUG_

   void ExceptionResponse(const char* id, const char* fu, const ::std::exception& e) noexcept
   {
      try {
         juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, "Error",
             juce::String("Exception ") + e.what() + ' ' + Demangle<juce::String>(id) + "::" + fu
                 + " Version " + ProjectInfo::versionString);
      }
      catch (...) {
      }
   }

} // namespace rsj
