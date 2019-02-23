#pragma once
/*
==============================================================================

DebugInfo.h

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
#include <string>
#include <vector>

namespace juce {
   class String;
}

class DebugInfo {
 public:
   explicit DebugInfo(const juce::String& profile_directory) noexcept;
   ~DebugInfo() = default;
   DebugInfo(const DebugInfo& other) = default;
   DebugInfo(DebugInfo&& other) = default;
   DebugInfo& operator=(const DebugInfo& other) = default;
   DebugInfo& operator=(DebugInfo&& other) = default;
   void ResetOutput() noexcept
   {
      iterate_ = 0;
   }
   const std::string* GetInfo() noexcept;

 private:
   void Send(std::string&& msg);
   std::vector<std::string> info_;
   size_t iterate_{0};
};

namespace rsj {
   std::string GetKeyboardLayout();
}