# libevent



**libevent简介**

Libevent是基于 Reactor 模式的网络库，有几个显著的亮点：
- 事件驱动（event-driven），高性能;
- 轻量级，专注于网络；
- 跨平台，支持Windows、Linux、*BSD和Mac Os；
- 支持多种I/O多路复用技术， epoll、poll、dev/poll、select和kqueue等；
- 支持I/O，定时器和信号等事件；
- 注册事件优先级

----

Reactor是一种事件驱动机制

和普通函数调用的不同之处在于：应用程序不是主动的调用某个API完成处理，而是恰恰相反，Reactor逆置了事件处理流程，应用程序需要提供相应的接口并注册到Reactor上，如果相应的时间发生，Reactor将主动调用应用程序注册的接口，这些接口又称为“回调函数”。


Libevent也是向Libevent框架注册相应的事件和回调函数；当这些事件发生时，Libevent会调用这些回调函数处理相应的事件（I/O读写、定时和信号）。

---

## Reactor模式几个组件

### 1） 事件源

Linux上是文件描述符fd；程序在指定的事件源上注册关心的事件，比如I/O事件。
文件描述符（file descriptor），就是用来表示输入输出对象的整数，例如打开的文件以及网络通信用的套接字等。

对应到 libevent 中的 bufferevent

### 2） event demultiplexer——事件多路分发机制

由操作系统提供的I/O多路复用机制，比如select和epoll。
程序首先将事件源及其事件注册到event demultiplexer上；
当有事件到达时，event demultiplexer会发出通知“在已经注册的句柄集中，一个或多个句柄的事件已经就绪”；
程序收到通知后，就可以在非阻塞的情况下对事件进行处理了。

对应到libevent中，依然是select、poll、epoll等，但是libevent使用结构体eventop进行了封装，以统一的接口来支持这些I/O多路复用机制，达到了对外隐藏底层系统机制的目的。

### select

用单线程来处理多个连接的时候，像这种等待输入时的运行中断（被称为阻塞）是致命的。为了避免阻塞，在读取数据前必须先检查文件描述符中的输入是否已经到达并可用。

于是，在UNIX系操作系统中，对多个文件描述符，可以使用一个叫做select的系统调用来监视它们是否处于可供读写的状态。select系统调用将要监视的文件描述符存入一个fd_set结构体，并设定一个超时时间，对它们的状态进行监视。当指定的文件描述符变为可读、可写、发生异常等状态，或者经过指定的超时时间时，该调用就会返回。之后，通过检查fd_set，就可以得知在指定的文件描述符之中，发生了怎样的状态变化。

---

select系统调用能够监视的文件描述符数量是有上限的，这个上限定义在宏FD_SETSIZE中，虽然因操作系统而异，但一般是在1024个左右。

为了通过fd_set得知到底哪些文件描述符已经处于可用状态，必须每次都将监视中的文件描述符全部检查一遍。当需要监视的文件描述符越来越多时，这种小开销累积起来，也会引发大问题。

---

### epoll

epoll功能是由epoll_create、epoll_ctl和epoll_wait这三个系统调用构成的。

先通过epoll_create系统调用创建监视描述符，该描述符用于代表要监视的文件描述符,然后通过epoll_ctl将监视描述符进行注册，再通过epoll_wait进行实际的监视

和select系统调用相比，epoll的优点如下：
- 要监视的fd数量没有限制
- 只返回产生事件的fd的信息，因此无需遍历所有的fd，性能能够得到改善。

---

### 3） Reactor——反应器

- 是事件管理的接口，内部使用event demultiplexer注册、注销事件；
- 运行事件循环，当有事件进入“就绪”状态时，调用注册事件的回调函数处理事件。

对应到libevent中，就是event_base结构体。

### 4） Event Handler——事件处理程序

事件处理程序提供了一组接口，每个接口对应了一种类型的事件，供Reactor在相应的事件发生时调用，执行相应的事件处理。通常它会绑定一个有效的句柄。

对应到libevent中，就是event结构体。

---

　　在 Reactor 模式中，通常都有一个事件循环(Event Loop)，在 Libevent 中，这个事件循环就是`event_base`结构体：

```cpp
struct event_base *event_base_new(void);           // 创建事件循环
void event_base_free(struct event_base *base);     // 销毁事件循环
int event_base_dispatch(struct event_base *base);  // 运行事件循环
```



------

　　Libevent 使用`event`结构体来代表事件，可以使用`event_new()`创建一个事件：

```cpp
struct event *event_new(struct event_base *base, // 事件循环
                        evutil_socket_t fd,      // 文件描述符
                        short what,              // 事件类型
                        event_callback_fn cb,    // 回调函数
                        void *arg);              // 传递给回调函数的参数
```

　　创建一个事件之后，要怎么把它加入到事件循环呢？可以使用`event_add()`函数：

```cpp
int event_add(struct event *ev,             // 事件
              const struct timeval *tv);    // 超时时间
```

---

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC/img/c3aa71ccd7398b55c4b7b9508b88a130.png)



