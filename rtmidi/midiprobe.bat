rem command line compile for Windows
rem see https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line
rem need to run developer command prompt to use this
cl /EHsc /O2 /D__WINDOWS_MM__ /DNDEBUG /DWIN32 /D_CONSOLE midiprobe.cpp rtmidi.cpp odbc32.lib odbccp32.lib winmm.lib