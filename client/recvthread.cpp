#include "recvthread.h"

RecvThread::RecvThread(QJsonObject obj) {
    total = 0;
    port = obj.value("port").toInt();
    fileLength = obj.value("length").toInt();

    QString pathName = obj.value("filename").toString();
    QStringList strList = pathName.split('/');
    fileName = strList.at(strList.size()-1);
}

void RecvThread::run() {
    file = new QFile("recv_" + fileName);
    file->open(QIODevice::WriteOnly);
    recvSocket = new QTcpSocket;
    connect(recvSocket, &QTcpSocket::readyRead, this, &RecvThread::recv_file, Qt::DirectConnection);
    // Qt::DirectConnection 参考 https://www.bbsmax.com/A/kjdw1OpAJN/
    recvSocket->connectToHost(QHostAddress(IP), port);
    if(!recvSocket->waitForConnected(10000)) {
        this->quit();
    } else {

    }
    exec();
}

void RecvThread::recv_file() {
    QByteArray ba = recvSocket->readAll();
    total += ba.size();
    file->write(ba);

    if (total >= fileLength) {
        file->close();
        recvSocket->close();
        //delete file;
        //delete recvSocket;
        emit recv_file_success_signals(fileName);
        this->quit();
    }
}
