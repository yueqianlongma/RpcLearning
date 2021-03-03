
#include"Log.h"
#include"EPoller.h"
#include"EventLoop.h"
#include"Channel.h"
#include<sys/epoll.h>
#include<unistd.h>
using namespace net;

namespace
{
    int IN = 1;
    int OUT = -1;
}

EPoller::EPoller(EventLoop* loop)
        :   loop_(loop),
            events_(128),
            epollfd_(::epoll_create1(EPOLL_CLOEXEC))
{
    if(epollfd_ == -1)
        fatal("EPoller::epoll_create1()");
}

EPoller::~EPoller()
{
    ::close(epollfd_);
}


void EPoller::poll(EPoller::ChannelList& activeChannels)
{
    int maxEvents = static_cast<int>(events_.size());
    int nEvents = epoll_wait(epollfd_, events_.data(), maxEvents, -1);
    if(nEvents == -1){
        if(errno != EINTR)
            error("EPoller::epoll_wait()");
    }
    else if(nEvents > 0){
        for(int i = 0; i < nEvents; ++i){
            auto channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->setRevents(events_[i].events);
            activeChannels.push_back(channel);            
        }
        if(nEvents == maxEvents)
            events_.resize(2 * events_.size());
    }
}

void EPoller::updateChannel(Channel* channel)
{
    loop_->assertInLoopThread();
    int op = 0;
    if(channel->getState() == OUT){
        assert(!channel->isNoneEvents());
        op = EPOLL_CTL_ADD;
        channel->setState(IN);
    }
    else if(!channel->isNoneEvents()){
        op = EPOLL_CTL_MOD;
    }
    else {
        op = EPOLL_CTL_DEL;
        channel->setState(OUT);
    }
    updateChannel(op, channel);
}

void EPoller::updateChannel(int op, Channel* channel)
{
    epoll_event event;
    event.events = channel->getEvents();
    event.data.ptr = channel;
    int ret = ::epoll_ctl(epollfd_, op, channel->fd(), &event);
    if(ret == -1)
        error("EPoller::epoll_ctl()");
}