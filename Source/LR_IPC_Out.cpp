/*
  ==============================================================================

    LR_IPC_OUT.cpp

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
#include "LR_IPC_OUT.h"
#include "CommandMap.h"
#include "LRCommands.h"

constexpr auto LrOutPort = 58763;

const std::unordered_map<String, KeyPress> LR_IPC_OUT::KPMappings = {
#ifdef _WIN32
{ "KPImportImages", KeyPress::createFromDescription("ctrl + shift + i") },
{ "KPExportImages", KeyPress::createFromDescription("control + shift + e") },
{ "KPIncreaseGridSize", KeyPress::createFromDescription("+") },
{ "KPDecreaseGridSize", KeyPress::createFromDescription("-") },
{ "KPShowExtras", KeyPress::createFromDescription("ctrl + shift + h") },
{ "KPShowBadges", KeyPress::createFromDescription("ctrl + alt + shift + h") },
{ "KPCycleViewStyle", KeyPress::createFromDescription("j") },
{ "ShoInfoLoupe", KeyPress::createFromDescription("ctrl + i") },
{ "ShoCycleInfoStyle", KeyPress::createFromDescription("i") },
{ "KPSwapImages", KeyPress::createFromDescription("cursor down") },
{ "KPHideToolbar", KeyPress::createFromDescription("t") },
{ "KPHidePanels", KeyPress::createFromDescription("tab") },
{ "KPHideAllPanels", KeyPress::createFromDescription("shift + tab") },
{ "KPSelectAll", KeyPress::createFromDescription("ctrl + a") },
{ "KPSelectNone", KeyPress::createFromDescription("ctrl + d") },
{ "KPSelectActive", KeyPress::createFromDescription("ctrl + shift + d") },
{ "KPDeselectActive", KeyPress::createFromDescription("shift + d") },
{ "KPSelectFlagged", KeyPress::createFromDescription("ctrl + alt + a") },
{ "KPDeselectUnflagged", KeyPress::createFromDescription("ctrl + alt + shift + d") },
{ "KPPrevSelected", KeyPress::createFromDescription("ctrl + cursor left") },
{ "KPNextSelected", KeyPress::createFromDescription("ctrl + cursor right") },
{ "KPGoNormalMode", KeyPress::createFromDescription("ctrl + alt + f") },
{ "KPFullScrHidePanles", KeyPress::createFromDescription("ctrl + shift + f") },
{ "KPFullScrPreview", KeyPress::createFromDescription("f") },
{ "KPCycleLightsOut", KeyPress::createFromDescription("l") },
{ "KPGoLightsOutDim", KeyPress::createFromDescription("ctrl + shift + l") },
{ "KPMergeHDR", KeyPress::createFromDescription("ctrl + h") },
{ "KPMergeHDRnoDlg", KeyPress::createFromDescription("ctrl + shift + h") },
{ "KPMergePano", KeyPress::createFromDescription("ctrl + m") },
{ "KPMergePanonoDlg", KeyPress::createFromDescription("ctrl + shift + m") },
{ "KPEditInPhotoshop", KeyPress::createFromDescription("ctrl + e") },
{ "KPEditInOther", KeyPress::createFromDescription("ctrl + alt + e") },
{ "KPAddToQuickCollection", KeyPress::createFromDescription("b") },
{ "KPAddToQuickCollectionAndNext", KeyPress::createFromDescription("shift + b") },
{ "KPShowQuickCollection", KeyPress::createFromDescription("ctrl + b") },
{ "KPSaveQuickCollection", KeyPress::createFromDescription("ctrl + alt + b") },
{ "KPClearQuickCollection", KeyPress::createFromDescription("ctrl + shift + b") },
{ "KPTargetQuickCollection", KeyPress::createFromDescription("ctrl + alt + shift + b") },
{ "KPGroupIntoStack", KeyPress::createFromDescription("ctrl + g") },
{ "KPUnstack", KeyPress::createFromDescription("ctrl + shift + g") },
{ "KPExpandStack", KeyPress::createFromDescription("s") },
{ "KPToTopStack", KeyPress::createFromDescription("shift + s") },
{ "KPUpInStack", KeyPress::createFromDescription("{") },
{ "KPDnInStack", KeyPress::createFromDescription("}") },
{ "KPRotateLeft", KeyPress::createFromDescription("ctrl + [") },
{ "KPRotateRight", KeyPress::createFromDescription("ctrl + ]") },
{ "KPDelete", KeyPress::createFromDescription("delete") },
{ "KPDeleteRej", KeyPress::createFromDescription("ctrl + delete") },
{ "KPRemoveFromCat", KeyPress::createFromDescription("alt + delete") },
{ "KPTrash", KeyPress::createFromDescription("ctrl + alt + shift + delete") },
{ "KPAutoSync", KeyPress::createFromDescription("ctrl + alt + shift + a") },
{ "KPPasteFromPrevious", KeyPress::createFromDescription("ctrl + alt + v") },
{ "KPMatchExposures", KeyPress::createFromDescription("ctrl + alt + shift + m") },
{ "KPBeforeAfter", KeyPress::createFromDescription("\\") },
{ "KPAutoTone", KeyPress::createFromDescription("ctrl + u") },
{ "KPClipping", KeyPress::createFromDescription("j") },
{ "KPIncreaseSize", KeyPress::createFromDescription("]") },
{ "KPDecreaseSize", KeyPress::createFromDescription("[") },
{ "KPIncreaseFeather", KeyPress::createFromDescription("}") },
{ "KPDecreaseFeather", KeyPress::createFromDescription("{") },
{ "KPRotateCropAspect", KeyPress::createFromDescription("x") },
#else
{ "KPImportImages", KeyPress::createFromDescription("command + shift + i") },
{ "KPExportImages", KeyPress::createFromDescription("command + shift + e") },
{ "KPIncreaseGridSize", KeyPress::createFromDescription("+") },
{ "KPDecreaseGridSize", KeyPress::createFromDescription("-") },
{ "KPShowExtras", KeyPress::createFromDescription("command + shift + h") },
{ "KPShowBadges", KeyPress::createFromDescription("command + option + shift + h") },
{ "KPCycleViewStyle", KeyPress::createFromDescription("j") },
{ "ShoInfoLoupe", KeyPress::createFromDescription("command + i") },
{ "ShoCycleInfoStyle", KeyPress::createFromDescription("i") },
{ "KPSwapImages", KeyPress::createFromDescription("cursor down") },
{ "KPHideToolbar", KeyPress::createFromDescription("t") },
{ "KPHidePanels", KeyPress::createFromDescription("tab") },
{ "KPHideAllPanels", KeyPress::createFromDescription("shift + tab") },
{ "KPSelectAll", KeyPress::createFromDescription("command + a") },
{ "KPSelectNone", KeyPress::createFromDescription("command + d") },
{ "KPSelectActive", KeyPress::createFromDescription("command + shift + d") },
{ "KPDeselectActive", KeyPress::createFromDescription("shift + d") },
{ "KPSelectFlagged", KeyPress::createFromDescription("command + option + a") },
{ "KPDeselectUnflagged", KeyPress::createFromDescription("command + option + shift + d") },
{ "KPPrevSelected", KeyPress::createFromDescription("command + cursor left") },
{ "KPNextSelected", KeyPress::createFromDescription("command + cursor right") },
{ "KPGoNormalMode", KeyPress::createFromDescription("command + option + f") },
{ "KPFullScrHidePanles", KeyPress::createFromDescription("command + shift + f") },
{ "KPFullScrPreview", KeyPress::createFromDescription("f") },
{ "KPCycleLightsOut", KeyPress::createFromDescription("l") },
{ "KPGoLightsOutDim", KeyPress::createFromDescription("command + shift + l") },
{ "KPMergeHDR", KeyPress::createFromDescription("command + h") },
{ "KPMergeHDRnoDlg", KeyPress::createFromDescription("command + shift + h") },
{ "KPMergePano", KeyPress::createFromDescription("command + m") },
{ "KPMergePanonoDlg", KeyPress::createFromDescription("command + shift + m") },
{ "KPEditInPhotoshop", KeyPress::createFromDescription("command + e") },
{ "KPEditInOther", KeyPress::createFromDescription("command + option + e") },
{ "KPAddToQuickCollection", KeyPress::createFromDescription("b") },
{ "KPAddToQuickCollectionAndNext", KeyPress::createFromDescription("shift + b") },
{ "KPShowQuickCollection", KeyPress::createFromDescription("command + b") },
{ "KPSaveQuickCollection", KeyPress::createFromDescription("command + option + b") },
{ "KPClearQuickCollection", KeyPress::createFromDescription("command + shift + b") },
{ "KPTargetQuickCollection", KeyPress::createFromDescription("command + option + shift + b") },
{ "KPGroupIntoStack", KeyPress::createFromDescription("command + g") },
{ "KPUnstack", KeyPress::createFromDescription("command + shift + g") },
{ "KPExpandStack", KeyPress::createFromDescription("s") },
{ "KPToTopStack", KeyPress::createFromDescription("shift + s") },
{ "KPUpInStack", KeyPress::createFromDescription("{") },
{ "KPDnInStack", KeyPress::createFromDescription("}") },
{ "KPRotateLeft", KeyPress::createFromDescription("command + [") },
{ "KPRotateRight", KeyPress::createFromDescription("command + ]") },
{ "KPDelete", KeyPress::createFromDescription("delete") },
{ "KPDeleteRej", KeyPress::createFromDescription("command + delete") },
{ "KPRemoveFromCat", KeyPress::createFromDescription("option + delete") },
{ "KPTrash", KeyPress::createFromDescription("command + option + shift + delete") },
{ "KPAutoSync", KeyPress::createFromDescription("command + option + shift + a") },
{ "KPPasteFromPrevious", KeyPress::createFromDescription("command + option + v") },
{ "KPMatchExposures", KeyPress::createFromDescription("command + option + shift + m") },
{ "KPBeforeAfter", KeyPress::createFromDescription("\\") },
{ "KPAutoTone", KeyPress::createFromDescription("command + u") },
{ "KPClipping", KeyPress::createFromDescription("j") },
{ "KPIncreaseSize", KeyPress::createFromDescription("]") },
{ "KPDecreaseSize", KeyPress::createFromDescription("[") },
{ "KPIncreaseFeather", KeyPress::createFromDescription("}") },
{ "KPDecreaseFeather", KeyPress::createFromDescription("{") },
{ "KPRotateCropAspect", KeyPress::createFromDescription("x") },
#endif
};

/**********************************************************************************************//**
 * @fn  LR_IPC_OUT::LR_IPC_OUT()
 *
 * @brief   Default constructor.
 *
 *
 * @date    3/20/2016
 **************************************************************************************************/

