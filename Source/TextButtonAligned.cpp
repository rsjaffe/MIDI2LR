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

#include "TextButtonAligned.h"

void TextButtonAligned::paintButton(
    juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
   /* from TextButton.cpp: only change is second call from lf:: to TextButtonAligned:: */
   juce::Component::getLookAndFeel().drawButtonBackground(g, *this,
       juce::Component::findColour(juce::Button::getToggleState()
                                       ? juce::TextButton::ColourIds::buttonOnColourId
                                       : juce::TextButton::ColourIds::buttonColourId),
       shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
   /* the following normally calls lf.drawButtonText, hijacking the call for specific formatting */
   drawButtonText(g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void TextButtonAligned::drawButtonText(juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
    [[maybe_unused]] bool shouldDrawButtonAsDown)
{
   /* Based on drawButtonText in LookAndFeel_V2 (V3 doesn't override V2 for this call). Only change
    * is alignment on last line. */
   juce::Font font(juce::Component::getLookAndFeel().getTextButtonFont(button, button.getHeight()));
   g.setFont(font);
   g.setColour(button
                   .findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                       : juce::TextButton::textColourOffId)
                   .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

   const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
   const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

   const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
   const int leftIndent =
       juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
   const int rightIndent =
       juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
   const int textWidth = button.getWidth() - leftIndent - rightIndent;

   if (textWidth > 0)
      g.drawFittedText(button.getButtonText(), leftIndent, yIndent, textWidth,
          button.getHeight() - yIndent * 2, alignment_, 2);
}
