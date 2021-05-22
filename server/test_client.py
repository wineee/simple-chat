from socket import *
import json
from threading import Thread

IP = "39.108.144.134"
SERVER_PORT = 8000      
BUFLEN = 1024

# 实例化一个socket对象，指明协议
dataSocket = socket(AF_INET, SOCK_STREAM)

# 连接服务端socket
dataSocket.connect((IP, SERVER_PORT))

print("R 注册| L 登陆| A 加好友| G 建群| AG 加群| CM 群聊| C 私聊|  M 群成员| E 退出")

while True:
    # 从终端读入用户输入的字符串
    toSend = input(">>>").strip()
    data = ""
    if toSend == "E":
        break
    if toSend == "R":
        name = input("name:")
        passwd = input("passwd:")
        data = {"cmd": "register", "user": name, "passwd": passwd}
    if toSend == "L":
        name = input("name:")
        passwd = input("passwd:")
        data = {"cmd": "login", "user": name, "passwd": passwd}
    if toSend == "A":
        name = input("name:")
        friend = input("friend:")
        data = {"cmd": "add", "user": name, "friend": friend}
    if toSend == "G":
        name = input("name:")
        group = input("group:")
        data = {"cmd": "create_group", "user": name, "group": group}
    if toSend == "AG":
        name = input("name:")
        group = input("group:")
        data = {"cmd": "add_group", "user": name, "group": group}
    if toSend == "C":
        name_from = input("user_from:")
        name_to = input("user_to:")
        text = input("text:")
        data = {"cmd": "private_chat", "user_from": name_from, "user_to": name_to, "text": text}
    if toSend == "CM":
        user = input("user:")
        group = input("group name:")
        text = input("text:")
        data = {"cmd":"group_chat", "user":user, "group":group, "text":text}
    if toSend == "M":
        group = input("group name:")
        data = {"cmd": "get_group_member", "group": group}
    if data != "":
        jdata = json.dumps(data)
        # 发送消息，也要编码为 bytes
        dataSocket.send(jdata.encode())
    # 等待接收服务端的消息
    recved = dataSocket.recv(BUFLEN)
    # 如果返回空bytes，表示对方关闭了连接
    if not recved:
        break
    # 打印读取的信息
    print(recved.decode())

dataSocket.close()


