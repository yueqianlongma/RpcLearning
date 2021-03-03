
#include"TimeTool.h"
using namespace net;

TimeTool::TimeTool()
{
    using std::chrono::high_resolution_clock;
    auto tp = high_resolution_clock::now();
    auto t = high_resolution_clock::to_time_t(tp);
    localtime_r(&t, &tm_);
}

std::string TimeTool::GetFormatTime()
{
    char buffer[32] = {0};
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_);
    return buffer;
}

int TimeTool::GetYear()
{
    return tm_.tm_year + 1900;
}

int TimeTool::GetMonth()
{
    return tm_.tm_mon;
}

int TimeTool::GetDay()
{
    return tm_.tm_mday;
}

int TimeTool::GetHour()
{
    return tm_.tm_hour;
}

int TimeTool::GetMin()
{
    return tm_.tm_min;
}

int TimeTool::GetSec()
{
    return tm_.tm_sec;
}
