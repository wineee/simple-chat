#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    socket = new QTcpSocket;
    socket->connectToHost(QHostAddress("39.108.144.134"), 8000); // ip地址和端口号

    connect(socket, &QTcpSocket::connected, this, &Widget::connect_success);
    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::connect_success()
{
    QMessageBox::information(this, "链接提示", "链接服务器成功");
}

// 完成注册功能
void Widget::on_registerButton_clicked()
{
    QString username = ui->userLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd", "register");
    obj.insert("user", username);
    obj.insert("passwd", password);

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

// 服务器的回复
void Widget::server_reply() {
    // socket对象有数据可读的时候出发槽函数
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();

    QString cmd = obj.value("cmd").toString();
    if (cmd == "register_reply") {
        client_register_handler(obj.value("result").toString());//注册的处理函数
    } else if (cmd == "login_reply") {
        //client_login_handler(obj.value("result").toString(),obj.value("friend").toString(),obj.value("Group").toString());
        QString str = obj.value("group").toString();
        QMessageBox::warning(this,"群列表里面的东西显示一下",str);    //这个地方是空的，不知道为什么啊

        client_login_handler(obj.value("result").toString(),obj.value("friend").toString(),obj.value("group").toString());
        //QString str = QString(obj.value("group").toString());
    }
}

void Widget::client_register_handler(QString res) {
    if(res =="success") {//服务器返回success
        QMessageBox::information(this,"注册提示","注册成功");
    }
    else if(res=="failure") {
        QMessageBox::information(this,"注册提示","注册失败");//已经存在该用户
    }
}

void Widget::on_loginButton_clicked() {
    QString username = ui->userLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd","login");
    obj.insert("user",username);
    obj.insert("passwd",password);

    userName = username;

    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Widget::client_login_handler(QString res,QString fri,QString group)
{
    if (res == "user_not_exist") {
        QMessageBox::warning(this,"登录提示","用户不存在");
    } else if (res == "passwd_error") {
        QMessageBox::warning(this,"登录提示","密码错误");
    } else if (res == "success") {
        //跳到登录成功的界面
        this->hide();
        socket->disconnect(SIGNAL(readyRead()));
        Chatlist * c = new Chatlist(socket, fri, group, userName);
        c->setWindowTitle(userName);
        c->show();
    } else {
        QMessageBox::warning(this,"登录错误",res);
    }
}
