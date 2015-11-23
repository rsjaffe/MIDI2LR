/*
  ==============================================================================

	LR_IPC_In.h
	Created: 22 Aug 2015 9:44:03pm
	Author:  Parth

  ==============================================================================
*/

#ifndef LR_IPC_IN_H_INCLUDED
#define LR_IPC_IN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_map>
#include "CommandMap.h"

class LR_IPC_IN : public StreamingSocket,
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
