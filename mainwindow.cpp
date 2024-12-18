#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口标题和大小
    setWindowTitle("键盘记录器");
    resize(600, 400);

    // 创建中央部件和布局
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    layout = new QVBoxLayout(centralWidget);

    // 创建文本显示区域
    logDisplay = new QTextEdit(this);
    logDisplay->setReadOnly(true);
    layout->addWidget(logDisplay);

    // 创建按钮
    startButton = new QPushButton("开始记录", this);
    stopButton = new QPushButton("停止记录", this);
    stopButton->setEnabled(false);

    layout->addWidget(startButton);
    layout->addWidget(stopButton);

    // 创建键盘记录器实例
    keylogger = new KeyLogger();

    // 创建定时器用于更新显示
    updateTimer = new QTimer(this);
    updateTimer->setInterval(100); // 每0.1秒更新一次

    // 连接信号和槽
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startLogging);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopLogging);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateLog);
}

MainWindow::~MainWindow()
{
    delete keylogger;
}

void MainWindow::startLogging()
{
    // 启动键盘记录
    keylogger->start();
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    updateTimer->start();
}

void MainWindow::stopLogging()
{
    // 停止键盘记录
    keylogger->stop();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    updateTimer->stop();
}

void MainWindow::updateLog()
{
    // 读取日志文件并更新显示
    QFile file("keylog.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
#endif
        QString content = in.readAll();
        if (content != logDisplay->toPlainText()) {
            // 保存当前滚动条位置
            QScrollBar *scrollBar = logDisplay->verticalScrollBar();
            bool wasAtBottom = scrollBar->value() == scrollBar->maximum();
            
            // 更新内容
            logDisplay->setText(content);
            
            // 如果之前在底部，则保持在底部
            if (wasAtBottom) {
                scrollBar->setValue(scrollBar->maximum());
            }
        }
        file.close();
    }
}
