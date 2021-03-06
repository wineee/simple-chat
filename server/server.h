#ifndef SERVER_H
#define SERVER_H

#include <event2/event.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/listener.h>
#include <thread>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <unistd.h>

#include "chatlist.h"

using namespace std;

#define IP "172.18.90.51"
#define PORT 8000

class Server {
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
  static void send_file_hander(size_t length, int port, int *from_fd, int *to_fd);
 public:
  Server(const char *ip = IP, int port = PORT);
  ~Server();
};
  
#endif
