#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>

#define RX_BUF_SIZE  1024*16
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
    _ComIsOpen = false;
    _DisplayTimer.setInterval(500);
    _DisplayTimer.start();
    connect(&_DisplayTimer,SIGNAL(timeout()),this,SLOT(DisplayTimeout()));

    setWindowTitle("IMU Calibration Tool");
    ui->closeMyComBtn->setEnabled(false); //开始“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false); //开始“发送数据”按钮不可用
    this->setStyleSheet("background-color:rgb(200,200,200)");
    ui->groupBox_2->setStyleSheet("color:black");
    ui->groupBox_2->setStyleSheet("background-color:rgb(200,200,200)");
    ui->textBrowser->setStyleSheet("background-color:rgb(240,240,240)");
    ui->textBrowser_2->setStyleSheet("background-color:rgb(240,240,240)");
    ui->IMU_ID_lineEdit->setStyleSheet("background-color:rgb(240,240,240)");
}
char tmp;
MainWindow::~MainWindow()
{
//    delete lData;
    delete ui;
}
#define SUNCO_RX_CMD_SIZE 42

bool rx_filter(char *rx_data,char *rx_buf,int rx_buf_len)
{
        static int32_t	_rx_cmd_order = 0;
        static int32_t	_rx_cmd_data_addr = 0 ;

        for(int i=0;i<rx_buf_len;i++){
                if(_rx_cmd_data_addr>SUNCO_RX_CMD_SIZE){//
                        _rx_cmd_data_addr = 0;;
                        _rx_cmd_order= 0 ;
                }

                switch(_rx_cmd_order){
                        case 0:
                                tmp = rx_buf[i];
                                if(((unsigned char)rx_buf[i]==0xFF)||((unsigned char)rx_buf[i]==0xF0)){
//                                if((unsigned char)rx_buf[i]==0xFF){
                                        _rx_cmd_order = 1;
                                }
                                break;
                        case 1:
                                if((unsigned char)rx_buf[i]==0x8A){
                                        // warnx("Find sencond Head");
                                        _rx_cmd_order = 2;

//                                        memset( rx_data , 0 , RX_BUF_SIZE );
                                        rx_data[0] = 0x00;
                                        rx_data[1] = 0x8A;
                                        _rx_cmd_data_addr = 2;
                                }else{
                                        // warnx("No Find sencond Head");
                                        _rx_cmd_order = 0;
                                }
                                break;
                        case 2:
                                rx_data[_rx_cmd_data_addr] = rx_buf[i];
                                if( ++_rx_cmd_data_addr == SUNCO_RX_CMD_SIZE ){

//                                        if(rx_buf[i]==0x0A){//
//                                                warnx("_rx_cmd_data_addr=%d",_rx_cmd_data_addr);
                                                _rx_cmd_order = 0;//order 0
                                                // warnx("Rx from Camera:%x,%x,%x,%x,%x,%x,%x,%x",rx_data[0],rx_data[1],rx_data[2],rx_data[3];
                                                return true;
//                                        }else{
////                                                warnx("Don't find Cmd.");
//                                                _rx_cmd_data_addr = 0;;
//                                                _rx_cmd_order= 0 ;
//                                        }
                                }

                                break;
                        default:
                                // warnx("Error _rx_cmd_order value = %x",_rx_cmd_order);
                                _rx_cmd_order = 0;//
                                break;
                }
        }
        return false;
}
void MainWindow::DisplayTimeout()
{
    if(_ComIsOpen){
        readMyCom();
    }
//    ui->textBrowser->append("Hello");
}

