/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
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
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class ControlsModel;
#ifndef _MSC_VER
#define _In_ //-V3547
#endif
// NOTE: must add juce:: qualifiers in header if regenerate file
//[/Headers]

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CCoptions :
    public juce::Component,
    private juce::TextEditor::Listener,
    public juce::Button::Listener {
 public:
   //==============================================================================
   CCoptions();
   ~CCoptions();

   //==============================================================================
   //[UserMethods]     -- You can add your own custom methods in this section.
   static void LinkToControlsModel(_In_ ControlsModel* model) noexcept { controls_model_ = model; }

   void BindToControl(int channel, int control_number);
   CCoptions(CCoptions&& other) noexcept = delete;
   CCoptions& operator=(CCoptions&& other) noexcept = delete;
   //[/UserMethods]

   void paint(juce::Graphics& g) override;
   void resized() override;
   void buttonClicked(juce::Button* buttonThatWasClicked) override;

 private:
   //[UserVariables]   -- You can add your own custom variables in this section.
   juce::TextEditor::LengthAndCharacterRestriction numrestrict_ {5, "0123456789"};
   void textEditorFocusLost(juce::TextEditor& t) override;
   inline static ControlsModel* controls_model_ {nullptr};
   int bound_channel_ {0}; // note: 0-based in program, add one to compensate for display
   int bound_number_ {0};
   //[/UserVariables]

   //==============================================================================
   std::unique_ptr<juce::GroupComponent> groupComponent;
   std::unique_ptr<juce::ToggleButton> twosbutton;
   std::unique_ptr<juce::ToggleButton> absbutton;
   std::unique_ptr<juce::ToggleButton> binbutton;
   std::unique_ptr<juce::ToggleButton> signbutton;
   std::unique_ptr<juce::TextEditor> maxvaltext;
   std::unique_ptr<juce::TextEditor> minvaltext;
   std::unique_ptr<juce::Label> minvallabel;
   std::unique_ptr<juce::Label> maxvallabel;
   std::unique_ptr<juce::TextButton> applyAll;
   std::unique_ptr<juce::Label> controlID;

   //==============================================================================
   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CCoptions)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
