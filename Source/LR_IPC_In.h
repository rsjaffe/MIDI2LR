#pragma once
/*
  ==============================================================================

	LR_IPC_In.h

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
#ifndef LR_IPC_IN_H_INCLUDED
#define LR_IPC_IN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_map>
#include "CommandMap.h"
#include "ProfileManager.h"
#include "MIDISender.h"

/**********************************************************************************************//**
* @class   LR_IPC_IN
*
* @brief   A lr ipc in.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

class LR_IPC_IN : public StreamingSocket,
	public Timer,
	public Thread
{
public:	
	LR_IPC_IN();
	virtual ~LR_IPC_IN() {};
	// closes the socket
	void shutdown();

	// re-enumerates MIDI OUT devices
	void refreshMIDIOutput();

	// Thread interface
	virtual void run() override;

	// Timer callback
	virtual void timerCallback() override;
	void Init(std::shared_ptr<CommandMap> mapCommand, std::shared_ptr<ProfileManager> profileManager, std::shared_ptr<MIDISender> midiSender);
private:
	// process a line received from the socket
	void processLine(const String& line);
    std::shared_ptr<CommandMap> m_commandMap;
    std::shared_ptr<ProfileManager> m_profileManager;
    std::shared_ptr<MIDISender> m_midiSender;
	std::unordered_map<String, int> parameterMap;
};



#endif  // LR_IPC_IN_H_INCLUDED
