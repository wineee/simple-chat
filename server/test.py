from socket import *
import json
from threading import Thread

IP = "39.108.144.134"
SERVER_PORT = 8080      
BUFLEN = 1024 * 1024

dataSocket = socket(AF_INET, SOCK_STREAM)
dataSocket.connect((IP, SERVER_PORT))


while True:
    recved = dataSocket.recv(BUFLEN)
    # 如果返回空bytes，表示对方关闭了连接
    if not recved:
        break
    # 打印读取的信息
    print(recved.decode())

dataSocket.close()


