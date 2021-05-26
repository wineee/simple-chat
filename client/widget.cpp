#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    isLogin = true;
    ui->passwordLineEdit_2->hide();
    pchatList = nullptr;

    // 换个思路，只读是不能编辑，而 QRadioButton 的改变一般是通过鼠标点击完成的，所以只有屏蔽掉鼠标点击事件就可以实现不能编辑
    ui->radioButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->radioButton->setFocusPolicy(Qt::NoFocus);

    // 窗体风格
    this->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
    // 窗体透明
    //setAttribute(Qt::WA_TranslucentBackground);
    QMovie * m_movie;
    // 加载动态图
    m_movie = new QMovie(":/lib/mian.gif"); // 这是需要播放的动态图
    m_movie->setScaledSize(QSize(590, 150));
    // 添加动态图
    ui->topLabel->setMovie(m_movie);
    // 开始动画
    m_movie->start();

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
    if (pchatList != nullptr)
        delete pchatList;
    delete ui;
}

void Widget::showwidget()
{
    if (pchatList != nullptr)
        pchatList->activateWindow();
    else
        this->show();
}

void Widget::closewidget()
{
     auto res = QMessageBox::information(this, tr("确认操作"), tr("确定要退出吗？"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
     if (res == QMessageBox::Yes)
        exit(0);
}

//~~~~~~~~~~~~~~ 界面移动 ~~~~~~~~~~~~~~~~~~~
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
//~~~~~~~~~~~~~~~ 界面移动结束 ~~~~~~~~~~~~~~~~

void Widget::connect_success() {
    ui->radioButton->setText(tr("服务器连接成功"));
    ui->radioButton->setChecked(true);
}

// 服务器的回复
void Widget::server_reply() {
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();

    QString cmd = obj.value("cmd").toString();
    if (cmd == "register_reply") {
        client_register_handler(obj.value("result").toString());//注册的处理函数
    } else if (cmd == "login_reply") {
        client_login_handler(obj.value("result").toString(),
                             obj.value("friend").toString(),
                             obj.value("group").toString());
    }
}

void Widget::on_toolButton_m_clicked() {
    this->hide();
}

void Widget::on_toolButton_x_clicked() {
    this->close();
    exit(0);
}

void Widget::on_changeButton_clicked() {
    if (isLogin) {
        isLogin = false;
        ui->LgRgButton->setText(tr("注册"));
        ui->changeButton->setText(tr("已有账号"));
        ui->passwordLineEdit_2->show();
    } else {
        isLogin = true;
        ui->LgRgButton->setText(tr("登录"));
        ui->changeButton->setText(tr("还没账号"));
        ui->passwordLineEdit_2->hide();
    }
}

void Widget::on_LgRgButton_clicked()
{
    if (isLogin) {
        QString username = ui->userLineEdit->text();
        QString password = ui->passwordLineEdit->text();

        QJsonObject obj;
        obj.insert("cmd","login");
        obj.insert("user",username);
        obj.insert("passwd",password);

        userName = username;

        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    } else {
        // 完成注册功能
        QString username = ui->userLineEdit->text();
        QString password = ui->passwordLineEdit->text();
        QString password2 = ui->passwordLineEdit_2->text();
        if (password.size() == 0) {
            QMessageBox::warning(this, tr("警告"), tr("密码不能为空！"));
            return;
        }
        if (password != password2) {
            QMessageBox::warning(this, tr("警告"), tr("输入密码不一致！"));
            return;
        }
        QJsonObject obj;
        obj.insert("cmd", "register");
        obj.insert("user", username);
        obj.insert("passwd", password);

        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
}


void Widget::client_register_handler(QString res) {
    if(res == "success") { // 服务器返回 register_reply:success
        QMessageBox::information(this, tr("注册提示"), tr("注册成功"));
        if (!isLogin)
            on_changeButton_clicked();
    }
    else if(res == "failure") {
        QMessageBox::information(this, tr("注册提示"), tr("注册失败")); // 已经存在该用户
    }
}

void Widget::client_login_handler(QString res,QString fri,QString group) {
    if (res == "user_not_exist") {
        QMessageBox::warning(this, tr("登录提示"), tr("用户不存在"));
    } else if (res == "passwd_error") {
        QMessageBox::warning(this, tr("登录提示"), tr("密码错误"));
    } else if (res == "success") {
        // 跳到登录成功的界面
        this->hide();
        socket->disconnect(SIGNAL(readyRead()));
        pchatList = new Chatlist(socket, fri, group, userName);
        pchatList->setWindowTitle(userName);
        pchatList->show();
    } else {
        QMessageBox::warning(this, tr("登录错误"), res);
    }
}
