

#ifndef __NETSERVER_TCP_SERVER_H__
#define __NETSERVER_TCP_SERVER_H__

#include"noncopyable.h"
#include"Callbacks.h"
#include"InetAddress.h"
#include<memory>
#include<atomic>
#include<set>

namespace net
{
class EventLoop;
class EventLoopThread;
class EventLoopThreadPool;
class Acceptor;
class TcpConnection;


class TcpServer: public noncopyable
{
public:
    TcpServer(EventLoop* , const InetAddress& );
    ~TcpServer();

    void setNumThread(size_t threadNums);
    void start();

    void setThreadInitCallback(const ThreadInitCallback& cb)
    { threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }


private:
    void newConnection(int connfd, const InetAddress& local, const InetAddress& peer);
    void closeConnection(const TcpConnectionPtr& conn);
    void closeConnectionInLoop(const TcpConnectionPtr& conn);
private:
    typedef std::unique_ptr<EventLoopThreadPool> EventLoopThreadPoolPtr;
    typedef std::unique_ptr<Acceptor> AcceptorPtr;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::set<TcpConnectionPtr> ConnectionSet;

    EventLoop* baseLoop_;
    AcceptorPtr acceptor_;
    EventLoopThreadPoolPtr threadPool_;
    std::atomic_bool started_;

    ConnectionSet connections_;
    InetAddress local_;

    ThreadInitCallback threadInitCallback_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
};


}



#endif