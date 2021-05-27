#include "server.h"

ChatDataBase *Server::chatdb = new ChatDataBase;
ChatInfo *Server::chatlist = new ChatInfo;

Server::Server(const char *ip, int port) {
  base = event_base_new();

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);
  
  listener = evconnlistener_new_bind(base, listener_cb, NULL,
				     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (listener == nullptr) {
    cout << "listener is null!" << endl;
  }

  event_base_dispatch(base);
}

Server::~Server() {
   event_base_free(base);
}

// 客户端发起连接时，触发该函数
void Server::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg) {
  cout << "接受客户端的连接，fd = " << fd << endl;
  // 创建工作线程来处理该客户端
  thread client_thread(client_handler, fd);
  client_thread.detach();  // 线程分离
}

void Server::client_handler(evutil_socket_t fd) {
  // 创建集合
  struct event_base *base = event_base_new();
  struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  if (nullptr == bev) {
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

void Server::read_cb(struct bufferevent *bev, void *ctx) {
  char buf[1024] = {0};
  int size = bufferevent_read(bev, buf, sizeof(buf));
  if (size < 0) {
    cout << "bufferevent_read Error!" << endl;
  }
  cout << buf << endl;

  Json::CharReaderBuilder rbuilder;
  Json::Value val;
  string err;
  std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
  if(reader->parse(buf, buf+strlen(buf), &val, &err) == false) {
    cout << "Json parse Error: " << err << endl;
  }
  
  string cmd = val["cmd"].asString();
  if (cmd == "register") {
    server_register(bev, val);
  } else if (cmd == "login") {
    server_login(bev, val);
  } else if (cmd == "add") {
    server_add_friend(bev, val);
  } else if (cmd == "create_group") {
    server_create_group(bev, val);
  } else if (cmd == "add_group") {
    server_add_group(bev, val);
  } else if (cmd == "private_chat") {
    server_private_chat(bev, val);
  } else if (cmd == "group_chat") {
    server_group_chat(bev, val);
  } else if (cmd == "get_group_member") {
    server_get_group_member(bev, val);
  } else if (cmd == "offline") {
    server_user_offline(bev, val);
  } else if (cmd == "send_file") {
    server_send_file(bev, val);
  }
}

// 注册
/**
   客户端发送： {“cmd”:"register", "user":"LiHua", "passwd":"11111"}
   成功返回： {“cmd”:"register_reply", "result":"success"};
   失败返回： {"cmd":"register_reply", "result":"failure"};
*/
void Server::server_register(struct bufferevent *bev, Json::Value val) {
  chatdb->my_database_connect("user");
  if (chatdb->my_database_user_exist(val["user"].asString())) {
    Json::Value returnVal;
    returnVal["cmd"] = "register_reply";
    returnVal["result"] = "failure";
    Json::StreamWriterBuilder wbuilder;
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  } else {
    chatdb->my_database_create_user_passwd(val["user"].asString(), val["passwd"].asString());
    Json::Value returnVal;
    Json::StreamWriterBuilder wbuilder;
    returnVal["cmd"] = "register_reply";
    returnVal["result"] = "success";
    string b = Json::writeString(wbuilder, returnVal); 
    if (bufferevent_write(bev,  b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
  chatdb->my_database_disconnect();
}

// 登陆
/**
   客户端发送： {“cmd”:"login", "user":"LiHua", "passwd":"11111"}
   失败返回： {"cmd":"login_reply", "result":"user_not_exist"};
   失败返回： {"cmd":"login_reply", "result":"passwd_error"};
   成功返回： {“cmd”:"login_reply", "result":"好友列表"" "group":"群列表"};
   成功返回： {“cmd”:"friend_reply", "friend":fri}
*/
void Server::server_login(struct bufferevent *bev, Json::Value val) {
  chatdb->my_database_connect("user");
  // 用户不存在
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  if (!chatdb->my_database_user_exist(val["user"].asString())) {
    returnVal["cmd"] = "login_reply";
    returnVal["result"] = "user_not_exist";
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }

  // 密码不正确
  else if (!chatdb->my_database_passwd_correct(val["user"].asString(), val["passwd"].asString())) {
    returnVal["cmd"] = "login_reply";
    returnVal["result"] = "passwd_error";
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }

  // 登陆成功
  else {
    // 在线用户链表
    User u = {val["user"].asString(), bev};
    chatlist->online_user->push_back(u);
    string fri_list = chatdb->my_database_get_friend(val["user"].asString());
    string group_list = chatdb->my_database_get_group(val["user"].asString());
    returnVal["cmd"] = "login_reply";
    returnVal["result"] = "success";
    returnVal["friend"] = fri_list;
    returnVal["group"] = group_list;
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }

    // 向好友发送上线通知
    returnVal.clear();
    returnVal["cmd"] = "friend_login";
    returnVal["friend"] = val["user"];
    b = Json::writeString(wbuilder, returnVal);
    string::size_type start = 1, end = 1;
    while (start < fri_list.size()) {
      end = fri_list.find('|', start);
      if (end == string::npos) 
        break;
      string fri = fri_list.substr(start, end-start);
      //cout << start << " " << end << " " << fri << " hhh\n"; 
      struct bufferevent *to_bev = chatlist->info_get_friend_bev(fri);
      if (to_bev != nullptr) {
         cout << fri << " ff \n";
         if (bufferevent_write(to_bev, b.c_str(), b.size()) < 0) {
	         cout << "bufferevent write error" << endl;
         }
      }
      start = end+1;
    }
  }

  chatdb->my_database_disconnect();
}

// 添加好友
/*
客户端发送：{"cmd":"add", "user":"LiHua", "friend":"Wu"}
好友不存在：{"cmd":"add_reply", "result":"user_not_exist"}
重复添加：{"cmd":"add_reply", "result":"already_friend"}
成功，回复自己：{"cmd":"add_reply", "result":"success"}
成功，回复好友：{"cmd":"add_reply", "result":"LiHua"}
*/

void Server::server_add_friend(struct bufferevent *bev, Json::Value val) {
  chatdb->my_database_connect("user");
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  // 好友不存在
  if (!chatdb->my_database_user_exist(val["friend"].asString())) {  
    returnVal["cmd"] = "add_reply";
    returnVal["result"] = "user_not_exist";
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
  // 已经是好友了
  else if (chatdb->my_database_is_friend(val["user"].asString(), val["friend"].asString())) {
    returnVal.clear();
    returnVal["cmd"] = "add_reply";
    returnVal["result"] = "already_friend";
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
  // 添加成功
  else {
    chatdb->my_database_add_new_friend(val["user"].asString(), val["friend"].asString());
    chatdb->my_database_add_new_friend(val["friend"].asString(), val["user"].asString());
    // 回复客户端添加成功
    returnVal.clear();
    returnVal["cmd"] = "add_reply";
    returnVal["result"] = "success";
    returnVal["friend"] = val["friend"];
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
    // 回复对方多了个好友
    struct bufferevent *to_bev = chatlist->info_get_friend_bev(val["friend"].asString());
    if (to_bev == nullptr) {
      cout << "对方不在线！未通知！" << endl;
    }
    else {
      returnVal.clear();
      returnVal["cmd"] = "add_friend_reply";
      returnVal["result"] = val["user"];
      b = Json::writeString(wbuilder, returnVal);
      if (bufferevent_write(to_bev, b.c_str(), b.size()) < 0) {
      	cout << "bufferevent write error" << endl;
      }
    }
  }
  chatdb->my_database_disconnect();
}

void Server::event_cb(struct bufferevent *bev, short what, void *ctx) {
  
}


// 创建群聊
/*
   客户端发送 {"cmd":"create_group", "user":"LiHua", "group":"lala"}
   群已经存在 {"cmd":"create_group_reply", "result":"group_exist"}
   群不存在 {"cmd":"create_group_reply", "result":"success"}
*/

void Server::server_create_group(struct bufferevent *bev, Json::Value val) {
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  chatdb->my_database_connect("chatgroup");
  if (chatdb->my_database_group_exist(val["group"].asString())) {
    returnVal["cmd"] = "create_group_reply";
    returnVal["result"] = "group_exist";
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
  else {
    // 修改数据库个人信息
    chatdb->my_database_add_new_group(val["group"].asString(), val["user"].asString());
    chatdb->my_database_disconnect();
    
    chatdb->my_database_connect("user");
    chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
    //服务器端有一个群列表，修改群列表

    chatlist->info_add_new_group(val["group"].asString(),val["user"].asString());

    returnVal["cmd"] = "create_group_reply";
    returnVal["result"] = "success";
    returnVal["group"] = val["group"];
    string b = Json::writeString(wbuilder, returnVal);
    //cout << b << "\n";
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
  chatdb->my_database_disconnect();
}


////////////////// 添加群
/*
   客户端发送：{"cmd":"add_group", "user":"Li", "group":""}
   群不存在：{"cmd":"add_group_reply", "result":"group_not_exit"};
   已经在群中：{"cmd":"add_group_reply", "result":"user_in_group"}
   添加成功:{"cmd":"add_group_reply", "result":"success"}
*/

void Server::server_add_group(struct bufferevent *bev, Json::Value val) {
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  returnVal["cmd"] = "add_group_reply";
  chatdb->my_database_connect("user");
  // 判断群是否存在
  if (!chatlist->info_group_exist(val["group"].asString())) {
    returnVal["result"] = "group_not_exit";
  }
  // 判断用户是否在群里
  else if (chatlist->info_user_in_group(val["group"].asString(), val["user"].asString())) {
    returnVal["result"] = "user_in_group";
  }
  // 添加成功
  else {
    // 修改数据库（用户表 群表）
    chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
    chatdb->my_database_disconnect();
    chatdb->my_database_connect("chatgroup");
    chatdb->my_database_group_add_user(val["group"].asString(), val["user"].asString());
    // 修改链表
    chatlist->info_group_add_user(val["group"].asString(), val["user"].asString());
    returnVal["result"] = "success";
    returnVal["group"] = val["group"];
  }
  string b = Json::writeString(wbuilder, returnVal);
  if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
    cout << "bufferevent write error" << endl;
  }
  chatdb->my_database_disconnect();
}


// 私聊
/*
   客户端发送： {“cmd”：“private_chat”, "user_from":"", "user_to":"", "text":""}
   对方不在线： {"cmd": "private_chat_reply", "result":"offline"}
   对方在线 服务器转发: {"cmd": "private_chat_recv", "user_from","", text:""}
   对方在线 服务器回复： {"cmd":"private_chat_reply", "result":"success"}
*/

void Server::server_private_chat(struct bufferevent *bev, Json::Value val) {
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  struct bufferevent *to_bev = chatlist->info_get_friend_bev(val["user_to"].asString());
  if (to_bev == nullptr) {
    // 对方不在线
    returnVal["cmd"] = "private_chat_reply";
    returnVal["result"] = "offline";
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
  else {
    // 转发
    returnVal["cmd"] = "private_chat_recv";
    returnVal["text"] = val["text"];
    returnVal["user_from"] = val["user_from"];
    string b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(to_bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
    // 成功
    returnVal.clear();
    returnVal["cmd"] = "private_chat_reply";
    returnVal["result"] = "success";
    b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
      cout << "bufferevent write error" << endl;
    }
  }
}

// 群聊
/*
   客户端发送：{”cmd“:"group_chat", "user":"", "group":"", "text":""}
   服务器转发：{"cmd":"group_chat_reply", "user":"", "group":"", "text":""}
   服务器回复:{"cmd":"group_chat_reply", "result":"success"}
*/

void Server::server_group_chat(struct bufferevent *bev, Json::Value val) {
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  returnVal["cmd"] = "group_chat_reply";
  returnVal["user"] = val["user"];
  returnVal["group"] = val["group"];
  returnVal["text"] = val["text"];
  string b = Json::writeString(wbuilder, returnVal);
  for (auto it = chatlist->group_info->begin(); it != chatlist->group_info->end(); it++) {
    if (val["group"].asString() == it->name) {
      for (auto member = it->l->begin(); member != it->l->end(); member++) {
        cout << member->name << " m \n";
	      struct bufferevent *to_bev = chatlist->info_get_friend_bev(member->name);
	      if (to_bev != nullptr) {
	        if (bufferevent_write(to_bev, b.c_str(), b.size()) < 0) {
	          cout << "bufferevent write error" << endl;
	        }
	      }
      }
    }
  }
  // 回复发送成功
  returnVal.clear();
  returnVal["cmd"] = "group_chat_reply";
  returnVal["result"] = "success";
  b = Json::writeString(wbuilder, returnVal);
  if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
    cout << "bufferevent write error" << endl;
  }
}

// 获取群成员
/*
  客户端发送:{"cmd":"get_group_member""get_group_member", "group":""}
  服务器回复:{"cmd":"get_group_member_reply","member":""}
*/

void Server::server_get_group_member(struct bufferevent *bev, Json::Value val) {
  string member = chatlist->info_get_group_member(val["group"].asString());
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  returnVal["cmd"] = "get_group_member_reply";
  returnVal["member"] = member;
  returnVal["group"] = val["group"];
  string b = Json::writeString(wbuilder, returnVal);
  if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
    cout << "bufferevent write error" << endl;
  }
}


// 用户下线
/*
  客户端发送 {"cmd":"offline", "user":""}
  向好友返回 {"cmd":"friend_offline", "result":"name"}
*/
void Server::server_user_offline(struct bufferevent *bev, Json::Value val) {
  // 从链表中删除用户
  for (auto it = chatlist->online_user->begin(); it != chatlist->online_user->end(); it++) {
    if (it->name == val["user"].asString()) {
      chatlist->online_user->erase(it);
      break;
    }
  }
  
  // 获取好友列表并返回
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  returnVal["cmd"] = "friend_offline";
  returnVal["result"] = val["user"];
  string b = Json::writeString(wbuilder, returnVal);
  chatdb->my_database_connect("user");
  string fri_list = chatdb->my_database_get_friend(val["user"].asString());
  chatdb->my_database_disconnect();
  string::size_type start = 1, end = 1;
  while (start < fri_list.size()) {
    end = fri_list.find('|', start);
    if (end == string::npos) {
      break;
    }
    string fri = fri_list.substr(start, end-start);
    cout << start << " " << end << " " << fri << " hhh\n"; 
    struct bufferevent *to_bev = chatlist->info_get_friend_bev(fri);
    if (to_bev != nullptr) {
      if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
	      cout << "bufferevent write error" << endl;
      }
    }
    start = end+1;
  }
}


// 文件传输
/*
   客户端发送 {"cmd":"send_file", "from_user":"", "to_user":"","filename":"a.txt","length":""}
   如果对方不在线 {"cmd":"send_file_relpy","result":"offline"}
   发送 {"cmd":"send_file_port_reply","port":"8080","filename":"a.txt","length":"100"}
   接受 {"cmd":"recv_file_port_reply","port":"8080","filename":"a.txt","length":"100"}
   超时{"cmd":"send_file_reply","result":"timeout"}
*/
void Server::server_send_file(struct bufferevent *bev, Json::Value val) {
  struct bufferevent *to_bev = chatlist->info_get_friend_bev(val["to_user"].asString());
  Json::Value returnVal;
  Json::StreamWriterBuilder wbuilder;
  string b;
  if (to_bev == nullptr) {
    returnVal["cmd"] = "send_file_reply";
    returnVal["result"] = "offline";
    b = Json::writeString(wbuilder, returnVal);
    if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
	    cout << "bufferevent write error" << endl;
    }
    return;
  }

  // 启动新线程，创建文件服务器
  int port = 8080, from_fd = 0, to_fd = 0;
  thread send_file_thread(send_file_hander, val["length"].asInt(), port, &from_fd, &to_fd);
  send_file_thread.detach();

  // 返回 port 给发送端
  returnVal.clear();
  returnVal["cmd"] = "send_file_port_reply";
  returnVal["port"] = port;
  returnVal["filename"] = val["filename"];
  returnVal["length"] = val["length"];
  b = Json::writeString(wbuilder, returnVal);
 // if (bufferevent_write(bev, b.c_str(), b.size()) < 0)
  if (send(bev->ev_read.ev_fd, b.c_str(), b.size(), 0) < 0) {
    cout << "bufferevent write error" << endl;
    return;
  }
  int count = 0;
  while (from_fd <= 0) {
    count++;
    usleep(100000);
    if (count == 100) {
      pthread_cancel(send_file_thread.native_handle()); // 取消线程
      returnVal.clear();
      returnVal["cmd"] = "send_file_reply";
      returnVal["result"] = "timeout";
      b = Json::writeString(wbuilder, returnVal);
      if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
 	      cout << "bufferevent write error" << endl;
      }
      return;
    }
  }
  cout << "part1 ok\n";

  // 返回端口号给接收端
  returnVal.clear();
  returnVal["cmd"] = "recv_file_port_reply";
  returnVal["port"] = port;
  returnVal["filename"]=val["filename"];
  returnVal["length"] = val["length"];
  b = Json::writeString(wbuilder, returnVal);
 // if (bufferevent_write(to_bev, b.c_str(), b.size()) < 0) {
  if(send(to_bev->ev_read.ev_fd, b.c_str(), b.size(), 0) < 0) { 
    cout << "bufferevent write error" << endl;
  }
  count = 0;
  while(to_fd <= 0) {
    count++;
    usleep(100000);
    if (count == 100) {
      pthread_cancel(send_file_thread.native_handle()); // 取消线程
      returnVal.clear();
      returnVal["cmd"] = "send_file_reply";
      returnVal["result"] = "timeout";
      b = Json::writeString(wbuilder, returnVal);
      if (bufferevent_write(bev, b.c_str(), b.size()) < 0) {
	      cout << "bufferevent write error" << endl;
      }
      if (bufferevent_write(to_bev, b.c_str(), b.size()) < 0) {
	      cout << "bufferevent write error" << endl;
      }
      return;
    }
  }
  cout << "part2 ok\n";
}

void Server::send_file_hander(size_t length, int port, int *f_fd, int *t_fd) {
  cout << "~~~~~~~~~~in send_file_hander~~~~~~~~~~~\n";
  const int MAXSIZE = 1024 * 1024;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    cout << "create sockfd failure!\n";
    return;
  }

  // sockfd为需要端口复用的套接字
  int opt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

  // 接受缓冲区
  int nRecvBuf = MAXSIZE;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char *)&nRecvBuf, sizeof(int));
  // 发送缓冲区
  int nSendBuf = MAXSIZE;
  setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char *)&nRecvBuf, sizeof(int));
  
  std::cout << "sockfd is: " << sockfd << std::endl;
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = inet_addr(IP);
  bzero(&(server_addr.sin_zero), 8);
  while (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1); 
  cout << "bind success\n";
  
  //进入监听状态，等待用户发起请求
  listen(sockfd, 10);
  
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  // 接受发送客户端的连接请求
  while (*f_fd <= 0) {
    *f_fd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
    //cout << " fromfd:" << *f_fd << "\n";
  }
  cout << "accept fromfd:" << *f_fd << "\n";

  // 接受接收客户端的连接请求
  while (*t_fd <= 0) {
    *t_fd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
     // cout << "tofd:" << *t_fd << "\n";
  }
  cout << "accept tofd:" << *t_fd << "\n";

  char buf[MAXSIZE] = {0};
  size_t size, sum = 0;

  while(true) {
    size = recv(*f_fd, buf, MAXSIZE, 0);
    if (size <= 0 || size > MAXSIZE) {
      break;
    }
    sum += size;
    send(*t_fd, buf, size, 0);
    if (sum >= length) {
      break;
    }
    memset(buf, 0, sizeof(buf));
  }
  cout << "send file end!\n";
  close(*f_fd);
  close(*t_fd);
  close(sockfd);
}
