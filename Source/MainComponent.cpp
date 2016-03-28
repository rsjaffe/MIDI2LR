/*
  ==============================================================================

  MainComponent.cpp

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

constexpr auto MainWidth = 400;
constexpr auto MainHeight = 650;
constexpr auto MainLeft = 20;
constexpr auto SpaceBetweenButton = 10;

/**********************************************************************************************//**
 * @fn  MainContentComponent::MainContentComponent(): ResizableLayout(this), _titleLabel("Title", "MIDI2LR"), _connectionLabel("Connection", "Not connected to LR"), _commandLabel("Command", ""), _commandTable("Table", nullptr), _commandTableModel(), _rescanButton("Rescan MIDI devices"), _removeRowButton("Remove selected row"), _saveButton("Save"), _loadButton("Load"), _versionLabel("Version", "Version " + String(ProjectInfo::versionString)), _settingsButton("Settings"), _profileNameLabel("ProfileNameLabel", ""), m_currentStatus("CurrentStatus", "no extra info"), m_commandMap(nullptr), m_lr_IPC_IN(nullptr), m_lr_IPC_OUT(nullptr), m_settingsManager(nullptr), m_midiProcessor(nullptr), m_midiSender(nullptr)
 *
 * @brief   Default constructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

MainContentComponent::MainContentComponent(): ResizableLayout{ this }, _titleLabel{ "Title", "MIDI2LR" },
_connectionLabel{ "Connection", "Not connected to LR" },
_commandLabel{ "Command", "" },
_commandTable{ "Table", nullptr },
_commandTableModel{},
_rescanButton{ "Rescan MIDI devices" },
_removeRowButton{ "Remove selected row" },
_saveButton{ "Save" },
_loadButton{ "Load" },
_versionLabel{ "Version", "Version " +
    String{ProjectInfo::versionString} },
    _settingsButton{ "Settings" },
    _profileNameLabel{ "ProfileNameLabel", "" },
    m_currentStatus{ "CurrentStatus", "no extra info" },
    m_commandMap{ nullptr },
    m_lr_IPC_IN{ nullptr },
    m_lr_IPC_OUT{ nullptr },
    m_settingsManager{ nullptr },
    m_midiProcessor{ nullptr },
    m_midiSender{ nullptr }
{

}

/**********************************************************************************************//**
 * @fn  MainContentComponent::~MainContentComponent()
 *
 * @brief   Destructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

MainContentComponent::~MainContentComponent()
{}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::paint(Graphics& g)
 *
 * @brief   Paints the given g.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  g   The Graphics to process.
 **************************************************************************************************/

void MainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::SetLabelSettings(Label &lblToSet)
 *
 * @brief   Sets label settings.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  lblToSet    Set the label to belongs to.
 **************************************************************************************************/

