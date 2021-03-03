#ifndef __NETSERVER_TIMER_QUEUE_H__
#define __NETSERVER_TIMER_QUEUE_H__

#include"noncopyable.h"
#include"Timer.h"
#include"Timestamp.h"
#include"Channel.h"
#include<set>
#include<vector>

namespace net
{

class TimerQueue: noncopyable
{
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    Timer* addTimer(TimerCallback cb, Timestamp when, NanoSecond interval);
    void   cancelTimer(Timer* timer);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    void handleRead();
    std::vector<Entry>  getExpired(Timestamp now);

private:
    const int     timerfd_;
    Channel       timerChannel_;
    TimerList     timers_;
    EventLoop*    loop_;
};

}


#endif