LR_IPC_OUT::LR_IPC_OUT(): InterprocessConnection(), m_SendKeys()
{


}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::shutdown()
 *
 * @brief   Shuts down this object and frees any resources it is using.
 *
 *
 * @date    3/20/2016
 **************************************************************************************************/

void LR_IPC_OUT::shutdown()
{
    stopTimer(),
        disconnect();
    m_commandMap.reset();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::timerCallback()
 *
 * @brief   Callback, called when the timer.
 *
 *
 * @date    3/20/2016
 **************************************************************************************************/

void LR_IPC_OUT::timerCallback()
{
    if (!isConnected())
        connectToSocket("127.0.0.1", LrOutPort, 100);
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::Init(std::shared_ptr<CommandMap>& mapCommand, std::shared_ptr<MIDIProcessor>& midiProcessor)
 *
 * @brief   S.
 *
 * @date    3/20/2016
 *
 * @param [in,out]  mapCommand      If non-null, the map command.
 * @param [in,out]  midiProcessor   If non-null, the MIDI processor.
 **************************************************************************************************/

void LR_IPC_OUT::Init(std::shared_ptr<CommandMap>& mapCommand, std::shared_ptr<MIDIProcessor>& midiProcessor)
{
    //copy the pointer
    m_commandMap = mapCommand;

    if (midiProcessor)
    {
        midiProcessor->addMIDICommandListener(this);
    }


    //start the timer
    startTimer(1000);


}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::addListener(LRConnectionListener *listener)
 *
 * @brief   Adds a listener.
 *
 *
 * @date    3/20/2016
 *
 * @param [in,out]  listener    If non-null, the listener.
 **************************************************************************************************/

void LR_IPC_OUT::addListener(LRConnectionListener *listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::connectionMade()
 *
 * @brief   Connection made.
 *
 *
 * @date    3/20/2016
 **************************************************************************************************/

void LR_IPC_OUT::connectionMade()
{
    for (auto listener : _listeners)
        listener->connected();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::connectionLost()
 *
 * @brief   Connection lost.
 *
 *
 * @date    3/20/2016
 **************************************************************************************************/

void LR_IPC_OUT::connectionLost()
{
    for (auto listener : _listeners)
        listener->disconnected();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::messageReceived(const MemoryBlock& )
 *
 * @brief   Message received.
 *
 * @date    3/20/2016
 *
 * @param   parameter1  The unused argument (msg)
 **************************************************************************************************/

void LR_IPC_OUT::messageReceived(const MemoryBlock& /*msg*/)
{

}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::sendCommand(const String &command) const
 *
 * @brief   Sends a command.
 *
 * @date    3/20/2016
 *
 * @param   command The command.
 **************************************************************************************************/

void LR_IPC_OUT::sendCommand(const String &command) const
{
    //check if there is a connection
    if (isConnected())
    {
        getSocket()->write(command.getCharPointer(), command.length());
    }
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleKPCommand() const
 *
 * @brief   Handles KP (key press) commands.
 *
 * @date    3/20/2016
 **************************************************************************************************/

void LR_IPC_OUT::handleKPCommand() const
{
    if (_valueToSend == 127)
        m_SendKeys.SendKeyDownUp(KPMappings.at(_commandToSend));
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleAsyncUpdate()
 *
 * @brief   Handles the asynchronous update.
 *
 *
 * @date    3/20/2016
 **************************************************************************************************/

void LR_IPC_OUT::handleAsyncUpdate()
{
    //check if there is a connection
    if (isConnected())
    {
        if (_commandToSend.startsWith("KP"))
            handleKPCommand();
        else
        {
            auto command(_commandToSend + String::formatted(" %d\n", _valueToSend));
            sendCommand(command);
        }
    }
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
 *
 * @brief   Handles MIDI CC messages.
 *
 *
 * @date    3/20/2016
 *
 * @param   midiChannel The MIDI channel.
 * @param   controller  The controller.
 * @param   value       The value.
 **************************************************************************************************/

void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
{
    MIDI_Message cc{ midiChannel, controller, true };

    if (m_commandMap)
    {
        if (!m_commandMap->messageExistsInMap(cc) ||
            m_commandMap->getCommandforMessage(cc) == "Unmapped" ||
            find(LRCommandList::NextPrevProfile.begin(),
            LRCommandList::NextPrevProfile.end(),
            m_commandMap->getCommandforMessage(cc)) != LRCommandList::NextPrevProfile.end())
            return;

        _commandToSend = m_commandMap->getCommandforMessage(cc);
        _valueToSend = value;
        handleAsyncUpdate();
    }
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
 *
 * @brief   Handles MIDI note messages.
 *
 *
 * @date    3/20/2016
 *
 * @param   midiChannel The MIDI channel.
 * @param   note        The note.
 **************************************************************************************************/

void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
{
    MIDI_Message note_msg{ midiChannel, note, false };

    if (m_commandMap)
    {

        if (!m_commandMap->messageExistsInMap(note_msg) ||
            m_commandMap->getCommandforMessage(note_msg) == "Unmapped" ||
            find(LRCommandList::NextPrevProfile.begin(),
            LRCommandList::NextPrevProfile.end(),
            m_commandMap->getCommandforMessage(note_msg)) != LRCommandList::NextPrevProfile.end())
            return;

        _commandToSend = m_commandMap->getCommandforMessage(note_msg);
        _valueToSend = 127;
        handleAsyncUpdate();
    }
}