void MainContentComponent::SetLabelSettings(Label &lblToSet)
{
    lblToSet.setFont(Font{ 12.f, Font::bold });
    lblToSet.setEditable(false);
    lblToSet.setColour(Label::textColourId, Colours::darkgrey);
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::handleAsyncUpdate()
 *
 * @brief   Update MIDI command components.
 *
 * @date    3/22/2016
 **************************************************************************************************/

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

/**********************************************************************************************//**
 * @fn  void MainContentComponent::handleMidiCC(int midiChannel, int controller, int value)
 *
 * @brief   Handles the MIDI Cc.
 *
 * @date    3/22/2016
 *
 * @param   midiChannel The MIDI channel.
 * @param   controller  The controller.
 * @param   value       The value.
 **************************************************************************************************/

void MainContentComponent::handleMidiCC(int midiChannel, int controller, int value)
{
    // Display the CC parameters and add/highlight row in table corresponding to the CC
    _lastCommand = String::formatted("%d: CC%d [%d]", midiChannel, controller, value);
    _commandTableModel.addRow(midiChannel, controller, true);
    _rowToSelect = _commandTableModel.getRowForMessage(midiChannel, controller, true);
    triggerAsyncUpdate();
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::handleMidiNote(int midiChannel, int note)
 *
 * @brief   Handles the MIDI note.
 *
 * @date    3/22/2016
 *
 * @param   midiChannel The MIDI channel.
 * @param   note        The note.
 **************************************************************************************************/

void MainContentComponent::handleMidiNote(int midiChannel, int note)
{
    // Display the Note parameters and add/highlight row in table corresponding to the Note
    _lastCommand = String::formatted("%d: Note [%d]", midiChannel, note);
    _commandTableModel.addRow(midiChannel, note, false);
    _rowToSelect = _commandTableModel.getRowForMessage(midiChannel, note, false);
    triggerAsyncUpdate();
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::connected()
 *
 * @brief   Connected this object.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MainContentComponent::connected()
{
    _connectionLabel.setText("Connected to LR", NotificationType::dontSendNotification);
    _connectionLabel.setColour(Label::backgroundColourId, Colours::greenyellow);
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::disconnected()
 *
 * @brief   Disconnect from the ed.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MainContentComponent::disconnected()
{
    _connectionLabel.setText("Not connected to LR", NotificationType::dontSendNotification);
    _connectionLabel.setColour(Label::backgroundColourId, Colours::red);
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::timerCallback()
 *
 * @brief   Callback, called when the timer.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MainContentComponent::timerCallback()
{
    // reset the command label's background to white
    _commandLabel.setColour(Label::backgroundColourId, Colours::white);
    stopTimer();
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::buttonClicked(Button* button)
 *
 * @brief   Button clicked.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  button  If non-null, the button.
 **************************************************************************************************/

void MainContentComponent::buttonClicked(Button* button)
{
    if (button == &_rescanButton)
    {
        // Re-enumerate MIDI IN and OUT devices	

        if (m_midiProcessor)
        {
            m_midiProcessor->rescanDevices();
        }

        if (m_midiSender)
        {
            m_midiSender->rescanDevices();
        }
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

        File profileDir;

        if (m_settingsManager)
        {
            profileDir = m_settingsManager->getProfileDirectory();
        }

        if (!profileDir.exists())
        {
            profileDir = File::getCurrentWorkingDirectory();
        }

        WildcardFileFilter wildcardFilter{ "*.xml", String::empty, "MIDI2LR profiles" };
        FileBrowserComponent browser{ FileBrowserComponent::canSelectFiles | FileBrowserComponent::saveMode |
            FileBrowserComponent::warnAboutOverwriting, profileDir, &wildcardFilter, nullptr };
        FileChooserDialogBox dialogBox{ "Save profile",
            "Enter filename to save profile",
            browser,
            true,
            Colours::lightgrey };
        if (dialogBox.show())
        {
            File selectedFile = browser.getSelectedFile(0).withFileExtension("xml");

            if (m_commandMap)
            {
                m_commandMap->toXMLDocument(selectedFile);
            }
        }
    }
    else if (button == &_loadButton)
    {
        File profileDir;

        if (m_settingsManager)
        {
            profileDir = m_settingsManager->getProfileDirectory();
        }

        if (!profileDir.exists())
        {
            profileDir = File::getCurrentWorkingDirectory();
        }


        WildcardFileFilter wildcardFilter{ "*.xml", String::empty, "MIDI2LR profiles" };
        FileBrowserComponent browser{ FileBrowserComponent::canSelectFiles | FileBrowserComponent::openMode, profileDir, &wildcardFilter, nullptr };
        FileChooserDialogBox dialogBox{ "Open profile", "Select a profile to open", browser, true, Colours::lightgrey };

        if (dialogBox.show())
        {
            std::unique_ptr<XmlElement> elem{ XmlDocument::parse(browser.getSelectedFile(0)) };
            if (elem)
            {
                auto newprofile = browser.getSelectedFile(0);
                auto command = String{ "ChangedToFullPath " } + newprofile.getFullPathName() + "\n";

                if (m_lr_IPC_OUT)
                {
                    m_lr_IPC_OUT->sendCommand(command);
                }
                _profileNameLabel.setText(newprofile.getFileName(), NotificationType::dontSendNotification);
                _commandTableModel.buildFromXml(elem.get());
                _commandTable.updateContent();
                _commandTable.repaint();
            }
        }
    }
    else if (button == &_settingsButton)
    {
        DialogWindow::LaunchOptions dwOpt;
        dwOpt.dialogTitle = "Settings";
        //create new object
        auto *comp = new SettingsComponent{};
        comp->Init(m_settingsManager);
        dwOpt.content.setOwned(comp);
        dwOpt.content->setSize(400, 300);
        dwOpt.escapeKeyTriggersCloseButton = true;
        dwOpt.useNativeTitleBar = false;
        _settingsDialog.reset(dwOpt.create());
        _settingsDialog->setVisible(true);
    }
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::profileChanged(XmlElement* elem, const String& filename)
 *
 * @brief   Profile changed.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  elem    If non-null, the element.
 * @param   filename        Filename of the file.
 **************************************************************************************************/

void MainContentComponent::profileChanged(XmlElement* elem, const String& filename)
{
    _commandTableModel.buildFromXml(elem);
    _commandTable.updateContent();
    _commandTable.repaint();
    _profileNameLabel.setText(filename, NotificationType::dontSendNotification);
//  _systemTrayComponent.showInfoBubble(filename, "Profile loaded");

    // Send new CC parameters to MIDI Out devices
    if (m_lr_IPC_IN)
    {
        m_lr_IPC_IN->refreshMIDIOutput();
    }
}

/**********************************************************************************************//**
 * @fn  void MainContentComponent::SetTimerText(int timeValue)
 *
 * @brief   Sets timer text.
 *
 * @date    3/22/2016
 *
 * @param   timeValue   The time value.
 **************************************************************************************************/

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

/**********************************************************************************************//**
 * @fn  void MainContentComponent::Init(std::shared_ptr<CommandMap>& commandMap, std::shared_ptr<LR_IPC_IN>& in, std::shared_ptr<LR_IPC_OUT>& out, std::shared_ptr<MIDIProcessor>& midiProcessor, std::shared_ptr<ProfileManager>& profileManager, std::shared_ptr<SettingsManager>& settingsManager, std::shared_ptr<MIDISender>& midiSender)
 *
 * @brief   S.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  commandMap      If non-null, the command map.
 * @param [in,out]  in              If non-null, the in.
 * @param [in,out]  out             If non-null, the out.
 * @param [in,out]  midiProcessor   If non-null, the MIDI processor.
 * @param [in,out]  profileManager  If non-null, manager for profile.
 * @param [in,out]  settingsManager If non-null, manager for settings.
 * @param [in,out]  midiSender      If non-null, the MIDI sender.
 **************************************************************************************************/

void MainContentComponent::Init(std::shared_ptr<CommandMap>& commandMap, std::shared_ptr<LR_IPC_IN>& in,
    std::shared_ptr<LR_IPC_OUT>& out, std::shared_ptr<MIDIProcessor>& midiProcessor, std::shared_ptr<ProfileManager>& profileManager,
    std::shared_ptr<SettingsManager>& settingsManager, std::shared_ptr<MIDISender>& midiSender)
{
    //copy the pointers
    m_commandMap = commandMap;
    m_lr_IPC_IN = in;
    m_lr_IPC_OUT = out;
    m_settingsManager = settingsManager;
    m_midiProcessor = midiProcessor;
    m_midiSender = midiSender;

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
    setSize(MainWidth, MainHeight);

    // Main title
    _titleLabel.setFont(Font{ 36.f, Font::bold });
    _titleLabel.setEditable(false);
    _titleLabel.setColour(Label::textColourId, Colours::darkgrey);
    _titleLabel.setComponentEffect(&_titleShadow);
    _titleLabel.setBounds(MainLeft, 10, MainWidth - 2 * MainLeft, 30);
    addToLayout(&_titleLabel, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_titleLabel);

    // Version label
    SetLabelSettings(_versionLabel);
    _versionLabel.setBounds(MainLeft, 40, MainWidth - 2 * MainLeft, 10);
    addToLayout(&_versionLabel, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_versionLabel);


    // Connection status
    _connectionLabel.setFont(Font{ 12.f, Font::bold });
    _connectionLabel.setEditable(false);
    _connectionLabel.setColour(Label::backgroundColourId, Colours::red);
    _connectionLabel.setColour(Label::textColourId, Colours::black);
    _connectionLabel.setJustificationType(Justification::centred);
    _connectionLabel.setBounds(200, 15, MainWidth - MainLeft - 200, 20);
    addToLayout(&_connectionLabel, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_connectionLabel);

    //get the button width
    long buttonWidth = (MainWidth - 2 * MainLeft - SpaceBetweenButton * 2) / 3;

    // Load button
    _loadButton.addListener(this);
    _loadButton.setBounds(MainLeft, 60, buttonWidth, 20);
    addToLayout(&_loadButton, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_loadButton);

    // Save button
    _saveButton.addListener(this);
    _saveButton.setBounds(MainLeft + buttonWidth + SpaceBetweenButton, 60, buttonWidth, 20);
    addToLayout(&_saveButton, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_saveButton);

    // Settings button
    _settingsButton.addListener(this);
    _settingsButton.setBounds(MainLeft + buttonWidth * 2 + SpaceBetweenButton * 2, 60, buttonWidth, 20);
    addToLayout(&_settingsButton, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_settingsButton);



    // Command Table
    _commandTable.setModel(&_commandTableModel);
    _commandTable.setBounds(MainLeft, 100, MainWidth - MainLeft * 2, MainHeight - 210);
    addToLayout(&_commandTable, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_commandTable);


    long labelWidth = (MainWidth - MainLeft * 2) / 2;


    // Profile name label
    SetLabelSettings(_profileNameLabel);
    _profileNameLabel.setBounds(MainLeft, MainHeight - 100, labelWidth, 20);
    addToLayout(&_profileNameLabel, anchorMidLeft, anchorMidRight);
    _profileNameLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(_profileNameLabel);

    // Last MIDI command
    _commandLabel.setFont(Font{ 12.f, Font::bold });
    _commandLabel.setEditable(false);
    _commandLabel.setColour(Label::textColourId, Colours::darkgrey);
    _commandLabel.setBounds(MainLeft + labelWidth, MainHeight - 100, labelWidth, 20);
    addToLayout(&_commandLabel, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_commandLabel);

    // Remove row button
    _removeRowButton.addListener(this);
    _removeRowButton.setBounds(MainLeft, MainHeight - 75, MainWidth - MainLeft * 2, 20);
    addToLayout(&_removeRowButton, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_removeRowButton);


    // Rescan MIDI button
    _rescanButton.addListener(this);
    _rescanButton.setBounds(MainLeft, MainHeight - 50, MainWidth - MainLeft * 2, 20);
    addToLayout(&_rescanButton, anchorMidLeft, anchorMidRight);
    addAndMakeVisible(_rescanButton);

    // adding the current status label, used for counting down.
    m_currentStatus.setBounds(MainLeft, MainHeight - 30, MainWidth - MainLeft * 2, 20);
    addToLayout(&m_currentStatus, anchorMidLeft, anchorMidRight);
    m_currentStatus.setJustificationType(Justification::centred);
    SetLabelSettings(m_currentStatus);
    addAndMakeVisible(m_currentStatus);

//	_systemTrayComponent.setIconImage(ImageCache::getFromMemory(BinaryData::MIDI2LR_png, BinaryData::MIDI2LR_pngSize));


    if (m_settingsManager)
    {
        // Try to load a default.xml if the user has not set a profile directory
        if (m_settingsManager->getProfileDirectory().isEmpty())
        {
            File defaultProfile = File::getSpecialLocation(File::currentExecutableFile).getSiblingFile("default.xml");
            std::unique_ptr<XmlElement> elem{ XmlDocument::parse(defaultProfile) };
            if (elem)
            {
                _commandTableModel.buildFromXml(elem.get());
                _commandTable.updateContent();
            }
        }
        else if (profileManager)
        {

            // otherwise use the last profile from the profile directory
            profileManager->switchToProfile(0);
        }
    }
    // turn it on
    activateLayout();



}