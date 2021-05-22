create database user;
create database chatgroup;

--- 创建用户
create table LiMin (passwd varchar(16), friend varchar(4096), chatgroup varchar(4096));
insert into LiMin (passwd) values ('11111');

-- 查询加入的群表
select chatgroup from `LiMin`

--- 创建交流群
use chatgroup;
create table 交流群1 (owner varchar(32), member varchar(2048))；
insert into 交流群1 values("Alice", "|Alice|");

--- 添加好友
select friend from Li
update Li set friend = '|Ma|wine|'
