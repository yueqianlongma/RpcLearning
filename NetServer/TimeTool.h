
#ifndef __NETSERVER_TIMETOOL_H__
#define __NETSERVER_TIMETOOL_H__

#include<chrono>
#include<ctime>
#include<string>

/**
 * 时间戳工具
 * 
 */

namespace net
{

class TimeTool
{
public:
    TimeTool();
    std::string GetFormatTime();
    int GetYear();
    int GetMonth();
    int GetDay();
    int GetHour();
    int GetMin();
    int GetSec();

private:
    std::tm tm_;
};


}

#endif