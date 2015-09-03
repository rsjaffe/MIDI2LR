/*
  ==============================================================================

    CommandMenu.h
    Created: 3 Aug 2015 3:27:33pm
    Author:  Parth

  ==============================================================================
*/

#ifndef COMMANDMENU_H_INCLUDED
#define COMMANDMENU_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"

class CommandMenu : public TextButton,
                    protected ButtonListener
{
public:
    CommandMenu(MIDI_Message& msg);
    void setMsg(MIDI_Message& msg);
    void setSelectedItem(int idx);

    // ButtonListener interface
    virtual void buttonClicked(Button* button) override;
    
private:
    MIDI_Message _msg;
    int _selectedItem;
};




#endif  // COMMANDMENU_H_INCLUDED
