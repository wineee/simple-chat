#-------------------------------------------------
#
# Project created by QtCreator 2021-04-18T21:37:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chat_soft
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    trayiconmenu.cpp \
    widget.cpp \
    chatlist.cpp \
    addfriend.cpp \
    creategroup.cpp \
    addgroup.cpp \
    privatechat.cpp \
    groupchat.cpp \
    sendthread.cpp \
    recvthread.cpp

HEADERS += widget.h \
    chatlist.h \
    addfriend.h \
    creategroup.h \
    addgroup.h \
    privatechat.h \
    groupchat.h \
    sendthread.h \
    recvthread.h \
    trayiconmenu.h

FORMS += widget.ui \
    chatlist.ui \
    addfriend.ui \
    creategroup.ui \
    addgroup.ui \
    privatechat.ui \
    groupchat.ui

RESOURCES += \
    Resources.qrc

DISTFILES += \
    lib/036428b265240e27035db396a284521a.gif \
    lib/10001.jpg \
    lib/back.gif \
    lib/background.png \
    lib/close.png \
    lib/delete.png \
    lib/delete2.png \
    lib/dengji.png \
    lib/fdog.png \
    lib/fdogicon.png \
    lib/guanbi.png \
    lib/icon2505897252.png \
    lib/jianpan.png \
    lib/jietou.png \
    lib/kaifdog.png \
    lib/kaisuo.png \
    lib/likaishijian.png \
    lib/lixianim.png \
    lib/mainicon.png \
    lib/mainicon2.png \
    lib/manglu.png \
    lib/max.png \
    lib/mian.gif \
    lib/min.png \
    lib/qusefdogicon.png \
    lib/sangangy.png \
    lib/search.png \
    lib/skin.png \
    lib/suo.png \
    lib/svip.png \
    lib/tianjiahaoyou.png \
    lib/wurao.png \
    lib/xiaolian.png \
    lib/yinshenim.png \
    lib/zaixian.png \
    lib/zuixiaohua.png
