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

struct MIDI_Message
{
    bool isCC;
    int channel;
    union {
        int controller;
        int pitch;
        int data;
    };

    MIDI_Message(int ch, int dat, bool iscc) : channel(ch),
                                               data(dat),
                                               isCC(iscc)
    {}

    bool operator==(const MIDI_Message &other) const
    {
        return (isCC == other.isCC && channel == other.channel && data == other.data);
    }
};

// hash function for MIDI_CC
namespace std {
    template <>
    struct hash<MIDI_Message>
    {
        std::size_t operator()(const MIDI_Message& k) const
        {
            return (std::hash<bool>()(k.isCC) ^ std::hash<int>()(k.channel) ^ (std::hash<int>()(k.data) << 1));
        }
    };
}

class CommandMap
{
public:
    static CommandMap& getInstance();
    void addCommandforMessage(int command, MIDI_Message &cc);
    int getCommandforMessage(MIDI_Message &cc);
    void removeMessage(MIDI_Message &cc);
    void clearMap();
    bool messageExistsInMap(MIDI_Message &cc);

    void toXMLDocument(File& file);

private:
    CommandMap();

    CommandMap(CommandMap const&)  = delete;
    void operator=(CommandMap const&) = delete;

    std::unordered_map<MIDI_Message, int> _messageMap;
};


#endif  // COMMANDMAP_H_INCLUDED
