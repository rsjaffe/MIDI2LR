=============
Prerequisites
=============

- Lightroom 6+
- Any USB MIDI controller (eg: Korg nanoKONTROL, etc.)

=====
Setup
=====

1. In Lightroom, add the MIDI2LR plugin by going to File>Plugin-Manager>Add
   and navigating to and selecting the MIDI2LR.lrplugin folder.

2. The MIDI2LR application should automatically launch. If it is closed,
   it can be launched by going to File>Plug-in Extras>Start MIDI2LR

3. Moving any controllers will populate the mapping table and allow you to
   set functions. You can also save/load mappings.

================================
Updating From A Previous Version
================================

1. Close the MIDI2LR application if running

2. In Lightroom, remove the old MIDI2LR plugin by going to File>Plugin-Manager>Remove

3. Add the new MIDI2LR plugin by going to File>Plugin-Manager>Add
   and navigating to and selecting the MIDI2LR.lrplugin folder

=====
Notes
=====

- The commands function in a 'pickup' manner. That is, moving a knob or slider
  on your MIDI controller WILL NOT change the mapped parameter UNTIL the value
  reaches what the value is in Lightroom.
  
  This way, as you switch photos, adjusting a controller will not cause undesirable
  changes

- Setting a profile folder (Settings>Set profile folder) will allow you to use
  buttons to switch between profiles.

- After setting a profile folder, adding new profiles to this folder will require 
  restarting MIDI2LR or going to Settings and choosing the same folder for them to be
  picked up by MIDI2LR.
  
===================================
Problems/Frequently Asked Questions
===================================

First see the FAQ at https://github.com/rsjaffe/MIDI2LR/wiki. If you still have
questions, post them at https://github.com/rsjaffe/MIDI2LR/issues.

=========
Changelog
=========

See https://github.com/rsjaffe/MIDI2LR/wiki/Version-History
