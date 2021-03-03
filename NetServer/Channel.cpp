

#include"Channel.h"
#include"EventLoop.h"
#include<assert.h>
using namespace net;


Channel::Channel(EventLoop* loop, int fd)
        :   loop_(loop),
            fd_(fd),
            events_(0),
            revents_(0),
            tied_(false),
            handlingEvents_(false),
            state_(-1)
{}

Channel::~Channel()
{
    assert(!handlingEvents_);
}

void Channel::handleEvents()
{
    loop_->assertInLoopThread();
    // channel is always a member of another object
    // e.g. Timer, Acceptor, TcpConnection
    // TcpConnection is managed by std::shared_ptr,
    // and may be destructed when handling events,
    // so we use weak_ptr->shared_ptr to
    // extend it's life-time.
    //条款20：对于类似shared_ptr但有可能空悬的指针使用weak_ptr
    if(tied_)
    {
        auto guard = tie_.lock();
        if(guard)
            handleEventsWithGuard();
    }
    else handleEventsWithGuard();
}

void Channel::handleEventsWithGuard()
{
    handlingEvents_ = true;
    if ( ( revents_ & EPOLLHUP ) && !( revents_ & EPOLLIN ) )
        if ( closeCallback_ )   closeCallback_();

    if ( revents_ & EPOLLERR )
        if(errorCallback_)  errorCallback_();

    if ( revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP ) )
        if(readCallback_)   readCallback_();

    if ( revents_ & EPOLLOUT )
        if(writeCallback_)  writeCallback_();
    handlingEvents_ = false;
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(state_ != -1);
    loop_->removeChannel(this);
}