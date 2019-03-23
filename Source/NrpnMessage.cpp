/*
==============================================================================

NrpnMessage.cpp

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
#include "NrpnMessage.h"

bool NrpnMessage::ProcessMidi(short control, short value)
{
   try {
      Expects(value <= 0x7F);
      Expects(control <= 0x7F);
      static const thread_local moodycamel::ProducerToken ptok(nrpn_queued_);
      auto ret_val = true;
      switch (control) {
      case 6: {
         auto dlock = std::lock_guard(data_guard_);
         if (ready_ >= 0b11) {
            SetValueMsb(value);
            if (IsReady()) {
               nrpn_queued_.enqueue(ptok, {true, GetControl(), GetValue()});
               Clear();
            }
         }
         else
            ret_val = false;
         break;
      }
      case 38u: {
         auto dlock = std::lock_guard(data_guard_);
         if (ready_ >= 0b11) {
            SetValueLsb(value);
            if (IsReady()) {
               nrpn_queued_.enqueue(ptok, {true, GetControl(), GetValue()});
               Clear();
            }
         }
         else
            ret_val = false;
         break;
      }
      case 98u: {
         auto dlock = std::lock_guard(data_guard_);
         SetControlLsb(value);
      } break;
      case 99u: {
         auto dlock = std::lock_guard(data_guard_);
         SetControlMsb(value);
      } break;
      default: // not an expected nrpn control #, handle as typical midi message
         ret_val = false;
      }
      return ret_val;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

rsj::Nrpn NrpnMessage::GetNrpnIfReady()
{
   try {
      static thread_local moodycamel::ConsumerToken ctok(nrpn_queued_);
      rsj::Nrpn retval;
      if (nrpn_queued_.try_dequeue(ctok, retval)) { return retval; }
      return rsj::kInvalidNrpn;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void NrpnMessage::Clear() noexcept
{
   ready_ = 0;
   control_msb_ = 0;
   control_lsb_ = 0;
   value_msb_ = 0;
   value_lsb_ = 0;
}