int  SentOrder = 0;
void MainWindow::readMyCom() //读串口函数
{
    static QByteArray gRecData="";
//    static char lRxCmd[64];

//    QByteArray temp = myCom->readAll();
////    //读取串口缓冲区的所有数据给临时变量temp
//    char *lData = temp.data();
    memset(lData,0,RX_BUF_SIZE);
    int lCnt = myCom->read(lData,RX_BUF_SIZE);

    ui->textBrowser->append(QString("lCnt:%1").arg(lCnt));
//    if(lCnt>0){
//        if(rx_filter(lRxCmd,lData,lCnt)){
//            static int lRxCmdCnt = 0;
//            static int lRxCmdPrintCnt = 0;
//            static int lRxCmdTotalCnt = 0;
//            ++lRxCmdTotalCnt;
//            if( ++lRxCmdCnt>200 ){
//                lRxCmdCnt = 0;
//                char lStr[256];
//                memset(lStr,0,sizeof(lStr));
//                sprintf(lStr,"Order:%d | Total:%8d",lRxCmdPrintCnt++,lRxCmdTotalCnt);
////                ui->textBrowser->setText(lStr);
////                ui->textBrowser->append("Hello");
//            }
//            memset(lRxCmd,0,sizeof(lRxCmd));
//        }





////        sprintf(lStrBuf,"%d>>%d:%x,%x,%x\r\n",SentOrder++,lCnt,*(int *)lData,*(int *)(lData+4),*(int *)(lData+8));
////        memcpy(lStrBuf+lStrBuf_addr,lData,lCnt);
//        gRecData.append(lData);
//        lStrBuf_addr += lCnt;
////        ui->textBrowser->insertPlainText(lStrBuf);
//        if(lStrBuf_addr>100000){
//            ui->textBrowser->setText(gRecData);
////            ui->textBrowser->moveCursor(QTextCursor::End);
//            lStrBuf_addr=0;
//        }

//    }
}
void MainWindow::on_openMyComBtn_clicked()
{
    QString portName = ui->portNameComboBox->currentText(); //获取串口名
    myCom = new Win_QextSerialPort("\\\\.\\"+portName,QextSerialBase::EventDriven);
    //定义串口对象，并传递参数，在构造函数里对其进行初始化
    if(!myCom ->open(QIODevice::ReadWrite)){//打开串口
        ui->textBrowser->append("Com Open Failed!");
        return;
    }
    setWindowTitle(ui->IMU_ID_lineEdit->text()+":"+ui->portNameComboBox->currentText());
    if(ui->baudRateComboBox->currentText()==tr("9600")) //根据组合框内容对串口进行设置
    myCom->setBaudRate(BAUD9600);
    else if(ui->baudRateComboBox->currentText()==tr("115200"))
    myCom->setBaudRate(BAUD115200);
    //设置波特率
    if(ui->dataBitsComboBox->currentText()==tr("8"))
    myCom->setDataBits(DATA_8);
    else if(ui->dataBitsComboBox->currentText()==tr("7"))
    myCom->setDataBits(DATA_7);
    //设置数据位
    if(ui->parityComboBox->currentText()==tr("无"))
    myCom->setParity(PAR_NONE);
    else if(ui->parityComboBox->currentText()==tr("奇"))
    myCom->setParity(PAR_ODD);
    else if(ui->parityComboBox->currentText()==tr("偶"))
    myCom->setParity(PAR_EVEN);
    //设置奇偶校验
    if(ui->stopBitsComboBox->currentText()==tr("1"))
    myCom->setStopBits(STOP_1);
    else if(ui->stopBitsComboBox->currentText()==tr("2"))
    myCom->setStopBits(STOP_2);
    //设置停止位
    myCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
    myCom->setTimeout(500); //设置延时
//    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
    //信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作
    ui->openMyComBtn->setEnabled(false); //打开串口后“打开串口”按钮不可用
    ui->closeMyComBtn->setEnabled(true); //打开串口后“关闭串口”按钮可用
    ui->sendMsgBtn->setEnabled(true); //打开串口后“发送数据”按钮可用
    ui->baudRateComboBox->setEnabled(false); //设置各个组合框不可用
    ui->dataBitsComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->portNameComboBox->setEnabled(false);

    _ComIsOpen = true;
}
void MainWindow::on_closeMyComBtn_clicked()
{
    _ComIsOpen = false;
    myCom->close();
    ui->openMyComBtn->setEnabled(true); //关闭串口后“打开串口”按钮可用
    ui->closeMyComBtn->setEnabled(false); //关闭串口后“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false); //关闭串口后“发送数据”按钮不可用
    ui->baudRateComboBox->setEnabled(true); //设置各个组合框可用
    ui->dataBitsComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);
}

