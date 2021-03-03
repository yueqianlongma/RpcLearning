#ifndef __NETSERVER_NONCOPYABLE_H__
#define __NETSERVER_NONCOPYABLE_H__

namespace net
{

class noncopyable
{
public:
    noncopyable(noncopyable&) = delete;
    noncopyable& operator=(noncopyable&) = delete;
protected:
    noncopyable() = default;        //这里必须要写, 因为上面我们使用了  noncopyable(noncopyable&), 虽然是delete的,但是任然使得默认的构造函数被删除
};

class copyable
{

};

}


#endif