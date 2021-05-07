#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QFileDialog>
#include <QCoreApplication>   //获取这个文件的路径
#include <QFile>    //操作文件离不开这个
#include <QCloseEvent>

class Chatlist;
struct ChatWidgetInfo;
#include "chatlist.h"   //互相包含

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT
public:
    explicit PrivateChat(QTcpSocket * s,QString u,QString f,Chatlist * c,QList<ChatWidgetInfo>* l,QWidget *parent = 0);
    ~PrivateChat();
    void closeEvent(QCloseEvent * event);

private slots:
    void on_sendButton_clicked();
    void show_text_slot(QJsonObject);

    //void on_pushButton_2_clicked();

    void on_fileButton_clicked();

private:
    Ui::PrivateChat *ui;
    QTcpSocket * socket;
    QString userName;
    QString friendName;
    Chatlist * mainWidget;
    QList<ChatWidgetInfo> * chatWidgetList;

};

#endif // PRIVATECHAT_H
