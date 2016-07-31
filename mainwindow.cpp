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
#include "com_fifo.h"

#define RX_BUF_SIZE  1024*512
#define STR_BUF_SIZE  1024*1024



char rx_buf[128];
char rx_DataSave[RX_BUF_SIZE];
int  rx_DataSave_addr=0;
char lStrBuf[STR_BUF_SIZE];
int  lStrBuf_addr=0;




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _RxBigBuffer = new unsigned char[1024];//(RX_BIG_BUFFER_SIZE);
    _RxBigBufferWriteAddr = 0;

	if(!com_fifo_init(&rx_fifo)){
		ui->textBrowser->append("Com Fifo init failed!");
	}
    memset(lStrBuf,0,STR_BUF_SIZE);
    _ComIsOpen = false;
	_bStartSample = false;
	ui->StartSampleBtn->setEnabled(false);
	ui->StopSampleBtn->setEnabled(false);

	_DisplayTimer.setInterval(100);
    _DisplayTimer.start();
    connect(&_DisplayTimer,SIGNAL(timeout()),this,SLOT(DisplayTimeout()));

    setWindowTitle("IMU Calibration Tool");
	ui->openMyComBtn->setEnabled(false);
    ui->closeMyComBtn->setEnabled(false); //开始“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false); //开始“发送数据”按钮不可用
    this->setStyleSheet("background-color:rgb(200,200,200)");
//	ui->AccelGpBox->setStyleSheet("background-color:rgb(200,200,200)");
//	ui->GyroGpBox->setStyleSheet("background-color:rgb(200,200,200)");
    ui->textBrowser->setStyleSheet("background-color:rgb(240,240,240)");

    ui->IMU_ID_lineEdit->setStyleSheet("background-color:rgb(240,240,240)");
	ui->SampleTotal_lineEdit->setStyleSheet("background-color:rgb(240,240,240)");
	ui->SampleGpBox->setStyleSheet("background-color:rgb(255,128,0)");
	ui->TotalCmd_label->setText("总数：");
	//------------------------------------------------------------------------
	SetTableView();

}
char tmp;
MainWindow::~MainWindow()
{

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

void MainWindow::SetTableView()
{
	_pTableView = new QStandardItemModel();
	_pTableView->setColumnCount(6);
	_pTableView->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Name"));
	_pTableView->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("CurVal"));
	_pTableView->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("MaxVal"));
	_pTableView->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("MinVal"));
	_pTableView->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("AvgVal"));
	_pTableView->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("Temp"));

	_pTableView->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignCenter);
	_pTableView->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);
	_pTableView->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignCenter);
	_pTableView->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignCenter);
	_pTableView->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignCenter);
	_pTableView->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignCenter);
	ui->tableView->setModel(_pTableView);
	//表头信息显示居左
	ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	//设置列宽不可变
	ui->tableView->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
	ui->tableView->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
	ui->tableView->horizontalHeader()->setResizeMode(2,QHeaderView::Fixed);
	ui->tableView->horizontalHeader()->setResizeMode(3,QHeaderView::Fixed);
	ui->tableView->horizontalHeader()->setResizeMode(4,QHeaderView::Fixed);
	ui->tableView->horizontalHeader()->setResizeMode(5,QHeaderView::Fixed);
	ui->tableView->setColumnWidth(0,90);
	ui->tableView->setColumnWidth(1,105);
	ui->tableView->setColumnWidth(2,105);
	ui->tableView->setColumnWidth(3,105);
	ui->tableView->setColumnWidth(4,105);
	ui->tableView->setColumnWidth(5,105);

	_pTableView->setItem(0,0,new QStandardItem("AccelX"));
	_pTableView->setItem(1,0,new QStandardItem("AccelY"));
	_pTableView->setItem(2,0,new QStandardItem("AccelZ"));
	_pTableView->setItem(3,0,new QStandardItem("GyroX"));
	_pTableView->setItem(4,0,new QStandardItem("GyroY"));
	_pTableView->setItem(5,0,new QStandardItem("GyroZ"));
	_pTableView->setItem(5,0,new QStandardItem("Baro"));


}

