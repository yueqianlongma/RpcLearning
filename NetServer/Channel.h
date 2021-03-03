#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include"noncopyable.h"
#include<functional>
#include<sys/epoll.h>
#include<memory>

// 可读            EPOLLIN
// 可写            EPOLLOUT
// 对方关闭        EPOLLRDHUP
// 异常            EPOLLPRI
// 错误            EPOLLERR
// 挂起            EPOLLHUP

namespace   net
{

class EventLoop;
class Channel: public noncopyable
{
public:
    typedef std::function<void()> ReadCallback;
    typedef std::function<void()> WriteCallback;
    typedef std::function<void()> CloseCallback;
    typedef std::function<void()> ErrorCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void setReadCallback(const ReadCallback& cb)
    {   readCallback_ = cb;     }
    void setWriteCallback(const WriteCallback& cb)
    {   writeCallback_ = cb;    }
    void setCloseCallback(const CloseCallback& cb)
    {   closeCallback_ = cb;    }
    void setErrorCallback(const ErrorCallback& cb)
    {   errorCallback_ = cb;    }
    
    void handleEvents();

    int fd() const {   return fd_; }
    int getEvents() const {   return events_; }
    void setRevents(int revents)   {    revents_ = revents; }
    bool isNoneEvents() const {   return events_ == 0; }

    void enableRead()   {   events_ |= (EPOLLIN | EPOLLPRI); update(); }
    void enableWrite()  {   events_ |= (EPOLLOUT ); update(); }

    void disableRead()  {   events_ &= ~EPOLLIN;  update(); }
    void disableWrite() {   events_ &= ~EPOLLOUT; update(); }
    void disableAll()   {   events_ = 0;          update(); }

    bool isReading() const {   return events_ & EPOLLIN;  }
    bool isWriting() const {   return events_ & EPOLLOUT;  }

    void tie(const std::shared_ptr<void>& obj);  

    void setState(int state) {   state_ = state; }
    int  getState() const {   return state_;  }

private:
    void update();
    void remove();
    void handleEventsWithGuard();

private:

    int state_;

    EventLoop*          loop_;
    const int           fd_;
    unsigned int        events_;
    unsigned int        revents_;

    std::weak_ptr<void> tie_;
    bool    tied_;
    
    bool    handlingEvents_;

    ReadCallback     readCallback_;
    WriteCallback    writeCallback_;
    CloseCallback    closeCallback_;
    ErrorCallback    errorCallback_;
};

}


#endif