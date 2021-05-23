#include "sendthread.h"

SendThread::SendThread(QJsonObject obj)
{
       //要解析的数据
       port = obj.value("port").toInt();
       fileName = obj.value("filename").toString();
       fileLength = obj.value("length").toInt();
}

void SendThread::run() {
    QTcpSocket sendSocket;
    sendSocket.connectToHost(QHostAddress("39.108.144.134"), port);
    if(!sendSocket.waitForConnected(10000)) {   //超过10秒
        this->quit();   //退出线程
    }
    else{
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);   //fielNAme是个路径
        while(true) {
              QByteArray  ba = file.read(1024); //1024个字节
              if(ba.size() == 0)
                  break;
              sendSocket.write(ba);
              sendSocket.flush(); //立马发过去
        }
        file.close();
        sendSocket.close();
        this->quit();
    }
}
