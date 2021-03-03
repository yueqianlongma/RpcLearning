#include<Log.h>
#include<Callbacks.h>
#include<TcpServer.h>
#include<EventLoop.h>
#include<InetAddress.h>
#include<TcpConnection.h>
using namespace net;


void connectionCallBack(const TcpConnectionPtr& conn)
{
    info("连接建立 %s", conn->name().c_str());
}

void writeCompleteCallBack(const TcpConnectionPtr& conn)
{
    info("%s 发送完毕", conn->name().c_str());
}


void messageCallBack(const TcpConnectionPtr& conn, Buffer& buff)
{
    info("收到数据 %d", buff.readableBytes());
    conn->send(buff);
}


int main(int argc, char** argv)
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 7777);
    TcpServer server_(&loop, addr);
    server_.setConnectionCallback(connectionCallBack);
    server_.setWriteCompleteCallback(writeCompleteCallBack);
    server_.setMessageCallback(messageCallBack);
    server_.setNumThread(2);
    server_.start();
    loop.loop();
    return 0;
}