void MainWindow::on_sendMsgBtn_clicked()
{
//    myCom->write(ui->sendMsgLineEdit->text().toAscii());

    char lSentData[64];
    lSentData[0] = 0xAE;
    lSentData[1] = 0xA2;

    lSentData[2] = 0x30;
    *(float *)(lSentData+3) = -6.001736111f;//3#ACCEL
    *(float *)(lSentData+7) = 29.81076389f;
    *(float *)(lSentData+11) = 76.79620833f;


    *(float *)(lSentData+15) = 4079.780167f;//
    *(float *)(lSentData+19) = -115.2742917f;
    *(float *)(lSentData+23) = 418.561f;

    *(float *)(lSentData+27) = 125.2491667f;
    *(float *)(lSentData+31) = 4094.3785f;//
    *(float *)(lSentData+35) = -120.3297917f;

    *(float *)(lSentData+39) = -414.4640417f;
    *(float *)(lSentData+43) = 130.2855417f;
    *(float *)(lSentData+47) = 4075.508792f;//

    lSentData[51] = 0xEA;

    myCom->write(lSentData,52);
    //以ASCII码形式将行编辑框中的数据写入串口
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
    *(float *)(lSentData+3) = 10.67141667f  ;//3#Gyro
    *(float *)(lSentData+7) = 3.100902778f   ;
    *(float *)(lSentData+11) = 46.16519444f    ;


    *(float *)(lSentData+15) = 16.3972375f   ;//
    *(float *)(lSentData+19) = 0.789029167f   ;
    *(float *)(lSentData+23) = 1.121291667f    ;

    *(float *)(lSentData+27) = -0.784379167f   ;
    *(float *)(lSentData+31) = 16.314625f   ;//
    *(float *)(lSentData+35) = 0.0076625f   ;

    *(float *)(lSentData+39) = -1.208220833f;
    *(float *)(lSentData+43) = -0.2182875f;
    *(float *)(lSentData+47) = 16.3757375f;//

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

void MainWindow::on_CleanLeftScreamBtn_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_testBtn_clicked()
{
    ui->textBrowser->insertPlainText("111111111111111111111111\r\n");
    ui->textBrowser_2->insertPlainText("111111111111111111111111\r\n");
}

float lFileData[8][3];



void MainWindow::on_ReadFileBtn_clicked()
{
    QFile file(_ParamsFileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QString lReadBuf;
        QString lStrValue;
//        QString lPrint;
        QStringList lStrlist;
        for(int i=0;i<8;i++){
            lReadBuf = file.readLine(256);

            lStrlist = lReadBuf.split(",");
            lStrValue = lStrlist.at(0);
            lFileData[i][0] = lStrValue.toFloat();
            lStrValue = lStrlist.at(1);
            lFileData[i][1] = lStrValue.toFloat();
            lStrValue = lStrlist.at(2);
            lFileData[i][2] = lStrValue.toFloat();
            ui->textBrowser->append(QString(" Order=%1: %2 | %3 | %4 ").arg(i).arg(lFileData[i][0]).arg(lFileData[i][1]).arg(lFileData[i][2]));
//            ui->textBrowser->insertPlainText(lStrlist.at(0));
//            ui->textBrowser->insertPlainText(lStrlist.at(1));
//            ui->textBrowser->insertPlainText(lStrlist.at(2));
//            for(int j=0;j<3;j++){
//                lStrValue = lStrlist.at(i);
////                lFileData[i][j] = lStrValue.toFloat();
////                ui->textBrowser->insertPlainText(QString(" A: %1 ").arg(lFileData[i][j]));
//                  ui->textBrowser->insertPlainText(lStrValue);
//            }
//            ui->textBrowser->insertPlainText("\r\n");
        }








//        ui->textBrowser->insertPlainText(lReadBuf);
//        ui->textBrowser->insertPlainText(lStrlist.at(0));

//        lReadBuf = file.readLine(256);
//        ui->textBrowser->insertPlainText(lReadBuf);

    }else{
        ui->textBrowser->insertPlainText(_ParamsFileName);
         ui->textBrowser->insertPlainText("File Open failed!\r\n");
    }
    file.close();
}

void MainWindow::on_CleanRightScreamBtn_clicked()
{
        ui->textBrowser_2->clear();
}

void MainWindow::on_ChooseFileBtn_clicked()
{
    _ParamsFileName = QFileDialog::getOpenFileName(this,tr("open file"),"",tr("Allfile(*.*);;datFile(*.dat)"));
    ui->textBrowser_2->append(_ParamsFileName);
}
