# 不能用的话，用 g++ *.cpp -o main -std=c++11 -levent -lpthread -lmysqlclient -ljsoncpp

cmake_minimum_required(VERSION 3.1)
project(simple-chat)

# 设置编译器（gcc/g++）
set(CMAKE_CXX_COMPILER "g++")

# 设置Debug/Release
set(CMAKE_BUILD_TYPE "Debug")

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -levent -lpthread -lmysqlclient -ljsoncpp -Wall")

# 需要哪些源码
set(SRC_LIST main.cpp chat_database.cpp server.cpp chatlist.cpp)

# add_exectuable 告诉工程生成一个可执行文件。
add_executable(main ${SRC_LIST})
