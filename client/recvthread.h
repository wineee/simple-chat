#ifndef RECVTHREAD_H
#define RECVTHREAD_H
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>



class RecvThread : public QThread
{
    Q_OBJECT
public:
    RecvThread(QJsonObject obj);
    void run();   //子线程SS

private slots:
    void recv_file();   //父线程

private:
    QString fileName;
    int fileLength;
    int total;
    int port;
    QTcpSocket * recvSocket;
    QFile * file;
};

#endif // RECVTHREAD_H
