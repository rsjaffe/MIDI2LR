// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.3.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
/*
==============================================================================

PWoptions.cpp

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

//[/Headers]

#include "PWoptions.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
ControlsModel* PWoptions::controls_model_{nullptr};
//[/MiscUserDefs]

//==============================================================================
PWoptions::PWoptions() {
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

  addAndMakeVisible(label = new Label("new label",
    TRANS("Minimum value")));
  label->setFont(Font(15.00f, Font::plain));
  label->setJustificationType(Justification::centredLeft);
  label->setEditable(false, false, false);
  label->setColour(TextEditor::textColourId, Colours::black);
  label->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

  addAndMakeVisible(minval = new TextEditor("minval"));
  minval->setExplicitFocusOrder(1);
  minval->setMultiLine(false);
  minval->setReturnKeyStartsNewLine(false);
  minval->setReadOnly(false);
  minval->setScrollbarsShown(true);
  minval->setCaretVisible(true);
  minval->setPopupMenuEnabled(true);
  minval->setText(TRANS("0"));

  addAndMakeVisible(label2 = new Label("new label",
    TRANS("Maximum value")));
  label2->setFont(Font(15.00f, Font::plain));
  label2->setJustificationType(Justification::centredLeft);
  label2->setEditable(false, false, false);
  label2->setColour(TextEditor::textColourId, Colours::black);
  label2->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

  addAndMakeVisible(maxval = new TextEditor("maxval"));
  maxval->setExplicitFocusOrder(2);
  maxval->setMultiLine(false);
  maxval->setReturnKeyStartsNewLine(false);
  maxval->setReadOnly(false);
  maxval->setScrollbarsShown(true);
  maxval->setCaretVisible(true);
  maxval->setPopupMenuEnabled(true);
  maxval->setText(TRANS("16383"));

  addAndMakeVisible(label3 = new Label("new label",
    TRANS("Pitch Wheel")));
  label3->setFont(Font(15.00f, Font::plain));
  label3->setJustificationType(Justification::centredLeft);
  label3->setEditable(false, false, false);
  label3->setColour(TextEditor::textColourId, Colours::black);
  label3->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

  //[UserPreSize]
  minval->setInputFilter(&numrestrict, false);
  maxval->setInputFilter(&numrestrict, false);
  minval->addListener(this);
  maxval->addListener(this);
    //[/UserPreSize]

  setSize(280, 350);

  //[Constructor] You can add your own custom stuff here..
  //[/Constructor]
}

PWoptions::~PWoptions() {
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

  label = nullptr;
  minval = nullptr;
  label2 = nullptr;
  maxval = nullptr;
  label3 = nullptr;

  //[Destructor]. You can add your own custom destruction code here..
  //[/Destructor]
}

//==============================================================================
void PWoptions::paint(Graphics& g) {
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

  g.fillAll(Colours::white);

  //[UserPaint] Add your own custom painting code here..
  //[/UserPaint]
}

void PWoptions::resized() {
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

  label->setBounds(32, 48, 150, 24); //-V112
  minval->setBounds(32, 80, 150, 24); //-V112
  label2->setBounds(32, 112, 150, 24); //-V112
  maxval->setBounds(32, 144, 150, 24); //-V112
  label3->setBounds(32, 16, 150, 24); //-V112
  //[UserResized] Add your own custom resize handling here..
  //[/UserResized]
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PWoptions::textEditorFocusLost(TextEditor& t) {
  short val = static_cast<short>(t.getText().getIntValue());
  String nam = t.getName();
  if (nam == "minval")
    controls_model_->setPWmin(boundchannel, val);
  else if (nam == "maxval")
    controls_model_->setPWmax(boundchannel, val);
}

void PWoptions::bindToControl(size_t channel) noexcept {
  boundchannel = channel;
}
//[/MiscUserCode]

//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PWoptions" componentName=""
                 parentClasses="public Component, private TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="280"
                 initialHeight="350">
  <BACKGROUND backgroundColour="ffffffff"/>
  <LABEL name="new label" id="c82a04232ee6984b" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="32 48 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Minimum value" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="minval" id="7a25d92146885d49" memberName="minval" virtualName=""
              explicitFocusOrder="1" pos="32 80 150 24" initialText="0" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <LABEL name="new label" id="819fead18214db34" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="32 112 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Maximum value" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="maxval" id="747392110d105b58" memberName="maxval" virtualName=""
              explicitFocusOrder="2" pos="32 144 150 24" initialText="16383"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="a391a4190e7e155c" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="32 16 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Pitch Wheel" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//[EndFile] You can add extra defines here...
//[/EndFile]