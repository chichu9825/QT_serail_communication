#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QTextStream>
#include <QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QIODevice>

#define RX_BUF_SIZE  1024*8
#define STR_BUF_SIZE  RX_BUF_SIZE*1024
char lData[RX_BUF_SIZE];
char lStrBuf[STR_BUF_SIZE];
int  lStrBuf_addr=0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    lData = new char(RX_BUF_SIZE);
//    lStrBuf = new char(STR_BUF_SIZE);
    memset(lStrBuf,0,STR_BUF_SIZE);
    ui->closeMyComBtn->setEnabled(false); //��ʼ���رմ��ڡ���ť������
    ui->sendMsgBtn->setEnabled(false); //��ʼ���������ݡ���ť������

}

MainWindow::~MainWindow()
{
//    delete lData;
    delete ui;
}


int  SentOrder = 0;
void MainWindow::readMyCom() //�����ں���
{
    static QByteArray gRecData="";
//    QByteArray temp = myCom->readAll();
////    //��ȡ���ڻ��������������ݸ���ʱ����temp
//    char *lData = temp.data();
    memset(lData,0,RX_BUF_SIZE);
    int lCnt = myCom->read(lData,RX_BUF_SIZE);
    if(lCnt>0){
//        sprintf(lStrBuf,"%d>>%d:%x,%x,%x\r\n",SentOrder++,lCnt,*(int *)lData,*(int *)(lData+4),*(int *)(lData+8));
//        memcpy(lStrBuf+lStrBuf_addr,lData,lCnt);
        gRecData.append(lData);
        lStrBuf_addr += lCnt;
//        ui->textBrowser->insertPlainText(lStrBuf);
        if(lStrBuf_addr>100000){
            ui->textBrowser->setText(gRecData);
//            ui->textBrowser->moveCursor(QTextCursor::End);
            lStrBuf_addr=0;
        }

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
//    lSentData[2] = 0x40;
    //lSentData[2] = 0x30;
    //lSentData[2] = 0x20;
    //lSentData[2] = 0x10;

    lSentData[2] = 0x40;
    *(float *)(lSentData+3) = 11.8322;//3#Gyro
    *(float *)(lSentData+7) = 45.7210f;
    *(float *)(lSentData+11) = 22.2408f;


    *(float *)(lSentData+15) = 16.3303f;//
    *(float *)(lSentData+19) = -0.0670f;
    *(float *)(lSentData+23) = 0.644f;
    *(float *)(lSentData+27) = 0.0414f;
    *(float *)(lSentData+31) = 16.364f;//
    *(float *)(lSentData+35) = 0.0889f;
    *(float *)(lSentData+39) = -1.5428f;
    *(float *)(lSentData+43) = -0.2155f;
    *(float *)(lSentData+47) = 16.3462f;//

//    lSentData[2] = 0x30;
//    *(float *)(lSentData+3) = 47.9461f;//3#ACCEL
//    *(float *)(lSentData+7) = -52.3691f;
//    *(float *)(lSentData+11) = 51.9598f;


//    *(float *)(lSentData+15) = 4078.1867f;//
//    *(float *)(lSentData+19) = -17.8834f;
//    *(float *)(lSentData+23) = 393.4122f;
//    *(float *)(lSentData+27) = 9.8891f;
//    *(float *)(lSentData+31) = 4099.9922f;//
//    *(float *)(lSentData+35) = 23.9538f;
//    *(float *)(lSentData+39) = -403.9699f;
//    *(float *)(lSentData+43) = -34.2214f;
//    *(float *)(lSentData+47) = 4111.3204f;//







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
