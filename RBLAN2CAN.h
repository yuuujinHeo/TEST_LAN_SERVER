#ifndef RBLAN2CAN_H
#define RBLAN2CAN_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>

class RBLAN2CAN : QObject
{
    Q_OBJECT

public:
    RBLAN2CAN();

    QTcpSocket  socket1;


    int sock1ConnectionStatus;

    int get_lan2can_sock1connection();

    int recv_fail_cnt;
    int recv_success_cnt;

    void SendControlData1(int target, int command, int para1, int para2);

public slots:
    void onTimer();

    void onSock1Connected();
    void onSock1Disconnected();
    void onSock1ReadyRead();
private:
    QTimer *timer;
    QTcpServer *server;
    QByteArray recvBuf1;
};

#endif // RBLAN2CAN_H
