REM For compiling C++ source, need to run this in the VS developer command prompt. 
REM For details, see https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs
REM To compile lua files, obtain/build version 5.1.4 of luac.exe and place in this directory.
REM This is the Windows version of the batch file. For Mac or Linux will need appropriate batch file.
SETLOCAL ENABLEEXTENSIONS
IF EXIST luac.exe (
PUSHD Source\LRPlugin\MIDI2LR.lrplugin\
FOR %%a IN (*.lua) DO ..\..\..\luac -o compiled/%%a %%a
POPD 
) ELSE (
ECHO luac.exe not found, lua files not compiled.
ECHO
)

IF DEFINED VSINSTALLDIR (
msbuild Builds\VisualStudio2017\MIDI2LR_App.vcxproj /p:configuration=Release /p:platform=x64
) ELSE (
echo Not in Visual Studio developer command prompt, build project will not be called.
)