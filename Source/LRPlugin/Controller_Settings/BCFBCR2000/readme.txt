BCF2000.txt and BCR2000.txt are settings for the BCF2000 and BCR2000 controllers. Following are instructions for getting ready with the BCF2000:

Download and install plugin into LR per usual (see ReadMe.txt) in the MIDI2LR folder
In the MIDI2LR popup
	Settings	
Turn off enable pickup mode (for BCF2000)
		Choose Profile Folder (*1)
LR plugin manager
	Make sure MIDI2LR is enabled
LR plugin extras
	Make sure server is started (MIDI2LR popup should be green and say “Connected to LR”)
	Options -> Profiles
		Type in the profile names (located in Profile Folder *1)
		The names will be predictive text once you start typing

Preparing the BCF2000

If it is used need to factory reset it:
https://pusherlabs.freshdesk.com/support/solutions/articles/161619-resetting-the-behringer-bcf-2000-bcr-2000-to-factory-defaults

Reprogram the BCF2000 using BCManager (*2)
http://mountainutilities.eu/bcmanager

What this (*2) does:
Remaps BCF2000 to “toggle off” mode. This allows top buttons to send LR commands with a single button press rather than be in their default toggle on/off, which is normally used for muting and unmuting music tracks. Trust me you need this. It also remaps the BCF2000 buttons Store, Learn, Edit and Exit to generic buttons that can then be used by MIDI2LR per usual. Also, you’ll want this. Trust me. 

BCManager uses a file within the “Controller Settings” folder in the MIDI2LR folder: preset2.bcf. (*3)  It’s the file that does the remapping. When BCF2000 is on, it remaps PRESET2 on the BCF2000. This is the only preset that you’ll need on the BCF2000 and will not need to ever touch the PRESET buttons again. LED on the BCF needs to show P-2. 

Turn on BCF2000 and connect to your computer
Open BCManager
Press button “Show list of B controls”
	Your BCF2000 should be listed and noted as “online”
File -> Open -> preset2.bcf (*3)
MIDI -> Send all data
Exit from BCManager

You’re done and ready to go!

Instructions courtesy of kfrazack