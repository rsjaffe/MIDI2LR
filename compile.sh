#!/bin/bash
if ! hash luac 2>/dev/null
then
	mkdir -p Source/LRPlugin/MIDI2LR.lrplugin/compiled
	pushd Source/LRPlugin/MIDI2LR.lrplugin || exit
	for filename in *.lua; do luac -o "compiled/${filename}" "${filename}"; done
	popd
fi
/usr/bin/xcodebuild -configuration Release -project Builds/MacOSX/MIDI2LR.xcodeproj
/usr/bin/xcodebuild -configuration Debug -project Builds/MacOSX/MIDI2LR.xcodeproj