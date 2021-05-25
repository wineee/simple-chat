#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPoint>
#include <QMouseEvent>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QMovie>

#include "chatlist.h"
#include "trayiconmenu.h"


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void connect_success();
    void server_reply();

private:
    void client_register_handler(QString);
    void client_login_handler(QString, QString, QString);
    Ui::Widget *ui;
    Chatlist *pchatList;
    QTcpSocket *socket;
    QString userName;

    // 窗口拖动
    bool isPressedWidget;
    QPoint last;
    void mousePressEvent(QMouseEvent *event); // 鼠标点击
    void mouseMoveEvent(QMouseEvent *event); // 鼠标移动
    void mouseReleaseEvent(QMouseEvent *event); // 鼠标释放

    // 托盘
    QSystemTrayIcon *systemtrayicon;
    QMenu *menu;
    QAction *m_pShowAction;
    QAction *m_pCloseAction;

    bool isLogin;

private slots:
    void showwidget();
    void closewidget();
    void on_toolButton_m_clicked();
    void on_toolButton_x_clicked();
    void on_changeButton_clicked();
    void on_LgRgButton_clicked();
};

#endif // WIDGET_H
