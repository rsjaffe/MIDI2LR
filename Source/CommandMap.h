/*
  ==============================================================================

    CommandMap.h
    Created: 31 Jul 2015 9:16:41pm
    Author:  Parth

  ==============================================================================
*/

#ifndef COMMANDMAP_H_INCLUDED
#define COMMANDMAP_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_map>

struct MIDI_Message
{
    bool isCC;
    int channel;
    union {
        int controller;
        int pitch;
        int data;
    };

    MIDI_Message() : isCC(0),
                     channel(0),
                     data(0)
    {
    }

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

    template <>
    struct hash<String>
    {
        std::size_t operator()(const String& k) const
        {
            return k.hash();
        }
    };
}

class CommandMap
{
public:
    static CommandMap& getInstance();
    void addCommandforMessage(int command, const MIDI_Message &cc);
    int getCommandforMessage(MIDI_Message &cc);
    void removeMessage(const MIDI_Message &cc);
    void clearMap();
    bool messageExistsInMap(const MIDI_Message &cc);

    MIDI_Message& getMessageForCommand(const String &command);
    bool commandHasAssociatedMessage(const String &command);

    void toXMLDocument(File& file);

private:
    CommandMap();

    CommandMap(CommandMap const&)  = delete;
    void operator=(CommandMap const&) = delete;

    std::unordered_map<MIDI_Message, int> _messageMap;
    std::unordered_map<String, MIDI_Message> _commandStringMap;
};


#endif  // COMMANDMAP_H_INCLUDED
