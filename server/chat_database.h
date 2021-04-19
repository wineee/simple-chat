#ifndef CHAT_DATABASE_H
#define CHAT_DATABASE_H

#include <mysql/mysql.h>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

class ChatDataBase {
 private:
  MYSQL *mysql;
 public:
  ChatDataBase();
  ~ChatDataBase();

  void my_database_connect(const char *);
  int my_database_get_group_name(string *);
  void my_database_get_group_member(string name, string &s);
  void my_database_disconnect();
  bool my_database_user_exist(string name);
  void my_database_user_passwd(string name, string passwd);
  bool my_database_passwd_correct(string name, string passwd);
  string my_database_get_friend(string name);
  bool my_database_is_friend(string name1, string name2);
  void my_database_add_new_friend(string, string);
  bool my_database_group_exist(string group_name);
  void my_database_add_new_group(string group_name, string user_name);
  void my_database_user_add_group(string user_name, string group_name);
  void my_database_group_add_user(string group_name, string user_name);
};

#endif
