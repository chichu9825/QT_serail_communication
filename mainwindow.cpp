#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QTextStream>
#include <QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QIODevice>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->closeMyComBtn->setEnabled(false); //��ʼ���رմ��ڡ���ť������
    ui->sendMsgBtn->setEnabled(false); //��ʼ���������ݡ���ť������

}

MainWindow::~MainWindow()
{
    delete ui;
}
char lStrBuf[256];
char lData[256];
int  SentOrder = 0;
void MainWindow::readMyCom() //�����ں���
{
//    static QByteArray gRecData="";
//    QByteArray temp = myCom->readAll();
////    //��ȡ���ڻ��������������ݸ���ʱ����temp
//    char *lData = temp.data();

    int lCnt = myCom->read(lData,sizeof(lData));
    if(lCnt>0){
        sprintf(lStrBuf,"%d>>%d:%x,%x,%x\r\n",SentOrder++,lCnt,*(int *)lData,*(int *)(lData+4),*(int *)(lData+8));
        ui->textBrowser->insertPlainText(lStrBuf);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
}
void MainWindow::on_openMyComBtn_clicked()
{
    QString portName = ui->portNameComboBox->currentText(); //��ȡ������
    myCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);
    //���崮�ڶ��󣬲����ݲ������ڹ��캯���������г�ʼ��
    myCom ->open(QIODevice::ReadWrite); //�򿪴���
    if(ui->baudRateComboBox->currentText()==tr("9600")) //������Ͽ����ݶԴ��ڽ�������
    myCom->setBaudRate(BAUD9600);
    else if(ui->baudRateComboBox->currentText()==tr("115200"))
    myCom->setBaudRate(BAUD115200);
    //���ò�����
    if(ui->dataBitsComboBox->currentText()==tr("8"))
    myCom->setDataBits(DATA_8);
    else if(ui->dataBitsComboBox->currentText()==tr("7"))
    myCom->setDataBits(DATA_7);
    //��������λ
    if(ui->parityComboBox->currentText()==tr("��"))
    myCom->setParity(PAR_NONE);
    else if(ui->parityComboBox->currentText()==tr("��"))
    myCom->setParity(PAR_ODD);
    else if(ui->parityComboBox->currentText()==tr("ż"))
    myCom->setParity(PAR_EVEN);
    //������żУ��
    if(ui->stopBitsComboBox->currentText()==tr("1"))
    myCom->setStopBits(STOP_1);
    else if(ui->stopBitsComboBox->currentText()==tr("2"))
    myCom->setStopBits(STOP_2);
    //����ֹͣλ
    myCom->setFlowControl(FLOW_OFF); //�������������ƣ�����ʹ�������������Ƶ�Ĭ������
    myCom->setTimeout(500); //������ʱ
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
    //�źźͲۺ��������������ڻ�����������ʱ�����ж����ڲ���
    ui->openMyComBtn->setEnabled(false); //�򿪴��ں󡰴򿪴��ڡ���ť������
    ui->closeMyComBtn->setEnabled(true); //�򿪴��ں󡰹رմ��ڡ���ť����
    ui->sendMsgBtn->setEnabled(true); //�򿪴��ں󡰷������ݡ���ť����
    ui->baudRateComboBox->setEnabled(false); //���ø�����Ͽ򲻿���
    ui->dataBitsComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->portNameComboBox->setEnabled(false);
}
void MainWindow::on_closeMyComBtn_clicked()
{
    myCom->close();
    ui->openMyComBtn->setEnabled(true); //�رմ��ں󡰴򿪴��ڡ���ť����
    ui->closeMyComBtn->setEnabled(false); //�رմ��ں󡰹رմ��ڡ���ť������
    ui->sendMsgBtn->setEnabled(false); //�رմ��ں󡰷������ݡ���ť������
    ui->baudRateComboBox->setEnabled(true); //���ø�����Ͽ����
    ui->dataBitsComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);
}

