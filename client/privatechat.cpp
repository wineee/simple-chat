#include "privatechat.h"
#include "ui_privatechat.h"

PrivateChat::PrivateChat(QTcpSocket *s, QString u, QString f, Chatlist *c, QList<ChatWidgetInfo> *l, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
    socket = s;
    userName = u;
    friendName = f;
    mainWidget = c;
    chatWidgetList = l;

    connect(mainWidget, &Chatlist::signal_to_sub_widget, this, &PrivateChat::show_text_slot);    
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::on_sendButton_clicked() {
    QString text = ui->inputEdit->toPlainText();
    QJsonObject obj;
    obj.insert("cmd", "private_chat");
    obj.insert("user_from", userName);
    obj.insert("user_to", friendName);
    obj.insert("text", text);
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    ui->inputEdit->clear();

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit->setTextColor(Qt::blue);
    ui->textEdit->append("[" + userName  + "] at " + time);
    ui->textEdit->setTextColor(Qt::black);
    ui->textEdit->append(text);
}

void PrivateChat::show_text_slot(QJsonObject obj) {
    if (obj.value("cmd").toString() == "private_chat_recv") {
        if (obj.value("user_from").toString() == friendName) {
            if (this->isMinimized()) {
               this->showNormal();
            }
            this->activateWindow(); // 显示在最上面

            QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            ui->textEdit->setTextColor(Qt::blue);
            ui->textEdit->append("[" + friendName  + "] at " + time);
            ui->textEdit->setTextColor(Qt::black);
            ui->textEdit->append(obj.value("text").toString());
        }
    }
}

void PrivateChat::closeEvent(QCloseEvent *event) {
    for (int i = 0; i < chatWidgetList->size(); i++) {
        if (chatWidgetList->at(i).name == friendName) {
            chatWidgetList->removeAt(i);
            break;
        }
    }
    event->accept(); // 确实关闭窗口
}

void PrivateChat::on_fileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("发送文件"), QCoreApplication::applicationFilePath());
    if(fileName.isEmpty()) {
        QMessageBox::warning(this, tr("发送文件提示"), tr("未选择文件"));
    } else {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QJsonObject obj;
        obj.insert("cmd", "send_file");
        obj.insert("from_user", userName);
        obj.insert("to_user", friendName);
        obj.insert("length", file.size());
        obj.insert("filename", fileName);
        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
}
