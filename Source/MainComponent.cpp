/*

*/

#include "MainComponent.h"
#include "LR_IPC.h"

//==============================================================================
MainContentComponent::MainContentComponent() : _titleLabel("Title", "MIDI2LR"),
											   _commandLabel("Command", ""),
											   _commandTable("Table", nullptr),
											   _commandTableModel()
{
	// Main title
	_titleLabel.setFont(Font(36.f, Font::bold));
	_titleLabel.setEditable(false);
	_titleLabel.setColour(Label::textColourId, Colours::darkgrey);
	_titleLabel.setComponentEffect(&_titleShadow);
	addAndMakeVisible(_titleLabel);

	// Last MIDI command
	_commandLabel.setFont(Font(12.f, Font::bold));
	_commandLabel.setEditable(false);
	_commandLabel.setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(_commandLabel);

	// Add ourselves as a listener for MIDI commands
	MIDIProcessor::getInstance().addMIDICommandListener(this);

	// Command Table
	_commandTable.setModel(&_commandTableModel);
	addAndMakeVisible(_commandTable);

    setSize (400, 600);

	// Start LR IPC
	LR_IPC::getInstance();
	 
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void MainContentComponent::resized()
{
	_titleLabel.setBoundsRelative(.1f, 0.f, .8f, .15f);
	_commandLabel.setBoundsRelative(.8f, 0.025f, .2f, .10f);
	_commandTable.setBoundsRelative(.1f, .2f, .8f, .8f);
}

// Update MIDI command components
void MainContentComponent::handleAsyncUpdate()
{
	// Update the last command label and set its colour to green
	_commandLabel.setText(_lastCommand, NotificationType::dontSendNotification);
	_commandLabel.setColour(Label::backgroundColourId, Colours::greenyellow);
	startTimer(1000);

	// Update the command table to add and/or select row corresponding to midi command
	_commandTable.updateContent();
	_commandTable.selectRow(_rowToSelect);
}

void MainContentComponent::handleMidiCC(int midiChannel, int controller, int value)
{
	_lastCommand = String::formatted("%d: CC%d [%d]", midiChannel, controller, value);
	_commandTableModel.addRow(midiChannel, controller);
	_rowToSelect = _commandTableModel.getRowForController(midiChannel, controller);
	triggerAsyncUpdate();
}

void MainContentComponent::handleMidiNote(int midiChannel, int note)
{
	// _lastCommand = String::formatted("%d: Note%d", midiChannel, note);
	// triggerAsyncUpdate();
}

void MainContentComponent::timerCallback()
{
	// reset the command label's background to white
	_commandLabel.setColour(Label::backgroundColourId, Colours::white);
	stopTimer();
}