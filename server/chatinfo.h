#ifndef CHATINFO_H
#define CHATINFO_H

#include <event.h>
#include <list>
#include "chat_database.h"

using namespace std;

#define MAXNUM 1024  //群数量

struct User {
  string name;
  struct bufferevent *bev;
};

struct GroupUser {
  string name;
};

struct Group {
  string name;
  list<GroupUser> *l;
};

class ChatInfo {
 private:
  list<User> *online_user;
  list<Group> *group_info;
  ChatDataBase *mydatabase;
 public:
  ChatInfo();
  ~ChatInfo();
  bool info_group_exist(string name);
  bool info_user_in_group(string group_name, string user_name);
  void info_group_add_user(string group_name, string user_name);
  string info_get_group_member(string group_name);
  struct bufferevent *info_get_friend_bev(string);
};

#endif
