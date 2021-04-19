cmake_minimum_required(VERSION 2.6)
project(demo)

# 设置编译器（gcc/g++）
set(CMAKE_CXX_COMPILER "g++")

#设置Debug/Release
set(CMAKE_BUILD_TYPE "Debug")

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -Wall")

# 设置可执行二进制文件的目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置存放编译出来的库文件的目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 并把该目录设置为链接目录
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 设定头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 增加子文件夹（src路径是通过子文件夹形式添加）
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
# 编译静态、动态链接库
add_library(TcpServer SHARED TcpServer.cpp)
target_link_libraries(TcpServer libevent.so)

# 编译可执行文件和设置链接库（注意顺序）
add_executable(main main.cpp)
target_link_libraries(main libTcpServer.so)
