# MIDI2LR

**MIDI2LR** is an application that interfaces MIDI controllers with Lightroom 6/CC. It processes MIDI input into develop parameter updates and photo actions, and sends MIDI output when parameters are changed for motorized feedback (on controllers that have motorized faders). A listing of available LightRoom commands is in the [Wiki](https://github.com/rsjaffe/MIDI2LR/wiki). Assistance on the code and design is welcome.

| Screenshot | Further information |
| -----------| -------------------- |
| <img src="http://rsjaffe.github.io/MIDI2LR/images/app070.png" /> | **Donations Welcome**: [Donate to development costs](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=YWHT4JMA42RXN). **Supported platforms**: Mac OSX 10.9+, Windows 7/8/10. **Building**: See the BUILD.txt file. **Project Page**: [Project Page](http://rsjaffe.github.io/MIDI2LR). **Wiki and Instruction Manual**: [Wiki :book:](https://github.com/rsjaffe/MIDI2LR/wiki). |

## Translation assistance
I'm adding translation of menu items for the Lightroom plugin (but not the app).
For those interested in helping, the files to edit are named TranslatedStrings_??.txt 
where ?? is replaced with the language code.

To provide a translation for the project, edit the txt file in the language you want 
to translate. The only languages supported by Lightroom are German (de), English (en), 
Spanish (es), French (fr), Italian (it), Japanese (ja), Korean (ko), Dutch (nl), 
Portuguese (pt), Swedish (sv), Chinese simplified (zh_cn) and Chinese traditional (zh_tw).

The following text editors are recommended for editing these files:

In Mac OS X simply use TextEdit and make sure you save the file type as “UTF-8” 
(rather than UTF-16 or UCS-2, for instance).

In Windows use Notepad, and be sure to save the file as type “UTF-8.”

Each line in the file is formatted like "$$$/MIDI2LR/Stuff/name=The text to translate".
Leave everything to the left of the = sign alone, and replace the text to the right of 
the = sign with the translation. If you see a ^ sign followed by a number in the string, 
leave that alone as well--that gets replaced with a calculated value in the program. No 
newline characters or comments are allowed. The strings in this file must all be enclosed 
with double quotes. If you're wondering what a translated file looks like, compare 
TranslatedStrings_en.txt with TranslatedStrings_de.txt.

Send any translations to me at rsjaffe@gmail.com. Thanks for your assistance!




