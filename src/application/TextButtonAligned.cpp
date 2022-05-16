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

#include <algorithm>

#include <gsl/gsl>

void TextButtonAligned::paintButton(juce::Graphics& g, const bool should_draw_button_as_highlighted,
    const bool should_draw_button_as_down)
{
   /* from TextButton.cpp: only change is second call from lf:: to TextButtonAligned:: */
#pragma warning(suppress : 26812)
   juce::Component::getLookAndFeel().drawButtonBackground(g, *this,
       juce::Component::findColour(
           juce::Button::getToggleState() ? juce::TextButton::ColourIds::buttonOnColourId
                                          : juce::TextButton::ColourIds::buttonColourId),
       should_draw_button_as_highlighted, should_draw_button_as_down);
   /* the following normally calls lf.drawButtonText, hijacking the call for specific formatting */
   DrawButtonText(g, *this, should_draw_button_as_highlighted, should_draw_button_as_down);
}

void TextButtonAligned::DrawButtonText(juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool should_draw_button_as_highlighted,
    [[maybe_unused]] bool should_draw_button_as_down) const
{
   /* Based on drawButtonText in LookAndFeel_V2 (V3 doesn't override V2 for this call). Only change
    * in my version is alignment on last line. */
   const auto font {
       juce::Component::getLookAndFeel().getTextButtonFont(button, button.getHeight())};
   g.setFont(font);
   g.setColour(button
                   .findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                       : juce::TextButton::textColourOffId)
                   .withMultipliedAlpha(button.isEnabled() ? 1.0F : 0.5F));

   const auto y_indent {std::min(4, button.proportionOfHeight(0.3F))}; //-V112
   const auto corner_size {std::min(button.getHeight(), button.getWidth()) / 2};
#ifdef _WIN32
   const auto font_height {_cvt_ftoi_fast(font.getHeight() * 0.6F + 0.5F)};
#else
   const auto font_height {gsl::narrow_cast<int>(font.getHeight() * 0.6F + 0.5F)};
#endif
   const auto left_indent {
       std::min(font_height, 2 + corner_size / (button.isConnectedOnLeft() ? 4 : 2))}; //-V112
   const auto right_indent {
       std::min(font_height, 2 + corner_size / (button.isConnectedOnRight() ? 4 : 2))}; //-V112

   if (const auto text_width {button.getWidth() - left_indent - right_indent}; text_width > 0) {
      g.drawFittedText(button.getButtonText(), left_indent, y_indent, text_width,
          button.getHeight() - y_indent * 2, alignment_, 2);
   }
}