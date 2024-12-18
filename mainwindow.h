#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include "keylogger.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startLogging();
    void stopLogging();
    void updateLog();

private:
    QWidget *centralWidget;
    QVBoxLayout *layout;
    QTextEdit *logDisplay;
    QPushButton *startButton;
    QPushButton *stopButton;
    KeyLogger *keylogger;
    QTimer *updateTimer;
};

#endif // MAINWINDOW_H
