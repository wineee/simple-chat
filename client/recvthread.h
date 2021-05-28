#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>

#include "IP.h"

class RecvThread : public QThread
{
    Q_OBJECT  // 用到信号与槽
public:
    RecvThread(QJsonObject obj);
    void run();

private slots:
    void recv_file();

private:
    QString fileName;
    int fileLength;
    int total;
    int port;
    QTcpSocket *recvSocket;
    QFile *file;

signals:
    void recv_file_success_signals(QString fileName);
};

#endif // RECVTHREAD_H
