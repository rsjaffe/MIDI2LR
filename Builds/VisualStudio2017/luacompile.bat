
REM To compile lua files, obtain/build version 5.1.4 of luac.exe and place in parent of Source directory.
SETLOCAL ENABLEEXTENSIONS
IF EXIST ..\..\luac.exe (
IF NOT EXIST Source\LRPlugin\MIDI2LR.lrplugin\compiled\ mkdir Source\LRPlugin\MIDI2LR.lrplugin\compiled\
PUSHD ..\..\Source\LRPlugin\MIDI2LR.lrplugin\
FOR %%a IN (*.lua) DO ..\..\..\luac -o compiled/%%a %%a
POPD 
) ELSE (
ECHO luac.exe not found, lua files not compiled.
ECHO
)
