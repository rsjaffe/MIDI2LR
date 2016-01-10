/*
  ==============================================================================
This file is part of MIDI2LR. Copyright 2016 by Jewest

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
#ifndef LOGGER_H
#define LOGGER_H

using namespace std;

// Debugging tool for messages
#include <string>
#include <iostream>     // std::cout
#include <fstream>      // std::fstream

class Logger
{
    public:
		Logger();
	    virtual ~Logger();
		void AddToLog(string &data);
	protected:

    bool IsOpen(void);
	void Open(void);
	void Close(void);

	private:
		std::fstream m_outStream;


};

void AddToLog(string newInfo);

#endif
