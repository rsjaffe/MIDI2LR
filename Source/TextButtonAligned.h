#ifndef MIDI2LR_TEXTBUTTONALIGNED_H_INCLUDED
#define MIDI2LR_TEXTBUTTONALIGNED_H_INCLUDED
/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include <JuceLibraryCode/JuceHeader.h>

class TextButtonAligned : public juce::TextButton {
 public:
   using TextButton::TextButton;
   explicit TextButtonAligned(juce::Justification::Flags alignment)
       : juce::TextButton(), alignment_{alignment} {};
   TextButtonAligned(const juce::String& buttonName, juce::Justification::Flags alignment)
       : juce::TextButton(buttonName), alignment_{alignment} {};
   TextButtonAligned(const juce::String& buttonName, const juce::String& toolTip,
       juce::Justification::Flags alignment)
       : juce::TextButton(buttonName, toolTip), alignment_{alignment} {};
   void paintButton(juce::Graphics&, bool, bool) override;

 private:
   void drawButtonText(juce::Graphics& g, TextButton& button,
       [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
       [[maybe_unused]] bool shouldDrawButtonAsDown);

   juce::Justification::Flags alignment_{juce::Justification::centredLeft};
};
#endif