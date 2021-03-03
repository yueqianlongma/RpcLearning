#ifndef __NETSERVER_COUNTDOWNLATCH_H__
#define __NETSERVER_COUNTDOWNLATCH_H__

#include"noncopyable.h"
#include<mutex>
#include<condition_variable>

/**
 * 倒计时(CountDownLatch)同步手段， 主要用途
 * 
 *  一. 主线程发起多个子线程，等这些子线程各自都完成一定的任务之后，主线程才继续执行。
 *                                                  通常用于主线程等待多个子线程完成初始化。
 * 
 *  二. 主线程发起多个子线程，子线程都等待主线程，主线程完成其他一些任务之后通知所有子线程开始执行。
 *                                                  通常用于多个子线程等待主线程发起“起跑”命令。
 */

namespace net
{

class CountDownLatch : public noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount()  const;

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};

}


#endif