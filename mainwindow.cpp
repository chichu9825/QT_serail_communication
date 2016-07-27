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
    ui->closeMyComBtn->setEnabled(false); //开始“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false); //开始“发送数据”按钮不可用

}

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
//                                char tmp = rx_buf[i];
                                if((rx_buf[i]==0xFF)||(rx_buf[i]==0xF0)){
                                        _rx_cmd_order = 1;
                                }
                                break;
                        case 1:
                                if(rx_buf[i]==0x8A){
                                        // warnx("Find sencond Head");
                                        _rx_cmd_order = 2;

                                        memset( rx_data , 0 , RX_BUF_SIZE );
                                        rx_data[0] = 0x00;
                                        rx_data[1] = 0x8A;
                                        _rx_cmd_data_addr = 2;
                                }else{
                                        // warnx("No Find sencond Head");
                                        _rx_cmd_order = 0;
                                }
                                break;
                        case 2://				rx_data[_rx_cmd_data_addr] = rx_buf[i];

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


int  SentOrder = 0;
void MainWindow::readMyCom() //读串口函数
{
    static QByteArray gRecData="";
    static char lRxCmd[64];
//    QByteArray temp = myCom->readAll();
////    //读取串口缓冲区的所有数据给临时变量temp
//    char *lData = temp.data();
    memset(lData,0,RX_BUF_SIZE);
    int lCnt = myCom->read(lData,RX_BUF_SIZE);
    if(lCnt>0){
        if(rx_filter(lRxCmd,lData,lCnt)){
            char lStr[256];
            memset(lStr,0,sizeof(lStr));
            sprintf(lStr,"AA:%8.6f",*(double *)rx_filter);
            ui->textBrowser->setText(lStr);
            memset(lRxCmd,0,sizeof(lRxCmd));
        }





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
    QString portName = ui->portNameComboBox->currentText(); //获取串口名
    myCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);
    //定义串口对象，并传递参数，在构造函数里对其进行初始化
    myCom ->open(QIODevice::ReadWrite); //打开串口
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
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
    //信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作
    ui->openMyComBtn->setEnabled(false); //打开串口后“打开串口”按钮不可用
    ui->closeMyComBtn->setEnabled(true); //打开串口后“关闭串口”按钮可用
    ui->sendMsgBtn->setEnabled(true); //打开串口后“发送数据”按钮可用
    ui->baudRateComboBox->setEnabled(false); //设置各个组合框不可用
    ui->dataBitsComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->portNameComboBox->setEnabled(false);
}
void MainWindow::on_closeMyComBtn_clicked()
{
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

//    -2.358333333	3.889319444	-0.83775
//    16.34295833	-0.028242796	0.097135871
//    0.030534328	16.33298333	-0.029176391
//    -0.101929836	0.029802727	16.31939583

    lSentData[2] = 0x40;
    *(float *)(lSentData+3) = -2.358333333f;//3#Gyro
    *(float *)(lSentData+7) = 3.889319444f;
    *(float *)(lSentData+11) = -0.83775f;


    *(float *)(lSentData+15) = 16.34295833f;//
    *(float *)(lSentData+19) = -0.028242796f;
    *(float *)(lSentData+23) = 0.097135871f;

    *(float *)(lSentData+27) = 0.030534328f;
    *(float *)(lSentData+31) = 16.33298333f;//
    *(float *)(lSentData+35) = -0.029176391f;

    *(float *)(lSentData+39) = -0.101929836f;
    *(float *)(lSentData+43) = 0.029802727f;
    *(float *)(lSentData+47) = 16.31939583f;//






//    -6.001736111	29.81076389	76.79620833
//    4079.780167	125.2491667	-414.4640417
//    -115.2742917	4094.3785	130.2855417
//    418.561	-120.3297917	4075.508792



//    lSentData[2] = 0x30;
//    *(float *)(lSentData+3) = -6.001736111f;//3#ACCEL
//    *(float *)(lSentData+7) = 29.81076389f;
//    *(float *)(lSentData+11) = 76.79620833f;


//    *(float *)(lSentData+15) = 4079.780167f;//
//    *(float *)(lSentData+19) = 125.2491667f;
//    *(float *)(lSentData+23) = -414.4640417f;

//    *(float *)(lSentData+27) = -115.2742917f;
//    *(float *)(lSentData+31) = 4094.3785f;//
//    *(float *)(lSentData+35) = 130.2855417f;

//    *(float *)(lSentData+39) = 418.561f;
//    *(float *)(lSentData+43) = -120.3297917f;
//    *(float *)(lSentData+47) = 4075.508792f;//

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
