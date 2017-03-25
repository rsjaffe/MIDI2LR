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

CCoptions.cpp

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

#include "CCoptions.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
#include "ControlsModel.h"
ControlsModel* CCoptions::controls_model_{nullptr};
//[/MiscUserDefs]

//==============================================================================
CCoptions::CCoptions()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible(groupComponent = new GroupComponent("CCmethod",
        TRANS("CC Message Type")));

    addAndMakeVisible(twosbutton = new ToggleButton("twoscomp"));
    twosbutton->setTooltip(TRANS("Control value is 1 or greater when turned one way, and 127 or smaller when turned the other."));
    twosbutton->setExplicitFocusOrder(2);
    twosbutton->setButtonText(TRANS("Two\'s complement"));
    twosbutton->setConnectedEdges(Button::ConnectedOnTop|Button::ConnectedOnBottom);
    twosbutton->setRadioGroupId(1);
    twosbutton->addListener(this);

    addAndMakeVisible(absbutton = new ToggleButton("Absolute"));
    absbutton->setTooltip(TRANS("Control value increases when turned one way, decreases when turned the other."));
    absbutton->setExplicitFocusOrder(1);
    absbutton->setConnectedEdges(Button::ConnectedOnBottom);
    absbutton->setRadioGroupId(1);
    absbutton->addListener(this);
    absbutton->setToggleState(true, dontSendNotification);

    addAndMakeVisible(binbutton = new ToggleButton("Binaryoffset"));
    binbutton->setTooltip(TRANS("Control value is near 63 when turned one way, and near 65 when turned the other."));
    binbutton->setExplicitFocusOrder(3);
    binbutton->setButtonText(TRANS("Binary offset"));
    binbutton->setConnectedEdges(Button::ConnectedOnTop|Button::ConnectedOnBottom);
    binbutton->setRadioGroupId(1);
    binbutton->addListener(this);

    addAndMakeVisible(signbutton = new ToggleButton("Signmagnitude"));
    signbutton->setTooltip(TRANS("Control value is near 1 when turned one way, and near 65 when turned the other."));
    signbutton->setExplicitFocusOrder(4); //-V112
    signbutton->setButtonText(TRANS("Sign and magnitude"));
    signbutton->setConnectedEdges(Button::ConnectedOnTop);
    signbutton->setRadioGroupId(1);
    signbutton->addListener(this);

    addAndMakeVisible(maxvaltext = new TextEditor("maxvaltext"));
    maxvaltext->setExplicitFocusOrder(6);
    maxvaltext->setMultiLine(false);
    maxvaltext->setReturnKeyStartsNewLine(false);
    maxvaltext->setReadOnly(false);
    maxvaltext->setScrollbarsShown(true);
    maxvaltext->setCaretVisible(true);
    maxvaltext->setPopupMenuEnabled(true);
    maxvaltext->setText(TRANS("127"));

    addAndMakeVisible(minvaltext = new TextEditor("minvaltext"));
    minvaltext->setExplicitFocusOrder(5);
    minvaltext->setMultiLine(false);
    minvaltext->setReturnKeyStartsNewLine(false);
    minvaltext->setReadOnly(false);
    minvaltext->setScrollbarsShown(true);
    minvaltext->setCaretVisible(true);
    minvaltext->setPopupMenuEnabled(true);
    minvaltext->setText(TRANS("0"));

    addAndMakeVisible(minvallabel = new Label("new label",
        TRANS("Minimum value\n")));
    minvallabel->setFont(Font(15.00f, Font::plain));
    minvallabel->setJustificationType(Justification::centredLeft);
    minvallabel->setEditable(false, false, false);
    minvallabel->setColour(TextEditor::textColourId, Colours::black);
    minvallabel->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    addAndMakeVisible(maxvallabel = new Label("maxlabel",
        TRANS("Maximum value")));
    maxvallabel->setFont(Font(15.00f, Font::plain));
    maxvallabel->setJustificationType(Justification::centredLeft);
    maxvallabel->setEditable(false, false, false);
    maxvallabel->setColour(TextEditor::textColourId, Colours::black);
    maxvallabel->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    addAndMakeVisible(applyAll = new TextButton("new button"));
    applyAll->setTooltip(TRANS("Apply these settings to all similar controls."));
    applyAll->setExplicitFocusOrder(7);
    applyAll->setButtonText(TRANS("Apply to all"));
    applyAll->addListener(this);

    addAndMakeVisible(controlID = new Label("channel 0 number 0",
        TRANS("Channel 0 Number 0")));
    controlID->setFont(Font(15.00f, Font::plain));
    controlID->setJustificationType(Justification::centred);
    controlID->setEditable(false, false, false);
    controlID->setColour(TextEditor::textColourId, Colours::black);
    controlID->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    //[UserPreSize]
        //[/UserPreSize]

    setSize(280, 350);

    //[Constructor] You can add your own custom stuff here..
    maxvaltext->setInputFilter(&numrestrict, false);
    minvaltext->setInputFilter(&numrestrict, false);
    maxvaltext->addListener(this);
    minvaltext->addListener(this);
    //[/Constructor]
}

