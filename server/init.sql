create database user;
create database chatgroup;

use chatgroup;

create table 交流群1 (owner varchar(32), member varchar(2048)) char set utf8;
create table 交流群2 (owner varchar(32), member varchar(2048)) char set utf8;
create table 交流群3 (owner varchar(32), member varchar(2048)) char set utf8;


insert into 交流群1 values("Alice", "Bob|LiMing");
insert into 交流群2 values("李白", "杜甫|白居易");

create table Bob (passwd varchar(16), friend varchar(4096));
insert 
