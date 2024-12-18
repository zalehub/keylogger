keylogger.cpp:15:5: warning: 'auto' type specifier is a C++11 extension [-Wc++11-extensions]
   15 |     auto now = std::chrono::system_clock::now();
      |     ^
keylogger.cpp:16:5: warning: 'auto' type specifier is a C++11 extension [-Wc++11-extensions]
   16 |     auto time = std::chrono::system_clock::to_time_t(now);
      |     ^
2 warnings generated.
# 键盘输入监听器

这是一个简单的键盘输入监听程序，用于记录键盘输入。

## 功能
- 监听键盘输入
- 将按键记录保存到日志文件
- 支持后台运行
- 记录输入时间戳

## 编译方法
```bash
g++ -o keylogger keylogger.cpp
```

## 使用方法
```bash
./keylogger
```

键盘输入将被记录到 keylog.txt 文件中。

注意：本程序仅供学习和研究使用。
