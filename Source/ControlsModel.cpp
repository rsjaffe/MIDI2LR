// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

ControlsModel.cpp

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#include "ControlsModel.h"

ControlsModel::ControlsModel() {}
ControlsModel::~ControlsModel() {}
ChannelModel::ChannelModel() {
  //program defaults
  ccLow_.fill(0);
  ccHigh_.fill(kMaxNRPN);
  CCmethod_.fill(RSJ::CCmethod::absolute);
  currentV_.fill(kMaxNRPNHalf);
  for (size_t a = 0; a <= kMaxMIDI; ++a) {
    ccHigh_[a] = kMaxMIDI;
    currentV_[a] = kMaxMIDIHalf;
  }
  //load settings
}
ChannelModel::~ChannelModel() {}

double ChannelModel::controllerToPlugin(short controlT, short controlN, short controlV) noexcept(ndebug) {
  assert((controlT == RSJ::CCflag && CCmethod_[controlN] == RSJ::CCmethod::absolute) ? (ccLow_[controlN] < ccHigh_[controlN]) : 1);
  assert((controlT == RSJ::PWflag) ? (PitchWheelMax > PitchWheelMin) : 1);
  assert((controlT == RSJ::PWflag) ? controlV >= PitchWheelMin && controlV <= PitchWheelMax : 1);
  //note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7 bits, high bits are shifted one right when placed into short.
  switch (controlT) {
    case RSJ::PWflag:
      return static_cast<double>(controlV - PitchWheelMin) / static_cast<double>(PitchWheelMax - PitchWheelMin);
    case RSJ::CCflag:
      switch (CCmethod_[controlN]) {
        case RSJ::CCmethod::absolute:
          return static_cast<double>(controlV - ccLow_[controlN]) / static_cast<double>(ccHigh_[controlN] - ccLow_[controlN]);
        case RSJ::CCmethod::binaryoffset:
          if (isNRPN(controlN))
            return offsetresult(controlV - kBit14, controlN);
          else
            return offsetresult(controlV - kBit7, controlN);
        case RSJ::CCmethod::signmagnitude:
          if (isNRPN(controlN))
            return offsetresult((controlV & kBit14) ? -(controlV & kLow13Bits) : controlV, controlN);
          else
            return offsetresult((controlV & kBit7) ? -(controlV & kLow6Bits) : controlV, controlN);
        case RSJ::CCmethod::twoscomplement: //see https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
          if (isNRPN(controlN)) //flip twos comp and subtract--independent of processor architecture
            return offsetresult((controlV & kBit14) ? -((controlV ^ kMaxNRPN) + 1) : controlV, controlN);
          else
            return offsetresult((controlV & kBit7) ? -((controlV ^ kMaxMIDI) + 1) : controlV, controlN);
        default:
          assert(!"Should be unreachable code in controllerToPlugin--unknown CCmethod");
          return 0.0;
      }
    case RSJ::NoteOnFlag:
      return 1.0;
    case RSJ::NoteOffFlag:
      return 0.0;
    default:
      assert(!"Should be unreachable code in controllerToPlugin--unknown control type");
      return 0.0;
  }
}