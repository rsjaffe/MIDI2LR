/*
==============================================================================

Ocpp.mm

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
#include "Ocpp.h"
#include "DebugInfo.h"         //for GetKeyboardLayout

UniChar rsj::Utf8ToUtf16(const std::string& param)
{
   if (!param.empty()) {
      NSString* result = @(param.c_str());
      UniChar ch = [result characterAtIndex:0];
      return ch;
   }
   return 0;
}

std::string rsj::AppDataMac()
{
   NSString* result = (@"~/Library/Application Support/MIDI2LR").stringByExpandingTildeInPath;
   return std::string(result.UTF8String);
} 

std::string rsj::AppLogMac()
{
   NSString* result = (@"~/Library/Logs/MIDI2LR").stringByExpandingTildeInPath;
   return std::string(result.UTF8String);
}

std::string rsj::GetKeyboardLayout()
{
   // get current keyboard layout by name
   TISInputSourceRef current_source = TISCopyCurrentKeyboardInputSource();
   if (!current_source) // returns null with Japanese keyboard layout
      current_source = TISCopyCurrentKeyboardLayoutInputSource();
   NSString* s =
       (__bridge NSString*)(TISGetInputSourceProperty(current_source, kTISPropertyInputSourceID));
   NSString* t =
       (__bridge NSString*)(TISGetInputSourceProperty(current_source, kTISPropertyLocalizedName));
   if (current_source)
      CFRelease(current_source);
   if (s and t) {
      return std::string(s.UTF8String) + ' ' + std::string(t.UTF8String);
   }
   return std::string("Could not get keyboard input source ID");
}

