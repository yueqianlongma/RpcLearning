cmake_minimum_required(VERSION 3.0.0)
project(NetServer VERSION 0.1.0)

include(GNUInstallDirs)

find_package(Threads REQUIRED)

# 设置编译环境， 这里使用c++11标准， 并编译时添加了thread
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11 -pthread -g -march=native")

list(APPEND NETSERVER_SRCS
    ${PROJECT_SOURCE_DIR}/Acceptor.cpp
    ${PROJECT_SOURCE_DIR}/Buffer.cpp
    ${PROJECT_SOURCE_DIR}/Channel.cpp
    ${PROJECT_SOURCE_DIR}/Connector.cpp
    ${PROJECT_SOURCE_DIR}/EPoller.cpp
    ${PROJECT_SOURCE_DIR}/EventLoop.cpp
    ${PROJECT_SOURCE_DIR}/EventLoopThread.cpp
    ${PROJECT_SOURCE_DIR}/EventLoopThreadPool.cpp
    ${PROJECT_SOURCE_DIR}/InetAddress.cpp
    ${PROJECT_SOURCE_DIR}/TcpClient.cpp
    ${PROJECT_SOURCE_DIR}/TcpConnection.cpp
    ${PROJECT_SOURCE_DIR}/TcpServer.cpp
    ${PROJECT_SOURCE_DIR}/TimerQueue.cpp
    ${PROJECT_SOURCE_DIR}/CountDownLatch.cpp
    ${PROJECT_SOURCE_DIR}/TimeTool.cpp
    )

option(BUILD_NETSERVER_SHARED_LIBRARY "Build NetServer Shared Library" OFF)
option(BUILD_NETSERVER_STATIC_LIBRARY "Build NetServer Shared Library" ON)
option(BUILD_NETSERVER_EXAMPLES "Build NetServer Examples" OFF)

##NetServer Shared Library
if(BUILD_NETSERVER_SHARED_LIBRARY)
    add_library(NetServer SHARED ${NETSERVER_SRCS})
    target_include_directories(NetServer PUBLIC ${PROJECT_SOURCE_DIR})
    target_link_libraries(NetServer Threads::Threads)
    install(TARGETS NetServer DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(BUILD_NETSERVER_SHARED_LIBRARY)

#NetServer Static library
if(BUILD_NETSERVER_STATIC_LIBRARY)
    add_library(NetServer_s STATIC ${NETSERVER_SRCS})
    target_include_directories(NetServer_s PUBLIC ${PROJECT_SOURCE_DIR})
    target_link_libraries(NetServer_s Threads::Threads)
    install(TARGETS NetServer_s DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(BUILD_NETSERVER_STATIC_LIBRARY)


if(BUILD_NETSERVER_SHARED_LIBRARY OR BUILD_NETSERVER_STATIC_LIBRARY)
    install(FILES 

        ${PROJECT_SOURCE_DIR}/Acceptor.h
        ${PROJECT_SOURCE_DIR}/Buffer.h
        ${PROJECT_SOURCE_DIR}/Callbacks.h
        ${PROJECT_SOURCE_DIR}/Channel.h
        ${PROJECT_SOURCE_DIR}/Connector.h
        ${PROJECT_SOURCE_DIR}/EPoller.h
        ${PROJECT_SOURCE_DIR}/EventLoop.h
        ${PROJECT_SOURCE_DIR}/EventLoopThread.h
        ${PROJECT_SOURCE_DIR}/EventLoopThreadPool.h
        ${PROJECT_SOURCE_DIR}/InetAddress.h
        ${PROJECT_SOURCE_DIR}/TcpClient.h
        ${PROJECT_SOURCE_DIR}/TcpConnection.h
        ${PROJECT_SOURCE_DIR}/TcpServer.h
        ${PROJECT_SOURCE_DIR}/Timer.h
        ${PROJECT_SOURCE_DIR}/TimerQueue.h
        ${PROJECT_SOURCE_DIR}/Timestamp.h

        ${PROJECT_SOURCE_DIR}/CountDownLatch.h
        ${PROJECT_SOURCE_DIR}/Log.h
        ${PROJECT_SOURCE_DIR}/noncopyable.h
        ${PROJECT_SOURCE_DIR}/TimeTool.h

        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif(BUILD_NETSERVER_SHARED_LIBRARY OR BUILD_NETSERVER_STATIC_LIBRARY)

include_directories(${PROJECT_SOURCE_DIR})


function(add_NetServer_executable EXECUTABLE_NAME EXECUTABLE_SOURCES)
    add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})
    target_link_libraries(${EXECUTABLE_NAME} NetServer_s)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction(add_NetServer_executable)


add_NetServer_executable(EchoClient Echo/EchoClient.cpp)
add_NetServer_executable(EchoServer Echo/EchoServer.cpp)