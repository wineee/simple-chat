#include "recvthread.h"

RecvThread::RecvThread(QJsonObject obj)
{
    total = 0;
    port = obj.value("port").toInt();
    fileLength = obj.value("length").toInt();

    QString pathName = obj.value("filename").toString();
    QStringList strList = pathName.split('/');
    fileName = strList.at(strList.size()-1);

}

void RecvThread::run() {
    //在子线程里面初始化
    file = new QFile(fileName);
    file->open(QIODevice::WriteOnly);

    recvSocket = new QTcpSocket;

    connect(recvSocket, &QTcpSocket::readyRead, this, &RecvThread::recv_file, Qt::DirectConnection);   //很中哟啊
    recvSocket->connectToHost(QHostAddress("39.108.144.134"), port);
    if(!recvSocket->waitForConnected(10000)) {
        this->quit();
    }else{

    }
    exec();//死循环使run一直运行
}

void RecvThread::recv_file() {
    QByteArray ba = recvSocket->readAll();
    total += ba.size();
    file->write(ba);

    if(total>=fileLength) {
        file->close();
        recvSocket->close();
        delete file;
        delete recvSocket;
        this->quit();
    }
}
