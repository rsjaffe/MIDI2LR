#ifndef MIDI2LR_NRPNMESSAGE_H_INCLUDED
#define MIDI2LR_NRPNMESSAGE_H_INCLUDED
/*
==============================================================================

NrpnMessage.h

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
#include <array>

#include "Concurrency.h"
#include "MidiUtilities.h"

class NrpnFilter {
   // This  assumes that all NRPN messages have 4 messages, though the NRPN standard allows omission
   // of the 4th message. If the 4th message is dropped, this class silently consumes the message
   // without emitting anything.
 public:
   struct ProcessResult {
      bool is_nrpn{};
      bool is_ready{};
      int control{};
      int value{};
   };
   ProcessResult operator()(const rsj::MidiMessage& message);

 private:
   void Clear(int channel) noexcept
   {
#pragma warning(suppress : 26446 26482) // Channel bounds-checked in calling functions
      intermediate_results_[channel] = {0, 0, 0, 0, 0};
   }
   struct InternalStructure {
      int control_lsb_{0};
      int control_msb_{0};
      int ready_flags_{0};
      int value_lsb_{0};
      int value_msb_{0};
   };
   mutable rsj::SpinLock filter_mutex_;
   static constexpr int kChannels{16};
   std::array<InternalStructure, kChannels> intermediate_results_{};
};

#endif