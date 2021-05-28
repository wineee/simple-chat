#ifndef CHATLIST_H
#define CHATLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QList>
#include <QCloseEvent>

#include "addfriend.h"
#include "creategroup.h"
#include "addgroup.h"
#include "privatechat.h"
#include "groupchat.h"

#include "sendthread.h"
#include "recvthread.h"

class PrivateChat;
class GroupChat;

namespace Ui {
class Chatlist;
}

struct ChatWidgetInfo
{
    PrivateChat *w;
    QString name;
};

struct groupWidgetInfo
{
    GroupChat * w;
    QString name;
};

class Chatlist : public QWidget
{
    Q_OBJECT

public:
    explicit Chatlist(QTcpSocket *, QString, QString, QString, QWidget *parent = 0);
    void closeEvent(QCloseEvent *event);
    ~Chatlist();

private slots:
    void server_reply();
    void on_createGroupButton_clicked();
    void on_addButton_clicked();
    void on_addGroupButton_clicked();
    void on_frientList_double_clicked();
    void on_groupList_double_clicked();

    void recv_file_success_noitce(QString fileName);

signals:
    void signal_to_sub_widget(QJsonObject);
    void signal_to_sub_widget_member(QJsonObject);
    void signal_to_sub_widget_group(QJsonObject);

private:
    void client_login_reply(QString);
    void client_add_friend_reply(QJsonObject &obj);
    void client_create_group_reply(QJsonObject &obj);
    void client_add_group_reply(QJsonObject & obj);
    void client_private_chat_reply(QString);
    void client_chat_reply(QJsonObject &obj);
    void client_get_group_member_reply(QJsonObject);
    void client_group_chat_reply(QJsonObject);
    void client_friend_offline(QString fri);
    void client_send_file_reply(QString);
    void client_send_file_port_reply(QJsonObject);
    void client_recv_file_port_reply(QJsonObject);

    Ui::Chatlist *ui;
    QTcpSocket *socket;
    QString userName;
    QList<ChatWidgetInfo> chatWidgetList;
    QList<groupWidgetInfo> groupWidgetList;
};

#endif // CHATLIST_H
