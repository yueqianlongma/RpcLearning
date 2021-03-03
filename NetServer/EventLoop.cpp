
#include"EventLoop.h"
#include"Log.h"
#include<sys/eventfd.h>
#include<unistd.h>
#include<iostream>
using namespace net;

namespace 
{
    std::thread::id getThreadId()
    {
        return std::this_thread::get_id();
    }
}


EventLoop::EventLoop()
        :   tid_(getThreadId()),
            quit_(false),
            doingPendingTasks_(false),
            poller_(this),
            wakeupFd_(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
            wakeupChannel_(this, wakeupFd_),
            timerQueue_(this)
{
    if(wakeupFd_ == -1)
        fatal("EventLoop::eventfd()");
    wakeupChannel_.setReadCallback([this](){
        handleRead();
    });
    wakeupChannel_.enableRead();
}

EventLoop::~EventLoop()
{
    // wakeupChannel_.disableAll();
    ::close(wakeupFd_);
}

void EventLoop::loop()
{
    assert(!quit_);
    assertInLoopThread();
    trace("EventLoop %p polling", this);
    while(!quit_){
        activeChannels_.clear();
        poller_.poll(activeChannels_);
        for(auto channel : activeChannels_)
            channel->handleEvents();
        doPendingTasks();
    }
    trace("EventLoop %p quit", this);
}

void EventLoop::quit()
{
    assert(!quit_);
    quit_ = true;
    if(!isInLoopThread())   wakeup();
}

void EventLoop::runInLoop(const Task& task)
{
    if(isInLoopThread()) task();
    else queueInLoop(task);
}

void EventLoop::runInLoop(Task&& task)
{
    if(isInLoopThread())    {
        task();
    }
    else queueInLoop(std::move(task));
}

void EventLoop::queueInLoop(const Task& task)
{
    {
        std::lock_guard<std::mutex> lck(mutex_);
        pendingTasks_.push_back(std::move(task));
    }
    // if we are not in loop thread, just wake up loop thread to handle new task
    // if we are in loop thread && doing pending task, wake up too.
    // note that the following code has race condition:
    //     if (doingPendingTasks_ || isInLoopThread())
    if(!isInLoopThread() || doingPendingTasks_)
        wakeup();
}

void EventLoop::queueInLoop(Task&&  task)
{
    {
        std::lock_guard<std::mutex> lck(mutex_);
        pendingTasks_.push_back(std::move(task));
    }
    if(!isInLoopThread() || doingPendingTasks_)
        wakeup();
}

Timer* EventLoop::runAt(Timestamp when, TimerCallback cb)
{
    return timerQueue_.addTimer(std::move(cb), when, NanoSecond::zero());
}

Timer* EventLoop::runAfter(NanoSecond interval, TimerCallback  cb)
{
    return  runAt(now() + interval, std::move(cb));
}

Timer* EventLoop::runEvery(NanoSecond interval, TimerCallback cb)
{
    return  timerQueue_.addTimer(std::move(cb), now() + interval, interval);
}

                                                                    
void EventLoop::cancelTimer(Timer* timer)
{
    timerQueue_.cancelTimer(timer);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(uint64_t));
    if(n != sizeof(uint64_t))
        error("EventLoop::wakeup() should :: write() %lu bytes", sizeof(uint64_t));
}

void EventLoop::updateChannel(Channel* channel)
{
    assertInLoopThread();
    poller_.updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assertInLoopThread();
    channel->disableAll();
}

void EventLoop::assertInLoopThread()
{
    assert(isInLoopThread());
}

bool EventLoop::isInLoopThread()
{
    return tid_ == getThreadId();
}

//copy on write
void EventLoop::doPendingTasks()
{
    assertInLoopThread();
    std::vector<Task> tasks;
    {
        std::lock_guard<std::mutex> lck(mutex_);
        tasks.swap(pendingTasks_);
    }
    doingPendingTasks_ = true;
    for(Task& task : tasks)
        task();
    doingPendingTasks_ = false;
}

void EventLoop::handleRead()
{
    uint64_t one;
    ssize_t  n = ::read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
        trace("EventLoop::handleRead() should ::read()  %lu bytes", sizeof(one));
}

