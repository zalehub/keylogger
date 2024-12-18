#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>

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
