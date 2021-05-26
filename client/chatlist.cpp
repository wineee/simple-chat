#include "chatlist.h"
#include "ui_chatlist.h"

Chatlist::Chatlist(QTcpSocket *s, QString fri, QString group, QString u, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chatlist)
{
    ui->setupUi(this);
    socket = s;
    userName = u;
    connect(socket, &QTcpSocket::readyRead, this, &Chatlist::server_reply);

    // 初始化好友列表和群列表
    QStringList friList = fri.split('|'); // 根据|去解析
    for (int i = 0; i < friList.size(); i++) {
        if (friList.at(i) != "")
            ui->friendList->addItem(friList.at(i));
    }

    QStringList groList = group.split('|');
    for (int i = 0; i < groList.size(); i++) {
        if (groList.at(i) != "")
            ui->groupList->addItem(groList.at(i));
    }

    connect(ui->friendList, &QListWidget::itemDoubleClicked, this, &Chatlist::on_frientList_double_clicked);
    connect(ui->groupList, &QListWidget::itemDoubleClicked, this, &Chatlist::on_groupList_double_clicked);
}

Chatlist::~Chatlist()
{
    delete ui;
}

void Chatlist::server_reply() {
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();

    do {
        if (cmd == "friend_login") {
           client_login_reply(obj.value("friend").toString());
           break;
        }
        if (cmd == "add_reply") {
            client_add_friend_reply(obj);
            break;
        }
        if (cmd == "add_friend_reply") {
            QString str = QString("%1把你添加为好友").arg(obj.value("result").toString());
            QMessageBox::information(this, "添加好友提醒", str);
            ui->friendList->addItem(obj.value("result").toString());
            break;
        }
        if (cmd == "create_group_reply") {
            client_create_group_reply(obj);
            break;
        }
        if (cmd == "add_group_reply") {
            client_add_group_reply(obj);
            break;
        }
        if (cmd == "private_chat_reply") {
            client_private_chat_reply(obj.value("result").toString());
            break;
        }
        if (cmd == "private_chat_recv") {
            client_chat_reply(obj);
            break;
        }
        if (cmd == "get_group_member_reply") {
            client_get_group_member_reply(obj);
            break;
        }
        if (cmd == "group_chat_reply") {
            client_group_chat_reply(obj);
            break;
        }
        if (cmd == "friend_offline") {
            client_friend_offline(obj.value("friend").toString());
            break;
        }
        if (cmd == "send_file_reply") {
            client_send_file_reply(obj.value("result").toString());
            break;
        }
        if (cmd == "send_file_port_reply") {
            client_send_file_port_reply(obj);
            break;
        }
        if (cmd == "recv_file_port_reply") {
            client_recv_file_port_reply(obj);
            break;
        }
    } while(0);
}

void Chatlist::client_login_reply(QString fri) {
    QString str = QString("%1好友上线").arg(fri);
    QMessageBox::information(this,"好友上线提醒",str);
}

void Chatlist::client_add_friend_reply(QJsonObject & obj){
    if(obj.value("result").toString() == "user_not_exist") {
        QMessageBox::warning(this, "添加好友提醒", "好友不存在");
    } else if (obj.value("result").toString() == "already_friend") {
        QMessageBox::warning(this, "添加好友提醒", "已经是好友");
    } else if (obj.value("result").toString() == "success") {
        QMessageBox::warning(this, "添加好友提醒", "好友添加成功");
        ui->friendList->addItem(obj.value("friend").toString());
    }
}

void Chatlist::client_create_group_reply(QJsonObject &obj) {
    if (obj.value("result").toString() == "group_exist") {
        QMessageBox::warning(this, "创建群提示", "群已经存在");
    } else if (obj.value("result").toString() == "success") {
        QString str = QString(obj.value("group").toString());
        QMessageBox::warning(this, "提示", str);
        ui->groupList->addItem(obj.value("group").toString());
    }
}

void Chatlist::client_add_group_reply(QJsonObject &obj) {
    if (obj.value("result").toString() == "group_not_exist") {
        QMessageBox::warning(this, "添加群提示", "群不存在");
    } else if (obj.value("result").toString() == "user_in_group") {
        QMessageBox::warning(this, "添加群提示", "已经在群里面");
    } else if (obj.value("result").toString() == "success") {
        ui->groupList->addItem(obj.value("group").toString());
    }
}

