#include "groupchat.h"
#include "ui_groupchat.h"

GroupChat::GroupChat(QTcpSocket *s, QString g, QString u, Chatlist *c, QList<groupWidgetInfo> *l, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupChat)
{
    ui->setupUi(this);
    socket = s;
    groupName = g;
    userName = u;
    mainWidget = c;
    groupWidgetList = l;

    QJsonObject obj;
    obj.insert("cmd", "get_group_member");
    obj.insert("group", groupName);
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    connect(mainWidget, &Chatlist::signal_to_sub_widget_member, this, &GroupChat::show_group_member);
    connect(mainWidget, &Chatlist::signal_to_sub_widget_group, this, &GroupChat::show_group_text);
}

GroupChat::~GroupChat()
{
    delete ui;
}

void GroupChat::show_group_member(QJsonObject obj) {
    if (obj.value("cmd").toString() == "get_group_member_reply") {
        if (obj.value("group").toString() == groupName){
            // 得到的是带有|的字符串
            QStringList strList = obj.value("member").toString().split("|");
            for (int i = 0; i < strList.size(); i++) {
                if (strList.at(i).size() == 0) continue;
                QTableWidgetItem *pusr = new QTableWidgetItem(strList.at(i));
                ui->tableWidget->insertRow(0);
                ui->tableWidget->setItem(0, 0, pusr);
            }
        }
    }
}

void GroupChat::on_sendButton_clicked() {
    QString text = ui->inputEdit->toPlainText();
    QJsonObject obj;
    obj.insert("cmd", "group_chat");
    obj.insert("user", userName);
    obj.insert("group", groupName);
    obj.insert("text", text);

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    ui->inputEdit->clear();

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit->setTextColor(Qt::blue);
    ui->textEdit->append("[" + userName + "] at " + time);
    ui->textEdit->setTextColor(Qt::black);
    ui->textEdit->append(text);
}

void GroupChat::show_group_text(QJsonObject obj) {
    // 有可能同时打开多个群聊窗口，收到但是不用都显示
    if (obj.value("cmd").toString() == "group_chat_reply") {
        if (obj.value("group").toString() == groupName) {
            if (this->isMinimized()) {
                this->showNormal();
            }
            this->activateWindow();

            QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            ui->textEdit->setTextColor(Qt::blue);
            ui->textEdit->append("[" + obj.value("user").toString() + "] at " + time);
            ui->textEdit->setTextColor(Qt::black);
            ui->textEdit->append(obj.value("text").toString());
        }
    }
}

void GroupChat::closeEvent(QCloseEvent *event) {
    for(int i=0; i < groupWidgetList->size(); i++) {
        if(groupWidgetList->at(i).name == groupName) {
            groupWidgetList->removeAt(i);
        }
    }
    event->accept();
}

void GroupChat::on_fileButton_clicked()
{
    //QString fileName =
}
