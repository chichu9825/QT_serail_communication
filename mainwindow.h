#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "win_qextserialport.h"
#include <QTimer>
#include <string.h>
#include <QtCore/qmath.h>
#include <QTreeView>
#include <QStandardItemModel>
//---------------------------------------------
#include <QStyledItemDelegate>
#include <QPainter>
#include <QHeaderView>

#include "com_fifo.h"
#define RX_BIG_BUFFER_SIZE 1024*1024*64

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
	QString  _DataStorageFileName;
    QTimer  _DisplayTimer;
	QFile	*_pDataStorageFile;
	QStandardItemModel  *_pTableView;
    bool    _ComIsOpen;
	bool	_bStartSample;
    unsigned char  *_RxBigBuffer;
    int    _RxBigBufferWriteAddr;
    int    _RxBigBufferReadAddr;
    COM_FIFO_TYPE   rx_fifo;
private slots:

	void on_StopSampleBtn_clicked();
 void on_StartSampleBtn_clicked();
 void on_ChooseSaveFilePathBtn_clicked();
	void on_ChooseFileBtn_clicked();
    void on_ReadFileBtn_clicked();
    void on_CleanLeftScreamBtn_clicked();
    void on_TranBtn_clicked();
    void on_sendMsgBtn_clicked();
    void on_closeMyComBtn_clicked();
    void on_openMyComBtn_clicked();
    void readMyCom();
	void SetTableView();
    void DisplayTimeout();
	void SampleInit();
};

#endif // MAINWINDOW_H