int  SentOrder = 0;
void MainWindow::readMyCom() //读串口函数
{
	if(!_bStartSample){
		return;
	}

    static QByteArray gRecData="";
//    static char lRxCmd[64];
    static int lRxCmdCnt=0;
	static int lGetCmdOrder = 0;
	static int lRxTotal = 0;
	QString lDataStr4File;
	if( lRxCmdCnt >= ui->SampleTotal_lineEdit->text().toInt() ){
		MainWindow::on_StopSampleBtn_clicked();
		ui->textBrowser->append("Have receive enough data . Stop Sample !");
	}
	memset(rx_buf,0,RX_BUF_SIZE);
	int lCnt = myCom->read(rx_buf,RX_BUF_SIZE);

	if(lCnt>0){
		lRxTotal += lCnt;
		ui->textBrowser->append(QString("lCnt:%1,lRxTotal:%2 = CMD:%3").arg(lCnt).arg(lRxTotal).arg(lRxTotal/42));
		com_fifo_write(&rx_fifo,rx_buf,lCnt);
		memset(rx_buf,0,16);


		while(rx_fifo.total>0){

			if(0==lGetCmdOrder){
				com_fifo_read(&rx_fifo,rx_buf,1);
				if( 0xFF == (unsigned char)rx_buf[0] ){
					lGetCmdOrder = 1;
				}
			}else if(1==lGetCmdOrder){
				com_fifo_read(&rx_fifo,rx_buf,1);
				if( 0x8A == (unsigned char)rx_buf[0] ){
					lGetCmdOrder = 2;
				}
			}else if(2==lGetCmdOrder){
				if(rx_fifo.total>=40){
					lGetCmdOrder = 0;
					rx_buf[0] = 0xFF;
					rx_buf[1] = 0x8A;
					com_fifo_read(&rx_fifo,rx_buf+2,40);

					if( 0x0A == (unsigned char)rx_buf[41] ){
						TYPE_sensors_data_t *lpDataFrame = (TYPE_sensors_data_t *)rx_buf;

						lRxCmdCnt++;
						ui->TotalCmd_label->setText("总数："+QString("%1").arg(lRxCmdCnt));
						lDataStr4File += QString("%1,%2,%3,%4,%5,%6,%7,%8,\r\n")
										 .arg(lRxCmdCnt).arg(lpDataFrame->uid)
										 .arg(lpDataFrame->accel_x)
										 .arg(lpDataFrame->accel_y)
										 .arg(lpDataFrame->accel_z)
										 .arg(lpDataFrame->gyro_x)
										 .arg(lpDataFrame->gyro_y)
										 .arg(lpDataFrame->gyro_z)
										 .arg(lpDataFrame->baro);
						//---------------------------------------------------
						_pTableView->setItem(0,1,new QStandardItem("AccelX"));
						_pTableView->setItem(1,1,new QStandardItem("AccelY"));
						_pTableView->setItem(2,1,new QStandardItem("AccelZ"));
						_pTableView->setItem(3,1,new QStandardItem("GyroX"));
						_pTableView->setItem(4,1,new QStandardItem("GyroY"));
						_pTableView->setItem(5,1,new QStandardItem("GyroZ"));
						_pTableView->setItem(5,1,new QStandardItem("Baro"));
					}

				}else{
					break;
				}
			}

		}
//		ui->textBrowser_2->append(lDataStr4File);


		QTextStream out(_pDataStorageFile);
		out<<lDataStr4File;
		out.flush();
		ui->textBrowser->append(QString("Total:%1,WriteAddr:%2,ReadAddr:%3,lRxCmdCnt:%4").arg(rx_fifo.total).arg(rx_fifo.WriteAddr).arg(rx_fifo.ReadAddr).arg(lRxCmdCnt));

	}
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


	if(!_pDataStorageFile->open(QIODevice::WriteOnly|QIODevice::Text)){
		ui->textBrowser->insertPlainText(_DataStorageFileName);
		ui->textBrowser->insertPlainText("File Open failed!\r\n");
	}

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
	_pDataStorageFile->close();
}

void MainWindow::on_sendMsgBtn_clicked()
{

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
//	ui->tableView->

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
			ui->textBrowser->append(QString(" Order=%1: %2 | %3 | %4 ")
									.arg(i).arg(lFileData[i][0])
									.arg(lFileData[i][1])
									.arg(lFileData[i][2]));
        }
    }else{
        ui->textBrowser->insertPlainText(_ParamsFileName);
         ui->textBrowser->insertPlainText("File Open failed!\r\n");
    }
	file.close();
}


void MainWindow::on_ChooseFileBtn_clicked()
{
    _ParamsFileName = QFileDialog::getOpenFileName(this,tr("open file"),"",tr("Allfile(*.*);;datFile(*.dat)"));
	ui->textBrowser->append(_ParamsFileName);
}

void MainWindow::on_ChooseSaveFilePathBtn_clicked()
{
	_DataStorageFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
												   "untitled.dat",
												   tr("Data (*.dat);;CSV (*.csv)"));
	ui->textBrowser->append(_DataStorageFileName);
	_pDataStorageFile = new QFile(_DataStorageFileName);
	ui->openMyComBtn->setEnabled(true);
	ui->StartSampleBtn->setEnabled(true);
}


void MainWindow::on_StartSampleBtn_clicked()
{
	if(ui->SampleTotal_lineEdit->text().toInt()<1){
		ui->textBrowser->append("请设置采样数！");
		return;
	}
	_bStartSample = true;
	ui->StartSampleBtn->setEnabled(false);
	ui->StopSampleBtn->setEnabled(true);
	ui->SampleTotal_lineEdit->setEnabled(false);
	ui->SampleGpBox->setStyleSheet("background-color:rgb(0,255,0)");
}

void MainWindow::on_StopSampleBtn_clicked()
{
	_bStartSample = false;
	ui->StartSampleBtn->setEnabled(true);
	ui->StopSampleBtn->setEnabled(false);
	ui->SampleTotal_lineEdit->setEnabled(true);
	ui->SampleGpBox->setStyleSheet("background-color:rgb(255,0,0)");
}
