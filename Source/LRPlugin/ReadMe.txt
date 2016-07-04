=============
Prerequisites
=============

- Lightroom 6+ or CC
- Windowws 7+ or OSX 10.9+
- A USB MIDI controller (eg: Korg nanoKONTROL, etc.)

=====
Setup
=====

1. Save the MIDI2LR.lrplugin folder and its contents. Make sure you don't need
   administrator privileges to write to that folder—the plugin may crash if it
   is running in a restricted folder.

2. In Lightroom, add the MIDI2LR plugin by going to File→Plugin-Manager→Add
   and navigating to and selecting the MIDI2LR.lrplugin folder.
   
3. If using OS X, to avoid the "unidentified developer" warning, right-click on the
   .lrplugin (whose location you can find in the LR Plugin Manager if you don't know 
   it) and "Show Package Contents" then you will see there is a Mac executable in 
   there called MIDI2LR. Right click on this and then Open it, agree to the OSX 
   security question. Only needs to be done once.

   Also, to avoid the "100% CPU utilization" bug, please shut down MIDI2LR app prior
   to shutting down Lightroom. Debugging assistance welcome--I haven't found the
   cause yet, and I don't have the setup to test this program in OS X.

4. The MIDI2LR application should automatically launch. If it is closed,
   it can be launched by going to File→Plug-in Extras→Start MIDI2LR

5. Moving any controllers will populate the mapping table and allow you to
   set functions. You can also save/load mappings.

6. Information on all the settings and options are in the wiki: 
   https://github.com/rsjaffe/MIDI2LR/wiki.

================================
Updating From A Previous Version
================================

1. Close the MIDI2LR application if running

2. In Lightroom, remove the old MIDI2LR plugin by going to File→Plugin-Manager→Remove

3. Add the new MIDI2LR plugin by going to File→Plugin-Manager→Add
   and navigating to and selecting the MIDI2LR.lrplugin folder

=====
Notes
=====

- The commands function in a 'pickup' manner. That is, moving a knob or slider
  on your MIDI controller WILL NOT change the mapped parameter UNTIL the value
  reaches what the value is in Lightroom.
  
  This way, as you switch photos, adjusting a controller will not cause undesirable
  changes

- Setting a profile folder (Settings→Set profile folder) will allow you to use
  buttons to switch between profiles.

- After setting a profile folder, adding new profiles to this folder will require 
  restarting MIDI2LR or going to Settings and choosing the same folder for them to be
  picked up by MIDI2LR.
  
===================================
Problems/Frequently Asked Questions
===================================

First see the FAQ at https://github.com/rsjaffe/MIDI2LR/wiki#faqknown-issues. If you
still have questions, post them at https://groups.google.com/forum/#!forum/midi2lr.

=========
Changelog
=========

See https://github.com/rsjaffe/MIDI2LR/wiki/Version-History

=======
Support
=======
Donations Welcome: https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=YWHT4JMA42RXN
