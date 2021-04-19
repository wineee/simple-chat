#include "chatlist.h"

ChatInfo::ChatInfo() {
  online_user = new list<User>;
  group_info = new list<Group>;
  mydatabase = new ChatDataBase;
  mydatabase->my_database_connect("chatgroup");

  string group_name[MAXNUM];
  int group_num = mydatabase->my_database_get_group_name(group_name);

  cout << group_num << endl;
  for (int i = 0; i < group_num; i++) {
    cout << group_name[i] << "\n";

    Group g;
    g.name = group_name[i];
    g.l = new list<GroupUser>;

    string member;
    mydatabase->my_database_get_group_member(group_name[i], member);
    cout << member << endl;
    if (member.size() == 0) {
      continue;
    }
    string::size_type start = 0, end = 0;
    while(start < member.size()) {
      end = member.find('|', start);
      if (end == string::npos) {
	end = member.size();
      }
      cout << start <<" "<< end << " " << member.substr(start, end-start) << "\n";
      g.l->push_back(GroupUser{member.substr(start, end-start)});
      start = end+1;
    }

    for (GroupUser user: *(g.l)) {
      cout << "name" << user.name << "\n";
    }
    group_info->push_back(g);
  }

  mydatabase->my_database_disconnect();
  
}

bool ChatInfo::info_group_exist(string group_name) {
  for (auto it = group_info->begin(); it != group_info->end(); it++) {
    if (it->name == group_name) {
      return true;
    }
  }
  return false;
}

bool ChatInfo::info_user_in_group(string group_name, string user_name) {
   for (auto it = group_info->begin(); it != group_info->end(); it++) {
    if (it->name == group_name) {
      for (auto uit = it->l->begin(); uit != it->l->end(); uit++) {
	if (uit->name == user_name) {
	  return true;
	}
      }
    }
  }
  return false;
}

void ChatInfo::info_group_add_user(string group_name, string user_name) {
   for (auto it = group_info->begin(); it != group_info->end(); it++) {
    if (it->name == group_name) {
      it->l->push_back(GroupUser{user_name});
    }
  }
}

struct bufferevent *ChatInfo::info_get_friend_bev(string name) {
  for (auto it = online_user->begin(); it != online_user->end(); it++) {
    if (it->name == name) {
      return it->bev;
    }
  }
  return nullptr;
}

string ChatInfo::info_get_group_member(string group_name) {
  for (auto it = group_info->begin(); it != group_info->end(); it++) {
    if (it->name == group_name) {
      string member_list;
      for (auto i = it->l->begin(); i != it->l->end(); i++) {
	member_list += i->name + "|";
      }
      return member_list;
    }
  }
  return "";
}

