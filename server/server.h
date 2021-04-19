#ifndef SERVER_H
#define SERVER_H

#include <event.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/listener.h>
#include <thread>
#include <iostream>
#include <json/json.h>
#include <unistd.h>

#include "chatinfo.h"

using namespace std;

class Server {
  friend class ChatInfo;
 private:
  struct event_base *base; // 事件集合
  struct evconnlistener *listener; // 监听事件
  static ChatInfo *chatlist;
  static ChatDataBase *chatdb;
 private:
  static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg);
  static void client_handler(evutil_socket_t fd);
  static void read_cb(struct bufferevent *bev, void *ctx);
  static void event_cb(struct bufferevent *bev, short what, void *ctx);
  static void server_register(struct bufferevent *bev, Json::Value val);
  static void server_login(struct bufferevent *bev, Json::Value val);   static void server_add_friend(struct bufferevent *bev, Json::Value val);
  static void server_create_group(struct bufferevent *bev, Json::Value val);
  static void server_add_group(struct bufferevent *bev, Json::Value val);
  static void server_private_chat(struct bufferevent *bev, Json::Value val);
  static void server_group_chat(struct bufferevent *bev, Json::Value val);
  static void server_get_group_member(struct bufferevent *bev, Json::Value val);
  static void server_user_offline(struct bufferevent *bev, Json::Value val);
  static void server_send_file(struct bufferevent *bev, Json::Value val);
  static void send_file_hander(int length, int port, int *from_fd, int *to_fd);
 public:
  Server(const char *ip = "120.0.0.1", int port = 8000);
  ~Server();
};
  
#endif
