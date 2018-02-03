// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

DebugInfo.mm

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
#include "DebugInfo.h"
#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>

std::string rsj::GetKeyboardLayout()
{
    // get current keyboard layout by name
    TISInputSourceRef current_source = TISCopyCurrentKeyboardInputSource();
    NSString *s = (__bridge NSString *)(TISGetInputSourceProperty(current_source, kTISPropertyInputSourceID));
    if (s) {
        // get last part of string (without com.apple.keylayout.)
        NSUInteger last_dot_num = [s rangeOfString:@"." options:NSBackwardsSearch].location;
        NSString *substring = [s substringFromIndex:last_dot_num + 1];
        return std::string([substring UTF8String]);
    }
    return std::string("could not get input source ID");
}