
#ifndef __NETSERVER_EVENT_LOOP_THREAD_POOL_H__
#define __NETSERVER_EVENT_LOOP_THREAD_POOL_H__

#include"noncopyable.h"
#include<memory>
#include<vector>

namespace net
{
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool: public noncopyable
{
public:
    typedef std::unique_ptr<EventLoopThread> EventLoopThreadPtr;
    typedef std::vector<EventLoopThreadPtr> ThreadPtrList;
    typedef std::vector<EventLoop*> EventLoopList;

public:
    EventLoopThreadPool(EventLoop*);

    void setThreadNums(int numThreads)
    { numThreads_ = numThreads; }

    void start();
    bool started()const 
    { return started_; }

    EventLoop* getNextLoop();
    EventLoopList getAllLoop();

private:
    bool started_;
    EventLoop* baseLoop_;
    int numThreads_;
    int next_;
    ThreadPtrList threads_;
    EventLoopList loops_;
};


}



#endif
