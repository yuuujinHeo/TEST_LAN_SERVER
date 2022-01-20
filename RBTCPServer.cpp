#include "RBTCPServer.h"
#include <iostream>
#include <QDataStream>

using namespace std;

RBTCPServer::RBTCPServer(QObject *parent)
    : QTcpServer(parent), RBTcpServer(0)
{
    RBConnectionStatus = false;
    RBTcpClient = new QTcpSocket();
}

RBTCPServer::~RBTCPServer(){

}

void RBTCPServer::RBServerOpen(QHostAddress::SpecialAddress _host, quint16 _port){
    RBHostAddress = _host;
    RBPortNumber = _port;

    RBTcpServer = new QTcpServer(this);
    connect(RBTcpServer, SIGNAL(newConnection()),this,SLOT(RBNewConnection()));

    if(!RBTcpServer->listen(RBHostAddress,RBPortNumber)){
        cout << "RBServer unable to start" << endl;
    }else{
        cout << "server open success" << endl;
    }
}

void RBTCPServer::RBNewConnection(){
    if(RBConnectionStatus == false){
        qDebug() << "NewConnection";
        RBTcpClient = RBTcpServer->nextPendingConnection();
        connect(RBTcpClient, SIGNAL(disconnected()),this,SLOT(RBClientDisconnected()));
        connect(RBTcpClient, SIGNAL(readyRead()),this,SLOT(RBReadData()));
        RBConnectionStatus = true;
        emit SigNewConnection();
    }else{
        QTcpSocket *dummy = RBTcpServer->nextPendingConnection();
        dummy->close();
    }
}

void RBTCPServer::RBClientDisconnected(){
    RBTcpClient->deleteLater();
    RBConnectionStatus = false;
    emit SigDisConnection();
}

void RBTCPServer::RBSendData(QByteArray &data){
    RBTcpClient->write(data);
}
void RBTCPServer::RBSendData(const char *str, int len){
    RBTcpClient->write(str,len);
}

void RBTCPServer::RBReadData(){
    QDataStream in(RBTcpClient);
    in.setVersion(QDataStream::Qt_5_2);

    int size = RBTcpClient->bytesAvailable();
    RBData.resize(size);
    in.readRawData(RBData.data(), size);
    dataReceived.push_back(RBData);
}
