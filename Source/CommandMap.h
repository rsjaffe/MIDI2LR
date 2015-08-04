/*
  ==============================================================================

    CommandMap.h
    Created: 31 Jul 2015 9:16:41pm
    Author:  Parth

  ==============================================================================
*/

#ifndef COMMANDMAP_H_INCLUDED
#define COMMANDMAP_H_INCLUDED

#include <unordered_map>
#include <vector>

#include "LRCommands.h"

struct MIDI_CC
{
    int channel;
    int controller;

    MIDI_CC(int ch, int cont) : channel(ch),
                                controller(cont)
    {}

    bool operator==(const MIDI_CC &other) const
    {
        return (channel == other.channel && controller == other.controller);
    }
};

// hash function for MIDI_CC
namespace std {
    template <>
    struct hash<MIDI_CC>
    {
        std::size_t operator()(const MIDI_CC& k) const
        {
            return (std::hash<int>()(k.channel) ^ (std::hash<int>()(k.controller) << 1));
        }
    };
}

class CommandMap
{
public:
    static CommandMap& getInstance();
    void addCommandforCC(int command, MIDI_CC &cc);
    int getCommandforCC(MIDI_CC &cc);
    void removeCC(MIDI_CC &cc);
    void emptyMap();
    bool controllerExistsInMap(MIDI_CC &cc);

    void toXMLDocument(File& file);

private:
    CommandMap();

    CommandMap(CommandMap const&)  = delete;
    void operator=(CommandMap const&) = delete;

    std::unordered_map<MIDI_CC, int> _controllerMap;
};



#endif  // COMMANDMAP_H_INCLUDED
