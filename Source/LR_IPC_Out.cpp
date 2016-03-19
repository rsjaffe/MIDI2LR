/*
  ==============================================================================

    LR_IPC_OUT.cpp
    Created: 2 Aug 2015 12:27:47am
    Author:  Parth, Jaffe

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
#include "Tools.h"


// define the port used to 
#define LR_OUT_PORT 58763 

const std::unordered_map<String, KeyPress> LR_IPC_OUT::KPMappings = {
#ifdef _WIN32
{ "KPImportImages", KeyPress::createFromDescription("ctrl + shift + i") },
{ "KPExportImages", KeyPress::createFromDescription("control + shift + e") },
{ "KPIncreaseGridSize", KeyPress::createFromDescription("=") },
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
{ "KPUpInStack", KeyPress::createFromDescription("shift + [") },
{ "KPDnInStack", KeyPress::createFromDescription("shift + ]") },
{ "KPRotateLeft", KeyPress::createFromDescription("ctrl + [") },
{ "KPRotateRight", KeyPress::createFromDescription("ctrl + ]") },
{ "KPDelete", KeyPress::createFromDescription("delete") },
{ "KPDeleteRej", KeyPress::createFromDescription("ctrl + delete") },
{ "KPRemoveFromCat", KeyPress::createFromDescription("alt + delete") },
{ "KPTrash", KeyPress::createFromDescription("ctrl + alt + shift + delete") },
{ "KPPasteFromPrevious", KeyPress::createFromDescription("ctrl + alt + v") },
{ "KPMatchExposures", KeyPress::createFromDescription("ctrl + alt + shift + m") },
{ "KPBeforeAfter", KeyPress::createFromDescription("\\") },
{ "KPAutoTone", KeyPress::createFromDescription("ctrl + u") },
{ "KPClipping", KeyPress::createFromDescription("j") },
{ "KPIncreaseSize", KeyPress::createFromDescription("]") },
{ "KPDecreaseSize", KeyPress::createFromDescription("[") },
{ "KPIncreaseFeather", KeyPress::createFromDescription("shift + ]") },
{ "KPDecreaseFeather", KeyPress::createFromDescription("shift + [") },
#else
{ "KPImportImages", KeyPress::createFromDescription("command + shift + i") },
{ "KPExportImages", KeyPress::createFromDescription("command + shift + e") },
{ "KPIncreaseGridSize", KeyPress::createFromDescription("=") },
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
{ "KPUpInStack", KeyPress::createFromDescription("shift + [") },
{ "KPDnInStack", KeyPress::createFromDescription("shift + ]") },
{ "KPRotateLeft", KeyPress::createFromDescription("command + [") },
{ "KPRotateRight", KeyPress::createFromDescription("command + ]") },
{ "KPDelete", KeyPress::createFromDescription("delete") },
{ "KPDeleteRej", KeyPress::createFromDescription("command + delete") },
{ "KPRemoveFromCat", KeyPress::createFromDescription("option + delete") },
{ "KPTrash", KeyPress::createFromDescription("command + option + shift + delete") },
{ "KPPasteFromPrevious", KeyPress::createFromDescription("command + option + v") },
{ "KPMatchExposures", KeyPress::createFromDescription("command + option + shift + m") },
{ "KPBeforeAfter", KeyPress::createFromDescription("\\") },
{ "KPAutoTone", KeyPress::createFromDescription("command + u") },
{ "KPClipping", KeyPress::createFromDescription("j") },
{ "KPIncreaseSize", KeyPress::createFromDescription("]") },
{ "KPDecreaseSize", KeyPress::createFromDescription("[") },
{ "KPIncreaseFeather", KeyPress::createFromDescription("shift + ]") },
{ "KPDecreaseFeather", KeyPress::createFromDescription("shift + [") },
#endif
};

LR_IPC_OUT::LR_IPC_OUT(): InterprocessConnection(), m_SendKeys()
{


}

void LR_IPC_OUT::shutdown()
{
    stopTimer(),
        disconnect();
}

void LR_IPC_OUT::timerCallback()
{
    if (!isConnected())
        connectToSocket("127.0.0.1", LR_OUT_PORT, 100);
}

void LR_IPC_OUT::Init(CommandMap * mapCommand, MIDIProcessor *midiProcessor)
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

void LR_IPC_OUT::addListener(LRConnectionListener *listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}

void LR_IPC_OUT::connectionMade()
{
    for (auto listener : _listeners)
        listener->connected();
}

void LR_IPC_OUT::connectionLost()
{
    for (auto listener : _listeners)
        listener->disconnected();
}

void LR_IPC_OUT::messageReceived(const MemoryBlock& UNUSED_ARG(msg))
{

}

void LR_IPC_OUT::sendCommand(const String &command)
{
    //check if there is a connection
    if (isConnected())
    {
        getSocket()->write(command.getCharPointer(), command.length());
    }
}

void LR_IPC_OUT::handleKPCommand()
{
    if (_valueToSend == 127)
        m_SendKeys.handleShortCutKeyDownUp(KPMappings.at(_commandToSend));
}

void LR_IPC_OUT::handleAsyncUpdate()
{
    //check if there is a connection
    if (isConnected())
    {
        if (_commandToSend.startsWith("KP"))
            handleKPCommand();
        else
        {
            String command(_commandToSend + String::formatted(" %d\n", _valueToSend));
            sendCommand(command);
        }
    }
}

void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
{
    MIDI_Message cc(midiChannel, controller, true);

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

void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
{
    MIDI_Message note_msg(midiChannel, note, false);

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

