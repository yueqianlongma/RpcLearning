
#ifndef __NETSERVER_EPOLLER_H__
#define __NETSERVER_EPOLLER_H__

#include"noncopyable.h"
#include<vector>

struct epoll_event;

namespace net
{

class EventLoop;
class Channel;

class EPoller: public noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    explicit EPoller(EventLoop* loop);
    ~EPoller();

    void poll(ChannelList& activeChannels);
    void updateChannel(Channel* channel);

private:
    void updateChannel(int op, Channel* channel);

private:
    EventLoop* loop_;
    std::vector<struct epoll_event> events_;
    int epollfd_;
};

}

#endif