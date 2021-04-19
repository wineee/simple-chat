from socket import *
import json

IP = "127.0.0.1"
SERVER_PORT = 8000
BUFLEN = 1024

# 实例化一个socket对象，指明协议
dataSocket = socket(AF_INET, SOCK_STREAM)

# 连接服务端socket
dataSocket.connect((IP, SERVER_PORT))

while True:
    # 从终端读入用户输入的字符串
    toSend = input(">>>")
    if toSend == "exit":
        break
    if toSend == "R":
        name = input("name:")
        passwd = input("passwd:")
        data = {"cmd": "register", "user": name, "password": passwd}
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
