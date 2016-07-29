#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "win_qextserialport.h"
#include <QTimer>

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
    QString  _ParamsFileName;
    QTimer  _DisplayTimer;
    bool    _ComIsOpen;
private slots:
    void on_ChooseFileBtn_clicked();
    void on_CleanRightScreamBtn_clicked();
    void on_ReadFileBtn_clicked();
    void on_testBtn_clicked();
    void on_CleanLeftScreamBtn_clicked();
    void on_TranBtn_clicked();
    void on_sendMsgBtn_clicked();
    void on_closeMyComBtn_clicked();
    void on_openMyComBtn_clicked();
    void readMyCom();
    void DisplayTimeout();
};

#endif // MAINWINDOW_H
