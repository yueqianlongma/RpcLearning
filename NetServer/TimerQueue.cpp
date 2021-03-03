
#include"TimerQueue.h"
#include"Log.h"
#include"EventLoop.h"
#include<sys/timerfd.h>
#include<unistd.h>
#include<ratio>
#include<cstring>
#include<assert.h>
using namespace net;

namespace
{
int timerFdCreate()
{
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(fd == -1)
        fatal("timerfd_create");
    return fd;
}

void timerFdRead(int fd)
{
    uint64_t val;
    ssize_t n = read(fd, &val, sizeof(val));
    if(n != sizeof(val))
        error("timerFdRead get %ld, not %lu", n, sizeof(val));
}

timespec duractionFromNow(Timestamp when)
{
    NanoSecond ns = when - now();
    if(ns < MicroSecond(100)) 
        ns = MicroSecond(100);

    timespec ret;
    ret.tv_sec = static_cast<time_t>(ns.count() / std::nano::den);
    ret.tv_nsec = ns.count() % std::nano::den;
    return ret;
}

void timerFdSet(int fd, Timestamp when)
{
    itimerspec newTime;
    bzero(&newTime, sizeof(itimerspec));
    newTime.it_value = duractionFromNow(when);
    
    int ret = timerfd_settime(fd, 0, &newTime, nullptr);
    if(ret == -1)
        fatal("timerfd_settime()");
}
}

TimerQueue::TimerQueue(EventLoop *loop)
    :   loop_(loop),
        timerfd_(timerFdCreate()),
        timerChannel_(loop, timerfd_)
{
    loop_->assertInLoopThread();
    timerChannel_.setReadCallback([this]{
        handleRead();
    });
    timerChannel_.enableRead();
}

TimerQueue::~TimerQueue()
{
    for(auto& t : timers_)
        delete  t.second;
    ::close(timerfd_);
}

Timer* TimerQueue::addTimer(TimerCallback cb, Timestamp when, NanoSecond interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop([=](){
        auto ret = timers_.insert({when, timer});
        assert(ret.second);
        if(timers_.begin() == ret.first)
            timerFdSet(timerfd_, when);
    });
    return timer;
}

void TimerQueue::cancelTimer(Timer* timer)
{
    loop_->runInLoop([this, timer](){
        timer->cancel();
        this->timers_.erase({timer->when(), timer});
        delete timer;
    });
}

void TimerQueue::handleRead()
{
    timerFdRead(timerfd_);

    Timestamp now(::now());
    for(auto& e : getExpired(now))
    {
        Timer* timer = e.second;
        assert(timer->expired(now));

        if(!timer->canceled())
            timer->run();
        if(!timer->canceled() && timer->repeat())
        {
            timer->restart(now);
            e.first = timer->when();
            timers_.insert(e);
        }
        else delete timer;
    }
    if(!timers_.empty())
        timerFdSet(timerfd_, timers_.begin()->first);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    Entry strike(now + NanoSecond(1), nullptr);
    std::vector<Entry> entries;

    auto end = timers_.lower_bound(strike);
    entries.assign(timers_.begin(), end);
    timers_.erase(timers_.begin(), end);

    return entries;
}
