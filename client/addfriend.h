#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

//群的课程的前几节几乎都有修改服务器内容，注意一下

namespace Ui {
class Addfriend;
}

class Addfriend : public QWidget
{
    Q_OBJECT

public:
    explicit Addfriend(QTcpSocket *s,QString u,QWidget *parent = 0);
    ~Addfriend();

private slots:
    void on_cancelButton_clicked();
   // void on_pushButton_2_clicked();
    void on_addButton_clicked();

private:
    Ui::Addfriend *ui;
    QTcpSocket *socket;
    QString userName;
};

#endif // ADDFRIEND_H
