// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

DebugInfo.cpp

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
#include "../JuceLibraryCode/JuceHeader.h"

#ifdef _WIN32
#include <array>
#include "Windows.h"
#include <unicode/unistr.h>

std::string rsj::GetKeyboardLayout()
{
    static_assert(sizeof(WCHAR) == sizeof(UChar),
        "For Unicode handling, assuming windows wide char is same as ICU 16-bit Unicode char.");
    std::array<WCHAR, KL_NAMELENGTH> keyboard_name;
    if (GetKeyboardLayoutNameW(keyboard_name.data())) {
        std::string utf8_rep;
        const icu::UnicodeString utf16_rep{keyboard_name.data()};
        utf16_rep.toUTF8String(utf8_rep);
        return utf8_rep;
    }
    return "unable to get keyboard layout name";
}
#endif


DebugInfo::DebugInfo()
{
    using namespace std::string_literals;
    info.emplace_back("System language "s + juce::SystemStats::getDisplayLanguage().toStdString());
    info.emplace_back("App version "s + ProjectInfo::versionString);
    info.emplace_back("MIDI2LR app path "s +
        juce::File::getSpecialLocation(juce::File::currentApplicationFile).getFullPathName().toStdString());
    info.emplace_back("Keyboard type "s + rsj::GetKeyboardLayout());
}

std::string const * const DebugInfo::GetInfo()
{
    if (iterate >= info.size())
        return nullptr;
    return &info[iterate++];
}
