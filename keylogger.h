#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

class KeyLogger {
private:
    std::ofstream logFile;
    bool isRunning;
    std::string buffer;
    
public:
    KeyLogger();
    ~KeyLogger();
    
    void start();
    void stop();
    void logKey(char key);
    void logUnicode(const std::string& text);
    std::string getCurrentTimestamp();
};

#endif // KEYLOGGER_H
