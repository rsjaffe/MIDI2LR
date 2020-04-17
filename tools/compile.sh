#!/bin/bash
if [[ $1 == 'sign' ]]; then
  /usr/bin/xcodebuild -configuration Release -project Builds/MacOSX/MIDI2LR.xcodeproj
  /usr/bin/xcodebuild -configuration Debug -project Builds/MacOSX/MIDI2LR.xcodeproj
else
  /usr/bin/xcodebuild -configuration Release -project Builds/MacOSX/MIDI2LR.xcodeproj CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO
  /usr/bin/xcodebuild -configuration Debug -project Builds/MacOSX/MIDI2LR.xcodeproj CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO
fi