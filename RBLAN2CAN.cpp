#include "RBLAN2CAN.h"
#include <iostream>
using namespace std;

RBLAN2CAN::RBLAN2CAN()
{
    sock1ConnectionStatus = false;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(100);

    server = new QTcpServer();
    recv_fail_cnt = 0;
    recv_success_cnt = 0;
    connect(&socket1, SIGNAL(connected()), this, SLOT(onSock1Connected()));
    connect(&socket1, SIGNAL(disconnected()), this, SLOT(onSock1Disconnected()));
    connect(&socket1, SIGNAL(readyRead()), this, SLOT(onSock1ReadyRead()));
}

int RBLAN2CAN::get_lan2can_sock1connection(){ return sock1ConnectionStatus;}

void RBLAN2CAN::onSock1Connected(){
    recvBuf1.clear();
    sock1ConnectionStatus = true;
    cout << "Satellite Board 1 Connected" << endl;
}
void RBLAN2CAN::onSock1Disconnected(){
    sock1ConnectionStatus = false;
    cout << "Satellite Board 1 Disonnected" << endl;
}

void RBLAN2CAN::onTimer(){
    // 배출구, 바코드, 컵디스펜서
    if(sock1ConnectionStatus == false){
        if(socket1.state() == QAbstractSocket::UnconnectedState) socket1.connectToHost("192.168.1.99", 1977);
    }

    recv_fail_cnt++;
}

#define RX_DATA_SIZE    10000
void RBLAN2CAN::onSock1ReadyRead(){
    unsigned char tempPacketData[RX_DATA_SIZE];

    QByteArray datas = socket1.readAll();
    recvBuf1 += datas;
    char *precvBuf;

    int end_pos = 0;
    int tcp_size;

    while(1){

        tcp_size = recvBuf1.size();
        precvBuf = recvBuf1.data();

        if(tcp_size < 4){
            break;
        }

        if(uchar(recvBuf1[0]) == 0x24){
            if(tcp_size >= 4){
                int packet_length = (short)(uchar(recvBuf1[1]) | (uchar(recvBuf1[2])<<8));
                if(packet_length < 0 || packet_length > RX_DATA_SIZE){
                    cout << "RBLAN2CAN:: packet length under zero " << packet_length << endl;
                    recvBuf1.remove(0, 1);
                }else{
                    end_pos = packet_length + 3 - 1;

                    if(tcp_size >= packet_length + 3){
                        memcpy(tempPacketData, precvBuf, packet_length+3);

                        if(uchar(tempPacketData[0]) == 0x24 && uchar(tempPacketData[packet_length + 3 - 1]) == 0x25){
                            int data_type = uchar(tempPacketData[5]);
                            recv_fail_cnt = 0;
                            recv_success_cnt++;
                        }else{
//                            cout << "RBLAN2CAN:: header footer not match" << endl;
                        }
                        recvBuf1.remove(0, end_pos);
                    }else{
//                        cout << "RBLANComm:: size not match : " << tcp_size-1 << ", " << end_pos << endl;
                        recvBuf1.remove(0, 1);
                    }
                }
            }
        }else{
            recvBuf1.remove(0, 1);
        }
    }
}

void RBLAN2CAN::SendControlData1(int target, int command, int para1, int para2){
    if(sock1ConnectionStatus == true){
        // Send Control Data ---------------------
        char send_byte[256];
        send_byte[0] = 0x24;            // HEADER
        unsigned int TotalDataLength = 14;
        send_byte[1] = (TotalDataLength)&0xFF;       // Total data length
        send_byte[2] = (TotalDataLength>>8)&0xFF;    // Total data length
        send_byte[3] = 0;               // From (Master : 0, Slave(CAN) : 1, General interface board : 2)
        send_byte[4] = 1;               // To (Master : 0, Slave(CAN) : 1, General interface board : 2)
        send_byte[5] = 0x10;               // Data type (CAN Data : 0)
        send_byte[6] = (target)&0xFF;
        send_byte[7] = (command)&0xFF;
        send_byte[8] = (para1)&0xFF;
        send_byte[9] = (para1>>8)&0xFF;
        send_byte[10] = (para1>>16)&0xFF;
        send_byte[11] = (para1>>24)&0xFF;
        send_byte[12] = (para2)&0xFF;
        send_byte[13] = (para2>>8)&0xFF;
        send_byte[14] = (para2>>16)&0xFF;
        send_byte[15] = (para2>>24)&0xFF;
        send_byte[16] = 0x25;

        socket1.write(send_byte, TotalDataLength+3);
    }
}
