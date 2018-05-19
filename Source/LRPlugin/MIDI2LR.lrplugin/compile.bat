REM To compile lua files, obtain version 5.1.4 of luac and place in this directory.
REM This is the Windows version of the compile command. For Mac or Linux will need appropriate batch file.
FOR %%a IN (*.lua) DO luac -o compiled/%%a %%a