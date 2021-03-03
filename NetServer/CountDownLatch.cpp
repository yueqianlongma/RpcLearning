
#include"CountDownLatch.h"
using namespace net;


CountDownLatch::CountDownLatch(int count)
    : count_(count)
{}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lck(mutex_);
    cv_.wait(lck, [this](){
        return this->count_ <=  0;
    });
}

void CountDownLatch::countDown()
{
    std::lock_guard<std::mutex> lck(mutex_);
    --count_;
    if(count_ == 0)
        cv_.notify_all();
}

int CountDownLatch::getCount()  const
{
    std::lock_guard<std::mutex> lck(mutex_);
    return count_;
}
