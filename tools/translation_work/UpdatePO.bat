cd "POE plugin"
for %%v in (*.po) do "C:\Program Files\Poedit\GettextTools\bin\msgmerge.exe" -vU --no-wrap %%v MIDI2LRplugin.pot
cd "..\POE installer"
for %%v in (*.po) do "C:\Program Files\Poedit\GettextTools\bin\msgmerge.exe" -vU --no-wrap %%v MIDI2LRinstaller.pot
cd "..\POE application"
for %%v in (*.po) do "C:\Program Files\Poedit\GettextTools\bin\msgmerge.exe" -vU --no-wrap %%v MIDI2LRapplication.pot
cd ..