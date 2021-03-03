
#ifndef __NETSERVER_CONNECTOR_H__
#define __NETSERVER_CONNECTOR_H__

#include"noncopyable.h"
#include"InetAddress.h"
#include"Channel.h"
#include"Callbacks.h"
#include<functional>

namespace net
{

class EventLoop;

class Connector: public noncopyable
{
public:
    Connector(EventLoop* loop, const InetAddress& peer);
    ~Connector();

    void start();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {   newConnectionCallback_ = cb;    }

    void setErrorCallback(const ErrorCallback& cb)
    {   errorCallback_ = cb;    }

private:
    void handleWrite();

private:
    EventLoop* loop_;
    const InetAddress peer_;
    const int sockfd_;
    bool connected_;
    bool started_;
    Channel channel_;
    NewConnectionCallback newConnectionCallback_;
    ErrorCallback errorCallback_;
};


}

#endif