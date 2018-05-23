#!/bin/bash
hash luac 2>/dev/null
if [ $? -eq 0 ]
mkdir -p Source/LRPlugin/MIDI2LR.lrplugin/compiled
pushd Source/LRPlugin/MIDI2LR.lrplugin
for filename in ./*.lua do luac -o "compiled/${filename}" "${filename}"; done
popd
fi
/usr/bin/xcodebuild -configuration Release -project Builds/MacOSX/MIDI2LR.xcodeproj