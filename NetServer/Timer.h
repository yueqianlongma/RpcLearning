#ifndef __NETSERVER_TIMER_H__
#define __NETSERVER_TIMER_H__

#include"noncopyable.h"
#include"Callbacks.h"
#include<functional>
#include"Timestamp.h"
#include<assert.h>

namespace net
{

class Timer : noncopyable
{
public:
    Timer(TimerCallback callback, Timestamp when, NanoSecond interval)
        :   callback_(callback),
            when_(when),
            interval_(interval),
            repeat_(interval_ > NanoSecond::zero()),
            canceled_(false)
    {
    }

    void run()  { if (callback_)  callback_(); }
    bool repeat() const { return repeat_; }
    bool expired(Timestamp now) const { return now >= when_; }
    Timestamp when() const { return when_; }

    void restart(Timestamp now)
    {
        assert(repeat_);
        when_ = now + interval_;
    }

    void cancel()
    {
        assert(!canceled_);
        canceled_ = true;
    }

    bool canceled()  const { return canceled_;   }

private:
    TimerCallback   callback_;
    Timestamp       when_;
    const   NanoSecond  interval_;
    bool    repeat_;
    bool    canceled_;
};

}

#endif