void MainWindow::on_sendMsgBtn_clicked()
{
    myCom->write(ui->sendMsgLineEdit->text().toAscii());
    //��ASCII����ʽ���б༭���е�����д�봮��
}

void MainWindow::on_TranBtn_clicked()
{
    char lSentData[64];
    char lPrintString[128];
    lSentData[0] = 0xAE;
    lSentData[1] = 0xA2;
    lSentData[2] = 0x40;
    //lSentData[2] = 0x30;
    //lSentData[2] = 0x20;
    //lSentData[2] = 0x10;

//    *(float *)(lSentData+3) = 8.4911f;//3#Gyro
//    *(float *)(lSentData+7) = 37.8781f;
//    *(float *)(lSentData+11) = 6.3112f;


//    *(float *)(lSentData+15) = 16.367f;//
//    *(float *)(lSentData+19) = 0.2965f;
//    *(float *)(lSentData+23) = 1.3981f;
//    *(float *)(lSentData+27) = -0.2797f;
//    *(float *)(lSentData+31) = 16.2792f;//
//    *(float *)(lSentData+35) = -0.2604f;
//    *(float *)(lSentData+39) = -1.3182f;
//    *(float *)(lSentData+43) = 0.1445f;
//    *(float *)(lSentData+47) = 16.4142f;//

    *(float *)(lSentData+3) = 22.8523f;//3#ACCEL
    *(float *)(lSentData+7) = -33.3464f;
    *(float *)(lSentData+11) = -25.4625f;


    *(float *)(lSentData+15) = 4087.212f;//
    *(float *)(lSentData+19) = 73.4102f;
    *(float *)(lSentData+23) = 289.9808f;
    *(float *)(lSentData+27) = -69.2f;
    *(float *)(lSentData+31) = 4101.7323f;//
    *(float *)(lSentData+35) = -71.9285f;
    *(float *)(lSentData+39) = -311.1946f;
    *(float *)(lSentData+43) = 50.5323f;
    *(float *)(lSentData+47) = 4098.2975f;//


//    *(float *)(lSentData+3) = -4.1165f;//1#ACCEL
//    *(float *)(lSentData+7) = -83.5961f;
//    *(float *)(lSentData+11) = 178.8089f;


//    *(float *)(lSentData+15) = 4092.456f;//
//    *(float *)(lSentData+19) = -38.6172f;
//    *(float *)(lSentData+23) = 267.5296f;
//    *(float *)(lSentData+27) = 36.4395f;
//    *(float *)(lSentData+31) = 4095.288f;//
//    *(float *)(lSentData+35) = -95.1961f;
//    *(float *)(lSentData+39) = -143.732f;
//    *(float *)(lSentData+43) = 111.3026f;
//    *(float *)(lSentData+47) = 4093.6032f;//


//    *(float *)(lSentData+3) = 7.1342f;//1#GYRO
//    *(float *)(lSentData+7) = 13.7555f;
//    *(float *)(lSentData+11) = -4.8458f;


//    *(float *)(lSentData+15) = 16.3721f;//
//    *(float *)(lSentData+19) = -0.1494f;
//    *(float *)(lSentData+23) = 0.697f;
//    *(float *)(lSentData+27) = 0.1367f;
//    *(float *)(lSentData+31) = 16.3517f;//
//    *(float *)(lSentData+35) = -0.3487f;
//    *(float *)(lSentData+39) = -0.6272f;
//    *(float *)(lSentData+43) = 0.5559f;
//    *(float *)(lSentData+47) = 16.3526f;//

    lSentData[51] = 0xEA;

    for(int i=0;i<52;i++){
            sprintf(lPrintString,"%02x ",(unsigned char)lSentData[i]);
            ui->textBrowser->insertPlainText(lPrintString);
//            ui->textBrowser->moveCursor(QTextCursor::End);
    }
    sprintf(lPrintString,"\r\n");
    ui->textBrowser->insertPlainText(lPrintString);
    ui->textBrowser->moveCursor(QTextCursor::End);
}