void Chatlist::client_private_chat_reply(QString res) {
    if (res == "offline") {
        QMessageBox::warning(this, "发送提醒", "对方不在线");
    }
}

void Chatlist::client_chat_reply(QJsonObject &obj) {
    // 当收到消息时，自动跳出一个窗口或者已经打开了
    // 看一下是谁发来的，先判断一下这个窗口有没有被打开
    int flag = 0;
    for (int i = 0; i < chatWidgetList.size(); i++) {
        if (chatWidgetList.at(i).name == obj.value("user_from").toString()) {
            flag = 1;
            break;
        }
    }
    if (flag == 0) { // 聊天窗口没有打开
        QString friendName = obj.value("user_from").toString();
        PrivateChat *privateChatWidget = new PrivateChat(socket, userName, friendName, this, &chatWidgetList);   //需要对应该一下构造函数
        privateChatWidget->setWindowTitle(friendName);
        privateChatWidget->show();
        ChatWidgetInfo c = {privateChatWidget, friendName};
        chatWidgetList.push_back(c);
    }
    emit signal_to_sub_widget(obj);
}

void Chatlist::client_get_group_member_reply(QJsonObject obj) {
    emit signal_to_sub_widget_member(obj);
}

void Chatlist::client_group_chat_reply(QJsonObject obj) {
    int flag = 0;
    for (int i = 0;i < groupWidgetList.size(); i++) {
        if (groupWidgetList.at(i).name == obj.value("group").toString()) {
            flag = 1;
            break;
        }
    }
    if(flag == 0) {
        QString groupName = obj.value("group").toString();
        GroupChat *groupChatWidget = new GroupChat(socket, groupName, userName, this, &groupWidgetList);
        groupChatWidget->setWindowTitle(groupName);
        groupChatWidget->show();
        groupWidgetInfo g = {groupChatWidget,groupName};
        groupWidgetList.push_back(g);
    }
    emit signal_to_sub_widget_group(obj);
}

void Chatlist::client_send_file_reply(QString res){
    if (res == "offline"){
        QMessageBox::warning(this, "发送文件提醒", "对方不在线");
    } else if (res == "timeout"){
        QMessageBox::warning(this, "发送文件提醒", "链接超时");
    }
}

void Chatlist::client_send_file_port_reply(QJsonObject obj) {
    // 启动线程，自己封装一个类
    SendThread *mySendthread = new SendThread(obj);
    mySendthread->start();
}

void Chatlist::client_recv_file_port_reply(QJsonObject obj) {
    RecvThread *myRecvThread = new RecvThread(obj);
    myRecvThread->start();
}

void Chatlist::client_friend_offline(QString fri) {
    QString str = QString("%1下线").arg(fri);
    QMessageBox::information(this,"下线提醒",str);
}

void Chatlist::on_addButton_clicked() {
    Addfriend *addFriendWidget = new Addfriend(socket,userName);
    addFriendWidget->show();
}

void Chatlist::on_createGroupButton_clicked() {
    CreateGroup *createGroupWidget = new CreateGroup(socket,userName);
    createGroupWidget->show();
}

void Chatlist::on_addGroupButton_clicked() {
    AddGroup * addGroupWidget =new AddGroup(socket,userName);
    addGroupWidget->show();
}

void Chatlist::on_frientList_double_clicked() {
    QString friendName = ui->friendList->currentItem()->text();    // 获取对方好友的名字
    PrivateChat *privateChatWidget = new PrivateChat(socket, userName, friendName, this, &chatWidgetList);
    privateChatWidget->setWindowTitle(friendName);
    privateChatWidget->show();

    ChatWidgetInfo c = {privateChatWidget,friendName};
    chatWidgetList.push_back(c);
}

void Chatlist::on_groupList_double_clicked() {
    QString groupName = ui->groupList->currentItem()->text();
    GroupChat *groupChatWidget = new GroupChat(socket, groupName, userName, this, &groupWidgetList);
    groupChatWidget->setWindowTitle(groupName);
    groupChatWidget->show();

    groupWidgetInfo g = {groupChatWidget, groupName};
    groupWidgetList.push_back(g);
}

void Chatlist::closeEvent(QCloseEvent *event) {
    QJsonObject obj;
    obj.insert("cmd", "offline");
    obj.insert("user", userName);
    QByteArray ba;
    ba = QJsonDocument(obj).toJson();
    socket->write(ba);
    socket->flush();
    //刷新socket缓冲区
    event->accept();
}
