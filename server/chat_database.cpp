#include "chat_database.h"

ChatDataBase::ChatDataBase() {
 
}

ChatDataBase::~ChatDataBase() {
  
}

void ChatDataBase::my_database_connect(const char* name) {
  mysql = mysql_init(nullptr);
  mysql = mysql_real_connect(mysql, "localhost", "root", "passwd", name, 0, nullptr, 0);
  if (mysql == nullptr) {
    cout << "connect database failure" << endl;
    return;
  }
  if (mysql_query(mysql, "set names utf8;") != 0) {
     cout << "set utf8 error!" << endl;
  }
}

int ChatDataBase::my_database_get_group_name(string *s) {
  if (mysql_query(mysql, "set names utf8;") != 0) {
    cout << "set utf8 error!" << endl;
  }
  if (mysql_query(mysql, "show tables;") != 0) {
    cout << "mysql query error!" << endl;
  }
  MYSQL_RES *res = mysql_store_result(mysql);
  if (nullptr == res) {
    cout << "mysql store result!" << endl;
  }

  int count = 0;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(res)) != 0) {
    s[count] += row[0];
    count++;
  }
  return count;
}

void ChatDataBase::my_database_disconnect() {
  mysql_close(mysql);
}

void ChatDataBase::my_database_get_group_member(string name, string &s) {
  char sql[1024] = {0};
  sprintf(sql, "select member from %s", name.c_str());
  if (mysql_query(mysql, sql) != 0) {
    cout << "mysql query erreor" << endl;
  }
  MYSQL_RES *res = mysql_store_result(mysql);
  if (nullptr == res) {
    cout << "mysql store result error" << endl;
  }
  MYSQL_ROW row = mysql_fetch_row(res);
  if (row[0] == nullptr) {
    return;
  }
  s += row[0];
}

bool ChatDataBase::my_database_user_exist(string name) {
   string sql = "show tables like '" + name + "'";
   if (mysql_query(mysql, sql.c_str())) {
     cout << "mysql query error" << endl;
     return false;
   }
   MYSQL_RES *res = mysql_store_result(mysql);
   MYSQL_ROW row = mysql_fetch_row(res);
   if (row == nullptr) {
     return false;
   } else {
     return true;
   }
}

void ChatDataBase::my_database_user_passwd(string name, string passwd) {
  string sql = "create table " + name + " (passwd varchar(16), friend varchar(4096), chatgroup varchar(4096))";
  if (mysql_query(mysql, sql.c_str()) != 0) {
     cout << "create table error!" << endl;
  }

  sql = "insert into " + name + " (passwd) values ('" + passwd + "')";
  if (mysql_query(mysql, sql.c_str()) != 0) {
     cout << "set passwd error!" << endl;
  }
}

bool ChatDataBase::my_database_passwd_correct(string name, string passwd) {
  string sql = "select passwd from " + name;
  if (mysql_query(mysql, sql.c_str()) != 0) {
    cout << "mysql query error!" << endl;
  }

  MYSQL_RES *res = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(res);
  return passwd == row[0];
}

string ChatDataBase::my_database_get_friend(string name) {
  string sql = "select friend from " + name;
  if (mysql_query(mysql, sql.c_str()) != 0) {
    cout << "mysql query error!" << endl;
  }

  MYSQL_RES *res = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(res);
  if (nullptr == row[0]) {
    return "";
  }
  return string("").append(row[0]);
}

bool ChatDataBase::my_database_is_friend(string name1, string name2) {
  string sql = "select friend from " + name1;
  if (mysql_query(mysql, sql.c_str()) != 0) {
    cout << "mysql query error!" << endl;
  }
  MYSQL_RES *res = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(res);
  if (nullptr == row[0]) {
    return false;
  }
  string all_friend = row[0];
  return all_friend.find("|"+name2+"|") != string::npos;
}

void ChatDataBase::my_database_add_new_friend(string name, string fri) {
  string sql = "select friend from " + name;
  if (mysql_query(mysql, sql.c_str()) != 0) {
    cout << "mysql query error!" << endl;
  }
  string friend_list;
  MYSQL_RES *res = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(res);
  if (row[0] == nullptr) {
    friend_list.append(fri);
  } else {
    friend_list.append(row[0]);
    friend_list += "|" + fri;
  }
  
  sql = "update " + name + " set friend = " + friend_list;
  if (mysql_query(mysql, sql.c_str()) != 0) {
    cout << "mysql query error!" << endl;
  }
  
}

bool ChatDataBase::my_database_group_exist(string group_name) {
   string sql = "show tables like " + group_name;
   if (mysql_query(mysql, sql.c_str())) {
     cout << "mysql query erreor" << endl;
   }
   MYSQL_RES *res = mysql_store_result(mysql);
   MYSQL_ROW row = mysql_fetch_row(res);
   if (nullptr == row) {
     return false;
   } else {
     return true;
   }
}

void ChatDataBase::my_database_add_new_group(string group_name, string user_name) {
  string sql = "create table " + group_name + " (owner varchar(32), member varchar(4096))";

  if (mysql_query(mysql, sql.c_str())) {
    cout << "mysql query erreor" << endl;
  }

  sql = "insert into " + group_name + " values ('" + user_name + "','"+ user_name +"')";

  
  if (mysql_query(mysql, sql.c_str())) {
    cout << "mysql query erreor" << endl;
  }
}

void ChatDataBase::my_database_user_add_group(string user_name, string group_name) {
  string sql = "select chatgroup from " + user_name;
  if (mysql_query(mysql, sql.c_str())) {
    cout << "mysql query erreor" << endl;
  }

  MYSQL_RES *res = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(res);
  string all_group;
  if (row[0] != nullptr) {
    all_group += row[0];
    all_group += "|" + group_name;
  } else {
    all_group = group_name;
  }

  sql = "update " + user_name + " set chatgroup = '" + all_group + "'";
  if (mysql_query(mysql, sql.c_str())) {
    cout << "mysql query erreor" << endl;
  }
  
}


void ChatDataBase::my_database_group_add_user(string group_name, string user_name) {
  string sql = "select member from " + group_name;
  if (mysql_query(mysql, sql.c_str())) {
    cout << "mysql query erreor" << endl;
  }

  MYSQL_RES *res = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(res);
  string all_member;
  if (row[0] != nullptr) {
    all_member += row[0];
    all_member += "|" + user_name;
  } else {
    all_member = user_name;
  }

  sql = "update " + group_name + " set member = '" + all_member + "'";
  if (mysql_query(mysql, sql.c_str())) {
    cout << "mysql query error" << endl;
  }
}
