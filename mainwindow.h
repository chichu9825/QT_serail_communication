#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "win_qextserialport.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Win_QextSerialPort *myCom;
private slots:
    void on_ReadFileBtn_clicked();
    void on_testBtn_clicked();
    void on_CleanScreamBtn_clicked();
    void on_TranBtn_clicked();
    void on_sendMsgBtn_clicked();
    void on_closeMyComBtn_clicked();
    void on_openMyComBtn_clicked();
    void readMyCom();
};

#endif // MAINWINDOW_H
