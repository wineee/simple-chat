#include "widget.h"
#include "ui_widget.h"

#define IP "39.108.144.134"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 窗体风格
    this->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
    // 窗体透明
    //setAttribute(Qt::WA_TranslucentBackground);

    // 初始化系统托盘
    systemtrayicon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/lib/chat.png");
    systemtrayicon->setIcon(icon);

    menu = new QMenu(this);
    m_pShowAction = new QAction("打开主界面");
    m_pCloseAction = new QAction("退出");
    menu->addAction(m_pShowAction);
    menu->addSeparator();
    menu->addAction(m_pCloseAction);
    connect(m_pShowAction, SIGNAL(triggered(bool)), this, SLOT(showwidget()));
    connect(m_pCloseAction, SIGNAL(triggered(bool)), this, SLOT(closewidget()));
    systemtrayicon->setContextMenu(menu);
    systemtrayicon->show();

    socket = new QTcpSocket;
    socket->connectToHost(QHostAddress(IP), 8000); // ip地址和端口号

    connect(socket, &QTcpSocket::connected, this, &Widget::connect_success);
    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showwidget()
{
    this->show();
}

void Widget::closewidget()
{
    this->close();
}


// 界面移动
void Widget::mousePressEvent(QMouseEvent *event) {
    isPressedWidget = true; // 当前鼠标按下的即是QWidget而非界面上布局的其它控件
    last = event->globalPos();
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (isPressedWidget) {
        int dx = event->globalX() - last.x();
        int dy = event->globalY() - last.y();
        last = event->globalPos();
        move(x()+dx, y()+dy);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event) {
    int dx = event->globalX() - last.x();
    int dy = event->globalY() - last.y();
    move(x()+dx, y()+dy);
    isPressedWidget = false; // 鼠标松开时，置为false
}
// 界面移动结束

void Widget::connect_success()
{
    QMessageBox::information(this, "连接提示", "连接服务器成功");
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

void Widget::client_register_handler(QString res) {
    if(res =="success") { // 服务器返回register_reply:success
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

void Widget::client_login_handler(QString res,QString fri,QString group) {
    if (res == "user_not_exist") {
        QMessageBox::warning(this,"登录提示","用户不存在");
    } else if (res == "passwd_error") {
        QMessageBox::warning(this,"登录提示","密码错误");
    } else if (res == "success") {
        // 跳到登录成功的界面
        this->hide();
        socket->disconnect(SIGNAL(readyRead()));
        Chatlist *c = new Chatlist(socket, fri, group, userName);
        c->setWindowTitle(userName);
        c->show();
    } else {
        QMessageBox::warning(this,"登录错误",res);
    }
}

// 服务器的回复
void Widget::server_reply() {
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();

    QString cmd = obj.value("cmd").toString();
    if (cmd == "register_reply") {
        client_register_handler(obj.value("result").toString());//注册的处理函数
    } else if (cmd == "login_reply") {
        QString str = obj.value("group").toString();
        client_login_handler(obj.value("result").toString(),
                             obj.value("friend").toString(),
                             obj.value("group").toString());
    }
}

void Widget::on_toolButton_m_clicked()
{
    this->hide();
}

void Widget::on_toolButton_2_clicked()
{
    this->close();
}
