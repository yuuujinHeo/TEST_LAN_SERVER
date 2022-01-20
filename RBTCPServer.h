#ifndef RBTCPSERVER_H
#define RBTCPSERVER_H

#include <QTcpServer>
#include <QObject>
#include <QTcpSocket>

class RBTCPServer : public QTcpServer
{
    Q_OBJECT
public:
    RBTCPServer(QObject *parent = 0);
    ~RBTCPServer();

    void RBServerOpen(QHostAddress::SpecialAddress _host = QHostAddress::Any, quint16 _port = 1977);
    void RBSendData(QByteArray &data);
    void RBSendData(const char *str, int len);

    int RBConnectionStatus;

    QVector<QByteArray> dataReceived;
    QByteArray RBData;

private slots:
    void RBNewConnection();
    void RBClientDisconnected();

    void RBReadData();

signals:
    void SigNewConnection();
    void SigDisConnection();

protected:
    QTcpServer *RBTcpServer;
    QTcpSocket *RBTcpClient;

private:
    QHostAddress::SpecialAddress    RBHostAddress;
    quint16                         RBPortNumber;
};

#endif // RBTCPSERVER_H