CCoptions::~CCoptions()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    groupComponent = nullptr;
    twosbutton = nullptr;
    absbutton = nullptr;
    binbutton = nullptr;
    signbutton = nullptr;
    maxvaltext = nullptr;
    minvaltext = nullptr;
    minvallabel = nullptr;
    maxvallabel = nullptr;
    applyAll = nullptr;
    controlID = nullptr;

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CCoptions::paint(Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll(Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CCoptions::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    groupComponent->setBounds(16, 60, 240, 157);
    twosbutton->setBounds(40, 117, 150, 24);
    absbutton->setBounds(40, 85, 150, 24);
    binbutton->setBounds(40, 149, 150, 24);
    signbutton->setBounds(40, 181, 150, 24);
    maxvaltext->setBounds(200, 268, 56, 24);
    minvaltext->setBounds(200, 228, 56, 24);
    minvallabel->setBounds(16, 228, 150, 24);
    maxvallabel->setBounds(16, 268, 150, 24);
    applyAll->setBounds((getWidth()/2)-(150/2), (getHeight()/2)+141, 150, 24);
    controlID->setBounds((getWidth()/2)-(248/2), 16, 248, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CCoptions::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked==twosbutton) {
        //[UserButtonCode_twosbutton] -- add your button handler code here..
        minvaltext->setVisible(false);
        minvallabel->setVisible(false);
        maxvallabel->setText(TRANS("Resolution"), juce::dontSendNotification);
        minvaltext->setText("0", juce::dontSendNotification);
        controls_model_->setCCmethod(boundchannel, boundnumber, RSJ::CCmethod::twoscomplement);
        //[/UserButtonCode_twosbutton]
    }
    else if (buttonThatWasClicked==absbutton) {
        //[UserButtonCode_absbutton] -- add your button handler code here..
        minvaltext->setVisible(true);
        minvallabel->setVisible(true);
        maxvallabel->setText(TRANS("Maximum value"), juce::dontSendNotification);
        controls_model_->setCCmethod(boundchannel, boundnumber, RSJ::CCmethod::absolute);
        //[/UserButtonCode_absbutton]
    }
    else if (buttonThatWasClicked==binbutton) {
        //[UserButtonCode_binbutton] -- add your button handler code here..
        minvaltext->setVisible(false);
        minvallabel->setVisible(false);
        maxvallabel->setText(TRANS("Resolution"), juce::dontSendNotification);
        minvaltext->setText("0", juce::dontSendNotification);
        controls_model_->setCCmethod(boundchannel, boundnumber, RSJ::CCmethod::binaryoffset);

        //[/UserButtonCode_binbutton]
    }
    else if (buttonThatWasClicked==signbutton) {
        //[UserButtonCode_signbutton] -- add your button handler code here..
        minvaltext->setVisible(false);
        minvallabel->setVisible(false);
        maxvallabel->setText(TRANS("Resolution"), juce::dontSendNotification);
        minvaltext->setText("0", juce::dontSendNotification);
        controls_model_->setCCmethod(boundchannel, boundnumber, RSJ::CCmethod::signmagnitude);
        //[/UserButtonCode_signbutton]
    }
    else if (buttonThatWasClicked==applyAll) {
        //[UserButtonCode_applyAll] -- add your button handler code here..
        RSJ::CCmethod ccm;
        if (twosbutton->getToggleState())
            ccm = RSJ::CCmethod::twoscomplement;
        else if (absbutton->getToggleState())
            ccm = RSJ::CCmethod::absolute;
        else if (binbutton->getToggleState())
            ccm = RSJ::CCmethod::binaryoffset;
        else if (signbutton->getToggleState())
            ccm = RSJ::CCmethod::signmagnitude;
        else {
            ccm = RSJ::CCmethod::absolute;
            assert(!"Should be unreachable apply all button");
        }
        controls_model_->setCCall(boundchannel, boundnumber,
            static_cast<short>(minvaltext->getText().getIntValue()),
            static_cast<short>(maxvaltext->getText().getIntValue()), ccm);
        //[/UserButtonCode_applyAll]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void CCoptions::textEditorFocusLost(TextEditor& t)
{
    short val = static_cast<short>(t.getText().getIntValue());
    String nam = t.getName();
    if (nam=="minvaltext")
        controls_model_->setCCmin(boundchannel, boundnumber, val);
    else if (nam=="maxvaltext")
        controls_model_->setCCmax(boundchannel, boundnumber, val);
}

void CCoptions::bindToControl(size_t channel, short number)
{
    boundchannel = static_cast<short>(channel);
    boundnumber = number;
    controlID->setText("channel "+juce::String(static_cast<unsigned>(channel))+" number "+juce::String(number),
        juce::dontSendNotification);
    minvaltext->setText(juce::String(controls_model_->getCCmin(boundchannel, boundnumber)), juce::dontSendNotification);
    maxvaltext->setText(juce::String(controls_model_->getCCmax(boundchannel, boundnumber)), juce::dontSendNotification);
    switch (controls_model_->getCCmethod(boundchannel, boundnumber)) {
    case RSJ::CCmethod::absolute:
        absbutton->setToggleState(true, juce::sendNotification);
        break;
    case RSJ::CCmethod::binaryoffset:
        binbutton->setToggleState(true, juce::sendNotification);
        break;
    case RSJ::CCmethod::signmagnitude:
        signbutton->setToggleState(true, juce::sendNotification);
        break;
    case RSJ::CCmethod::twoscomplement:
        twosbutton->setToggleState(true, juce::sendNotification);
        break;
    }
}
//[/MiscUserCode]

//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CCoptions" componentName=""
                 parentClasses="public Component, private TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="280"
                 initialHeight="350">
  <BACKGROUND backgroundColour="ffffffff"/>
  <GROUPCOMPONENT name="CCmethod" id="3dee10ca9db3e476" memberName="groupComponent"
                  virtualName="" explicitFocusOrder="0" pos="16 60 240 157" title="CC Message Type"/>
  <TOGGLEBUTTON name="twoscomp" id="9944ff0444a9d3c2" memberName="twosbutton"
                virtualName="" explicitFocusOrder="2" pos="40 117 150 24" tooltip="Control value is 1 or greater when turned one way, and 127 or smaller when turned the other."
                buttonText="Two's complement" connectedEdges="12" needsCallback="1"
                radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="Absolute" id="675cf3a684e30d96" memberName="absbutton"
                virtualName="" explicitFocusOrder="1" pos="40 85 150 24" tooltip="Control value increases when turned one way, decreases when turned the other."
                buttonText="Absolute" connectedEdges="8" needsCallback="1" radioGroupId="1"
                state="1"/>
  <TOGGLEBUTTON name="Binaryoffset" id="e846c902424344be" memberName="binbutton"
                virtualName="" explicitFocusOrder="3" pos="40 149 150 24" tooltip="Control value is near 63 when turned one way, and near 65 when turned the other."
                buttonText="Binary offset" connectedEdges="12" needsCallback="1"
                radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="Signmagnitude" id="e25fdd5195294f80" memberName="signbutton"
                virtualName="" explicitFocusOrder="4" pos="40 181 150 24" tooltip="Control value is near 1 when turned one way, and near 65 when turned the other."
                buttonText="Sign and magnitude" connectedEdges="4" needsCallback="1"
                radioGroupId="1" state="0"/>
  <TEXTEDITOR name="maxvaltext" id="3c2be035ac39e391" memberName="maxvaltext"
              virtualName="" explicitFocusOrder="6" pos="200 268 56 24" initialText="127"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="minvaltext" id="fa6314befd0fab62" memberName="minvaltext"
              virtualName="" explicitFocusOrder="5" pos="200 228 56 24" initialText="0"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="d2621d0baef5fe94" memberName="minvallabel"
         virtualName="" explicitFocusOrder="0" pos="16 228 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Minimum value&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="maxlabel" id="e9c5639016533dbc" memberName="maxvallabel"
         virtualName="" explicitFocusOrder="0" pos="16 268 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Maximum value" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="836af06f251dc94d" memberName="applyAll"
              virtualName="" explicitFocusOrder="7" pos="0Cc 141C 150 24" tooltip="Apply these settings to all similar controls."
              buttonText="Apply to all" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <LABEL name="channel 0 number 0" id="aa2312920c3b6ed" memberName="controlID"
         virtualName="" explicitFocusOrder="0" pos="0Cc 16 248 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Channel 0 Number 0" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//[EndFile] You can add extra defines here...
//[/EndFile]