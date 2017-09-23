#!/bin/sh
g++ -Wall -D__MACOSX_CORE__ -o midiprobe midiprobe.cpp RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation