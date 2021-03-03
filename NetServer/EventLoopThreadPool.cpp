
#include"EventLoopThreadPool.h"
#include"Log.h"
#include"EventLoop.h"
#include"EventLoopThread.h"
#include<cassert>
using namespace net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop)
    :   baseLoop_(loop),
        started_(false),
        next_(0),
        numThreads_(0)
{}

void EventLoopThreadPool::start()
{
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;
    for(int i = 0; i < numThreads_; ++i)
    {
        EventLoopThread* t = new EventLoopThread();
        threads_.emplace_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop = baseLoop_;

    if(!loops_.empty()){
        //round-robin
        loop = loops_[next_++];
        if(static_cast<size_t>(next_) >= loops_.size())
            next_ = 0;
    }
    return loop;
}

EventLoopThreadPool::EventLoopList EventLoopThreadPool::getAllLoop()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    if(loops_.empty())
        return {1, baseLoop_};
    return loops_;
}


