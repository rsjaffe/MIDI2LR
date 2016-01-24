/*
  ==============================================================================
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "MainComponent.h"
#include "LR_IPC_IN.h"
#include "MIDISender.h"

#include "SettingsComponent.h"

#define MAIN_WIDTH 400
#define MAIN_HEIGHT 650
#define MAIN_LEFT 20
#define SPACEBETWEENBUTTON 10


//==============================================================================
MainContentComponent::MainContentComponent(): ResizableLayout(this), _titleLabel("Title", "MIDI2LR"),
_connectionLabel("Connection", "Not connected to LR"),
_commandLabel("Command", ""),
_commandTable("Table", nullptr),
_commandTableModel(),
_rescanButton("Rescan MIDI devices"),
_removeRowButton("Remove selected row"),
_saveButton("Save"),
_loadButton("Load"),
_versionLabel("Version", "Version " +
    String(ProjectInfo::versionString)),
    _settingsButton("Settings"),
    _profileNameLabel("ProfileNameLabel", ""),
    m_currentStatus("CurrentStatus", "no extra info"),
	m_commandMap(NULL),
	m_lr_IPC_IN(NULL),
	m_lr_IPC_OUT(NULL)
{
	   
}

MainContentComponent::~MainContentComponent()
{}

void MainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void MainContentComponent::SetLabelSettings(Label &lblToSet)
{
    lblToSet.setFont(Font(12.f, Font::bold));
    lblToSet.setEditable(false);
    lblToSet.setColour(Label::textColourId, Colours::darkgrey);
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
    // Display the CC parameters and add/highlight row in table corresponding to the CC
    _lastCommand = String::formatted("%d: CC%d [%d]", midiChannel, controller, value);
    _commandTableModel.addRow(midiChannel, controller, true);
    _rowToSelect = _commandTableModel.getRowForMessage(midiChannel, controller, true);
    triggerAsyncUpdate();
}

void MainContentComponent::handleMidiNote(int midiChannel, int note)
{
    // Display the Note parameters and add/highlight row in table corresponding to the Note
    _lastCommand = String::formatted("%d: Note [%d]", midiChannel, note);
    _commandTableModel.addRow(midiChannel, note, false);
    _rowToSelect = _commandTableModel.getRowForMessage(midiChannel, note, false);
    triggerAsyncUpdate();
}

void MainContentComponent::connected()
{
    _connectionLabel.setText("Connected to LR", NotificationType::dontSendNotification);
    _connectionLabel.setColour(Label::backgroundColourId, Colours::greenyellow);
}

void MainContentComponent::disconnected()
{
    _connectionLabel.setText("Not connected to LR", NotificationType::dontSendNotification);
    _connectionLabel.setColour(Label::backgroundColourId, Colours::red);
}

void MainContentComponent::timerCallback()
{
    // reset the command label's background to white
    _commandLabel.setColour(Label::backgroundColourId, Colours::white);
    stopTimer();
}

void MainContentComponent::buttonClicked(Button* button)
{
    if (button == &_rescanButton)
    {
        // Re-enumerate MIDI IN and OUT devices
        MIDIProcessor::getInstance().rescanDevices();
        MIDISender::getInstance().rescanDevices();
		// Send new CC parameters to MIDI Out devices
		if (m_lr_IPC_IN)
		{
			m_lr_IPC_IN->refreshMIDIOutput();
		}		
    }
    else if (button == &_removeRowButton)
    {
        if (_commandTable.getSelectedRow() != -1)
        {
            _commandTableModel.removeRow(_commandTable.getSelectedRow());
            _commandTable.updateContent();
        }
    }
    else if (button == &_saveButton)
    {
        bool profileDirSet = SettingsManager::getInstance().getProfileDirectory().isNotEmpty();
		WildcardFileFilter wildcardFilter("*.xml", String::empty, "MIDI2LR profiles");
        FileBrowserComponent browser(FileBrowserComponent::canSelectFiles | FileBrowserComponent::saveMode |
            FileBrowserComponent::warnAboutOverwriting,
            profileDirSet ? SettingsManager::getInstance().getProfileDirectory() : File::getCurrentWorkingDirectory(),
			&wildcardFilter,        nullptr);
        FileChooserDialogBox dialogBox("Save profile",
            "Enter filename to save profile",
            browser,
            true,
            Colours::lightgrey);
        if (dialogBox.show())
        {
            File selectedFile = browser.getSelectedFile(0).withFileExtension("xml");
            CommandMap::getInstance().toXMLDocument(selectedFile);
        }
    }
    else if (button == &_loadButton)
    {
        bool profileDirSet = SettingsManager::getInstance().getProfileDirectory().isNotEmpty();
        WildcardFileFilter wildcardFilter("*.xml", String::empty, "MIDI2LR profiles");
        FileBrowserComponent browser(FileBrowserComponent::canSelectFiles | FileBrowserComponent::openMode,
            profileDirSet ? SettingsManager::getInstance().getProfileDirectory() : File::getCurrentWorkingDirectory(),
            &wildcardFilter,
            nullptr);
        FileChooserDialogBox dialogBox("Open profile",
            "Select a profile to open",
            browser,
            true,
            Colours::lightgrey);

        if (dialogBox.show())
        {
            ScopedPointer<XmlElement> elem = XmlDocument::parse(browser.getSelectedFile(0));
            if (elem)
            {
                File newprofile = browser.getSelectedFile(0);
                String command = String("ChangedToFullPath ") + newprofile.getFullPathName() + "\n";
                LR_IPC_OUT::getInstance().sendCommand(command);
                _profileNameLabel.setText(newprofile.getFileName(), NotificationType::dontSendNotification);
                _commandTableModel.buildFromXml(elem);
                _commandTable.updateContent();
                _commandTable.repaint();
            }
        }
    }
    else if (button == &_settingsButton)
    {
        DialogWindow::LaunchOptions dwOpt;
        dwOpt.dialogTitle = "Settings";
        dwOpt.content.setOwned(new SettingsComponent());
        dwOpt.content->setSize(400, 300);
        dwOpt.escapeKeyTriggersCloseButton = true;
        dwOpt.useNativeTitleBar = false;
        _settingsDialog = dwOpt.create();
        _settingsDialog->setVisible(true);
    }
}

void MainContentComponent::profileChanged(XmlElement* elem, const String& filename)
{
    _commandTableModel.buildFromXml(elem);
    _commandTable.updateContent();
    _commandTable.repaint();
    _profileNameLabel.setText(filename, NotificationType::dontSendNotification);
    _systemTrayComponent.showInfoBubble(filename, "Profile loaded");

    // Send new CC parameters to MIDI Out devices
	if (m_lr_IPC_IN)
	{
		m_lr_IPC_IN->refreshMIDIOutput();
	}    
}

void MainContentComponent::SetTimerText(int timeValue)
{
	if (timeValue > 0)
	{
		m_currentStatus.setText(String::formatted("Hiding in %i Sec.", timeValue), NotificationType::dontSendNotification);
	}
	else
	{
		m_currentStatus.setText("", NotificationType::dontSendNotification);
	}

}

void MainContentComponent::Init(CommandMap *commandMap, LR_IPC_IN *in, LR_IPC_OUT *out, MIDIProcessor *midiProcessor, ProfileManager *profileManager, SettingsManager *settingsManager)
{
	//copy the pointers
	m_commandMap = commandMap;
	m_lr_IPC_IN = in;
	m_lr_IPC_OUT = out;
		
	//call the function of the sub component.
	_commandTableModel.Init(commandMap);
	
	if (midiProcessor)
	{
		// Add ourselves as a listener for MIDI commands
		midiProcessor->addMIDICommandListener(this);
	}
	
	if (m_lr_IPC_OUT)
	{
		// Add ourselves as a listener for LR_IPC_OUT events
		m_lr_IPC_OUT->addListener(this);
	}

	if (profileManager)
	{
		// Add ourselves as a listener for profile changes
		profileManager->addListener(this);
	}

	//Set the component size
	setSize(MAIN_WIDTH, MAIN_HEIGHT);

	// Main title
	_titleLabel.setFont(Font(36.f, Font::bold));
	_titleLabel.setEditable(false);
	_titleLabel.setColour(Label::textColourId, Colours::darkgrey);
	_titleLabel.setComponentEffect(&_titleShadow);
	_titleLabel.setBounds(MAIN_LEFT, 10, MAIN_WIDTH - 2 * MAIN_LEFT, 30);
	addToLayout(&_titleLabel, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_titleLabel);

	// Version label
	SetLabelSettings(_versionLabel);
	_versionLabel.setBounds(MAIN_LEFT, 40, MAIN_WIDTH - 2 * MAIN_LEFT, 10);
	addToLayout(&_versionLabel, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_versionLabel);


	// Connection status
	_connectionLabel.setFont(Font(12.f, Font::bold));
	_connectionLabel.setEditable(false);
	_connectionLabel.setColour(Label::backgroundColourId, Colours::red);
	_connectionLabel.setColour(Label::textColourId, Colours::black);
	_connectionLabel.setJustificationType(Justification::centred);
	_connectionLabel.setBounds(200, 15, MAIN_WIDTH - MAIN_LEFT - 200, 20);
	addToLayout(&_connectionLabel, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_connectionLabel);

	//get the button width
	long buttonWidth = (MAIN_WIDTH - 2 * MAIN_LEFT - SPACEBETWEENBUTTON * 2) / 3;

	// Load button
	_loadButton.addListener(this);
	_loadButton.setBounds(MAIN_LEFT, 60, buttonWidth, 20);
	addToLayout(&_loadButton, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_loadButton);

	// Save button
	_saveButton.addListener(this);
	_saveButton.setBounds(MAIN_LEFT + buttonWidth + SPACEBETWEENBUTTON, 60, buttonWidth, 20);
	addToLayout(&_saveButton, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_saveButton);

	// Settings button
	_settingsButton.addListener(this);
	_settingsButton.setBounds(MAIN_LEFT + buttonWidth * 2 + SPACEBETWEENBUTTON * 2, 60, buttonWidth, 20);
	addToLayout(&_settingsButton, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_settingsButton);



	// Command Table
	_commandTable.setModel(&_commandTableModel);
	_commandTable.setBounds(MAIN_LEFT, 100, MAIN_WIDTH - MAIN_LEFT * 2, MAIN_HEIGHT - 210);
	addToLayout(&_commandTable, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_commandTable);


	long labelWidth = (MAIN_WIDTH - MAIN_LEFT * 2) / 2;


	// Profile name label
	SetLabelSettings(_profileNameLabel);
	_profileNameLabel.setBounds(MAIN_LEFT, MAIN_HEIGHT - 100, labelWidth, 20);
	addToLayout(&_profileNameLabel, anchorMidLeft, anchorMidRight);
	_profileNameLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(_profileNameLabel);

	// Last MIDI command
	_commandLabel.setFont(Font(12.f, Font::bold));
	_commandLabel.setEditable(false);
	_commandLabel.setColour(Label::textColourId, Colours::darkgrey);
	_commandLabel.setBounds(MAIN_LEFT + labelWidth, MAIN_HEIGHT - 100, labelWidth, 20);
	addToLayout(&_commandLabel, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_commandLabel);

	// Remove row button
	_removeRowButton.addListener(this);
	_removeRowButton.setBounds(MAIN_LEFT, MAIN_HEIGHT - 75, MAIN_WIDTH - MAIN_LEFT * 2, 20);
	addToLayout(&_removeRowButton, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_removeRowButton);


	// Rescan MIDI button
	_rescanButton.addListener(this);
	_rescanButton.setBounds(MAIN_LEFT, MAIN_HEIGHT - 50, MAIN_WIDTH - MAIN_LEFT * 2, 20);
	addToLayout(&_rescanButton, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_rescanButton);

	// adding the current status label, used for counting down.
	m_currentStatus.setBounds(MAIN_LEFT, MAIN_HEIGHT - 30, MAIN_WIDTH - MAIN_LEFT * 2, 20);
	addToLayout(&m_currentStatus, anchorMidLeft, anchorMidRight);
	m_currentStatus.setJustificationType(Justification::centred);
	SetLabelSettings(m_currentStatus);
	addAndMakeVisible(m_currentStatus);

	_systemTrayComponent.setIconImage(ImageCache::getFromMemory(BinaryData::MIDI2LR_png, BinaryData::MIDI2LR_pngSize));



	// Try to load a default.xml if the user has not set a profile directory
	if (SettingsManager::getInstance().getProfileDirectory().isEmpty())
	{
		File defaultProfile = File::getSpecialLocation(File::currentExecutableFile).getSiblingFile("default.xml");
		ScopedPointer<XmlElement> elem = XmlDocument::parse(defaultProfile);
		if (elem)
		{
			_commandTableModel.buildFromXml(elem);
			_commandTable.updateContent();
		}
	}
	else
	{
		// otherwise use the last profile from the profile directory
		ProfileManager::getInstance().switchToProfile(0);
	}

	// turn it on
	activateLayout();



}