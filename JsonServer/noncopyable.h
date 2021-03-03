#ifndef __JSONLEARN_NONCOPYABLE_H__
#define __JSONLEARN_NONCOPYABLE_H__

namespace json
{

class noncopyable
{
public:
    noncopyable(noncopyable&) = delete;
    noncopyable& operator=(noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};


}

#endif