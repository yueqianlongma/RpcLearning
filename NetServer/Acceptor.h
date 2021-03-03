
#ifndef __NETSERVER_ACCEPTOR_H__
#define __NETSERVER_ACCEPTOR_H__
#include"noncopyable.h"
#include"Channel.h"
#include"InetAddress.h"
#include"Callbacks.h"
#include<functional>

namespace net
{

class EventLoop;

class Acceptor: public noncopyable
{
public:
    Acceptor(EventLoop* loop, const InetAddress& local);
    ~Acceptor();

    bool listening() const
    {   return listening_; }

    void listen();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {   newConnectionCallback_ = cb; }

private:
    void handleRead();

private:
    int                 idleFd_;
    bool                listening_;
    EventLoop*          loop_;
    const int           acceptFd_;
    Channel             acceptChannel_;
    InetAddress         local_;
    NewConnectionCallback   newConnectionCallback_;
};


}


#endif