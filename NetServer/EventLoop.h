
#ifndef __NETSERVER_EVENT_LOOP_H__
#define __NETSERVER_EVENT_LOOP_H__

#include"noncopyable.h"
#include"EPoller.h"
#include"TimerQueue.h"
#include"Callbacks.h"
#include<functional>
#include"Timestamp.h"
#include<atomic>
#include<mutex>
#include<vector>
#include<thread>

namespace net
{

class Timer;
class Channel;

class EventLoop: noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();    //thread safe

    void runInLoop(const Task& task);
    void runInLoop(Task&& task);
    void queueInLoop(const Task& task);
    void queueInLoop(Task&& task);

    Timer* runAt(Timestamp when, TimerCallback cb);
    Timer* runAfter(NanoSecond interval, TimerCallback cb);
    Timer* runEvery(NanoSecond interval, TimerCallback cb);
    void   cancelTimer(Timer* timer);

    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread();
    bool isInLoopThread();

private:
    void doPendingTasks();
    void handleRead();

private:
    std::thread::id tid_;
    std::atomic_bool quit_;

    EPoller poller_;
    EPoller::ChannelList activeChannels_;

    const int wakeupFd_;
    Channel wakeupChannel_;

    bool doingPendingTasks_;
    std::mutex mutex_;
    std::vector<Task> pendingTasks_;

    TimerQueue timerQueue_;
};
    
} // namespace net



#endif