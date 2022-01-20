#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

int flag_connection = 0;
int flag_data = 0;
int prev_connection = 0;
int disconnection_count = 0;
int noreceive_count = 0;

int count1 = 0;
int count5 = 0;
int count10 = 0;
int dis_count1 = 0;
int dis_count5 = 0;
int dis_count10 = 0;

void SetLEColor(QLineEdit *le, QString color){
    le->setStyleSheet("QLineEdit{background-color:"+color+"}");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    server = new RBTCPServer();
    server->RBServerOpen(QHostAddress::AnyIPv4,1977);


    timer = new QTimer();

    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    timer->start(200);

    ui->TE_LOG->append(QDateTime::currentDateTime().toString("[yyyy-MM-dd] hh:mm:ss")+"  program start.. ");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimer()
{
    if(server->RBConnectionStatus == true){
        // Send Control Data ---------------------
        char send_byte[256];
        send_byte[0] = 0x24;            // HEADER
        unsigned int TotalDataLength = 14;
        send_byte[1] = (TotalDataLength)&0xFF;       // Total data length
        send_byte[2] = (TotalDataLength>>8)&0xFF;    // Total data length
        send_byte[3] = 0;               // From (Master : 0, Slave(CAN) : 1, General interface board : 2)
        send_byte[4] = 1;               // To (Master : 0, Slave(CAN) : 1, General interface board : 2)
        send_byte[5] = 0x10;               // Data type (CAN Data : 0)
        send_byte[6] = (0)&0xFF;
        send_byte[7] = (0)&0xFF;
        send_byte[8] = (0)&0xFF;
        send_byte[9] = (0>>8)&0xFF;
        send_byte[10] = (0>>16)&0xFF;
        send_byte[11] = (0>>24)&0xFF;
        send_byte[12] = (0)&0xFF;
        send_byte[13] = (0>>8)&0xFF;
        send_byte[14] = (0>>16)&0xFF;
        send_byte[15] = (0>>24)&0xFF;
        send_byte[16] = 0x25;
        server->RBSendData(send_byte, TotalDataLength+3);
    }
}
