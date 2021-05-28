# simple-chat


## Yet another Chat software, support private chat and group chat, based on libevent

## 客户端

> 使用 Qt 5.15，理论上在 linux 和 windows 下都可以编译运行

#### 注册界面

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/0807b54b8f4625a37aaffedf4b8eb9d0.png)

#### 登录界面

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/23929ebbcb61e4b9eb3d575f989eccfc.png)

#### 私聊

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/75fe8ef4a354026df356123a27c12cff.png)

#### 群聊

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/2fb9182d2b251b46e7abc4a1c8bf25e5.png)

#### 传文件

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/d72d87d1e66f3511dfc5ffb797b2b531.png)

参考资料(事实上参考资料的界面更完善)：[Fdog系列（四）：使用Qt框架模仿QQ实现登录界面，界面篇](https://blog.csdn.net/Fdog_/article/details/115864249)
[QtQQ](https://github.com/Blackmamba-xuan/QtQQ)

## 服务端

> 使用阿里云  Ubuntu 20.04.2 系统

```bash
apt-get install libevent-dev libjsoncpp-dev
apt-get install mysql-server mysql-client libmysqlclient-dev 
```

[阿里云添加安全组规则](https://www.jb51.net/article/190342.htm)

---

参考教程：https://www.bilibili.com/video/BV19U4y1p7pT

### [放上一点可供参考的笔记](https://github.com/wineee/simple-chat/blob/main/libevent.md)

> *虽然还有很多不完善的地方，比如客户端存储记录等等，不过主要思路已经实现了，以后可能不会积极更新*
