/*
  ==============================================================================

    LR_IPC_In.h
    Created: 22 Aug 2015 9:44:03pm
    Author:  Parth, Jaffe

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

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

class LR_IPC_IN: public StreamingSocket,
    public Timer,
    public Thread
{
public:
    static const int LR_IN_PORT;

    static LR_IPC_IN& getInstance();

    // closes the socket
    void shutdown();

    // re-enumerates MIDI OUT devices
    void refreshMIDIOutput();

    // Thread interface
    virtual void run() override;

    // Timer callback
    virtual void timerCallback() override;
private:
    LR_IPC_IN();

    LR_IPC_IN(LR_IPC_IN const&) = delete;
    void operator=(LR_IPC_IN const&) = delete;

    // process a line received from the socket
    void processLine(const String& line);

    std::unordered_map<String, int> parameterMap;
};



#endif  // LR_IPC_IN_H_INCLUDED