![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/6c1ab8b82ca70e4a4f4bed5e77b66af9.png)

#  server_addr

struct sockaddr_in {
short int sin_family;  //指代协议族，在socket编程中只能是AF_INET          
unsigned short int sin_port; //存储端口号（使用网络字节顺序）
struct in_addr sin_addr;  //存储IP地址
unsigned char sin_zero[8]; // 是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节。
};



Libevent 使用`evconnlistener`结构来表示 TCP Server，创建 TCP Server 的做法很简单：

```cpp
struct evconnlistener *evconnlistener_new_bind(
    struct event_base *base,        // 事件循环
    evconnlistener_cb cb,           // 回调函数，当 accept() 成功时会被调用
    void *arg,                      // 传递给回调函数的参数
    unsigned flags,                 // 选项
    const struct sockaddr *sa,      // 地址
    int socklen
);

void evconnlistener_free(struct evconnlistener *lev);
```

---

　调用`evconnlistener_new_bind()`函数之后，listening socket 会自动被设置成非阻塞的。我们还通过`flags`参数设置一些有用的选项，例如：

- `LEV_OPT_CLOSE_ON_FREE`表示当调用`evconnlistener_free()`时，相应的 listening socket 也会被`close()`掉。
- `LEV_OPT_REUSEABLE`表示会自动对 listening socket 设置`SO_REUSEADDR`这个 TCP 选项。(一般来说，一个端口释放后会等待两分钟之后才能再被使用，SO_REUSEADDR是让端口释放后立即就可以被再次使用。)

---

　下面的程序创建了一个简单的 TCP Server:

```cpp
Server::Server(const char *ip, int port) {
  base = event_base_new();

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET; 
  server_addr.sin_port = htons(port); // htons 将主机的无符号短整形数转换成网络字节顺序
  server_addr.sin_addr.s_addr = inet_addr(ip);// inet_addr 点分十进制的IP转换成一个长整数型数（u_long类型）
  
  listener = evconnlistener_new_bind(base, listener_cb, NULL,
				     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (listener == nullptr) {
    cout << "listener is null!" << endl;
  }

  event_base_dispatch(base);
}
```
```cpp

// 客户端发起连接时，触发该函数
void Server::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg) {
  // 创建工作线程来处理该客户端
  thread client_thread(client_handler, fd);
  client_thread.detach();  
}
void Server::client_handler(evutil_socket_t fd) {
  struct event_base *base = event_base_new();  // 创建集合
  struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  if (bev == nullptr) {
    cout << "bufferevent_socket_new error" << endl;
  }
  // 给 bufferevent 设置回调函数
  bufferevent_setcb(bev, read_cb, nullptr, event_cb, nullptr);
  // 使可以回调函数
  bufferevent_enable(bev, EV_READ);
  event_base_dispatch(base); // 监听客户端是否有数据连接
  event_base_free(base);
  cout << "exit" << "\n";
}
```

```cpp
void Server::read_cb(struct bufferevent *bev, void *ctx) {
  char buf[1024] = {0};
  int size = bufferevent_read(bev, buf, sizeof(buf));
  if (size < 0) {
    cout << "bufferevent_read Error!" << endl;
  }
  ...
}
```

# QTcpSocket

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/6e3037e8ae661a4bd547fde196db814c.png)

``` cpp
 
socket->connectToHost(QHostAddress("39.108.144.134"), 8000); // ip地址和端口号
connect(socket, &QTcpSocket::connected, this, &Widget::connect_success);
connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply);

QByteArray ba = socket->readAll();

socket->write(ba); 

```



# 数据库组织结构

create database user;
create database chatgroup;

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/9e2d67ce816e413ae3450ee9d6cffd74.png)

![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/e59946fe114d97e45a38787bdcbe6d72.png)

# 内存组织结构

## ![wishimg](https://cdn.jsdelivr.net/gh/wineee/MarkDownPIC@master/img/316f85cb9ac9e0eed3855238565ba75a.png)



 注册

   客户端发送： {“cmd”:"register", "user":"LiHua", "passwd":"11111"}
   成功返回： {“cmd”:"register_reply", "result":"success"};
   失败返回： {"cmd":"register_reply", "result":"failure"};

登陆

   客户端发送： {“cmd”:"login", "user":"LiHua", "passwd":"11111"}
   用户不存在返回： {"cmd":"login_reply", "result":"user_not_exist"};
   密码错误返回： {"cmd":"login_reply", "result":"passwd_error"};
   成功返回： {“cmd”:"login_reply", "result": "success", fri："好友列表"" "group":"群列表"};

私聊

   客户端发送： {“cmd”：“private_chat”, "user_from":"", "user_to":"", "text":""}
   对方不在线： {"cmd": "private_chat_reply", "result":"offline"}
   对方在线 服务器转发: {"cmd": "private_chat_recv", "user_from","", text:""}
   对方在线 服务器回复： {"cmd":"private_chat_reply", "result":"success"}

添加好友，创建群，加入群，群聊...
