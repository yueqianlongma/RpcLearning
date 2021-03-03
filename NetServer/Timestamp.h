
#ifndef _NETSERVER_TIME_STAMP_H__
#define _NETSERVER_TIME_STAMP_H__

#include<chrono>

namespace net
{

using   std::chrono::high_resolution_clock;
using   std::chrono::time_point_cast;
typedef std::chrono::nanoseconds    NanoSecond;
typedef std::chrono::microseconds   MicroSecond;
typedef std::chrono::milliseconds   MilliSecond;
typedef std::chrono::seconds        Second;
typedef std::chrono::minutes        Minute;
typedef std::chrono::hours          Hour;
typedef std::chrono::time_point 
                <high_resolution_clock, NanoSecond> Timestamp;

// time_point_cast<NanoSecond>(high_resolution_clock::now());
// high_resolution_clock::now()在linux系统上返回的默认类型是纳秒的
inline Timestamp now()
{
    return high_resolution_clock::now();
}

inline Timestamp nowAfter(NanoSecond interval)
{
    return  now() + interval;
}

inline Timestamp nowBefore(NanoSecond interval)
{
    return  now() - interval;
}

}

#endif