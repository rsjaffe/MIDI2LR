// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

Translate.cpp

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

#include "Translate.h"
#include "Translate.txt"
#include <JuceLibraryCode/JuceHeader.h>
#include <map>

void rsj::SetLanguage(const std::string& lg)
{
#pragma warning(suppress : 26426)
   static const std::map<std::string, const char*> translation_table{{"de", de}, {"es", es},
       {"fr", fr}, {"it", it}, {"ja", ja}, {"ko", ko}, {"nl", nl}, {"pt", pt}, {"sv", sv},
       {"zn_cn", zn_cn}, {"zn_tw", zn_tw}};
   if (const auto found = translation_table.find(lg); found != translation_table.end()) {
      const juce::String str(juce::CharPointer_UTF8(found->second));
      auto ls = std::make_unique<juce::LocalisedStrings>(str, false);
      juce::LocalisedStrings::setCurrentMappings(ls.release()); // takes ownership of ls
   }
   else
      juce::LocalisedStrings::setCurrentMappings(nullptr);
}