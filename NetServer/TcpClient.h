
#ifndef __NETSERVER_TCP_CLINET_H__
#define __NETSERVER_TCP_CLINET_H__

#include<functional>
#include<memory>
#include"Connector.h"
#include"InetAddress.h"
#include"Callbacks.h"
#include"CountDownLatch.h"
#include"noncopyable.h"

namespace net
{

class Timer;
class EventLoop;
class TcpConnection;

class TcpClient: public noncopyable
{
public:
    TcpClient(EventLoop*, const InetAddress& );
    ~TcpClient();

    void start();
    void setConnectionCallback(const ConnectionCallback& cb)
    {  connectionCallback_ = cb;     }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    {   writeCompleteCallback_ = cb;    }
    void setMessageCallback(const MessageCallback& cb)
    {   messageCallback_ = cb;  }
    void setErrorCallback(const ErrorCallback& cb)
    {
        connector_->setErrorCallback(cb);
    }

    TcpConnectionPtr getTcpConnection();

private:
    void retry();
    void newConnection(int connfd, const InetAddress& local, const InetAddress& peer);
    void closeConnection(const TcpConnectionPtr& conn);

private:
    typedef std::unique_ptr<Connector> ConnectorPtr;

    EventLoop* loop_;
    bool connected_;
    const InetAddress peer_;
    Timer* retryTimer_;
    ConnectorPtr connector_;
    TcpConnectionPtr connection_;
    ConnectionCallback      connectionCallback_;
    MessageCallback         messageCallback_;
    WriteCompleteCallback   writeCompleteCallback_;

    CountDownLatch countDown_;
};

}


#endif