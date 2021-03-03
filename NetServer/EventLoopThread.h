
#ifndef __NETSERVER_EVENT_LOOP_THREAD_H__
#define __NETSERVER_EVENT_LOOP_THREAD_H__

#include"noncopyable.h"
#include"CountDownLatch.h"
#include<thread>

namespace net
{

class EventLoop;

class EventLoopThread: public noncopyable
{
public:
    EventLoopThread() = default;
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void runInThread();

private:
    bool        started_ {false};
    EventLoop*  loop_{nullptr};
    std::thread     thread_;
    CountDownLatch  latch_{1};
};

}


#endif