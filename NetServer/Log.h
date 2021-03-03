#ifndef __NETSERVER_LOG__H__
#define __NETSERVER_LOG__H__
#include"TimeTool.h"
#include<cstdarg>
#include<cstdlib>

inline void log(int to_abort, const char* level, const char* filename, const char* function, uint32_t line, const char* fmt ...)
{
    va_list list;
    va_start(list, fmt);

    char log_str[512];
    int curlen = snprintf(log_str, sizeof(log_str), "[%s] [%s] [%s-%s-%d]", 
                                                                        net::TimeTool().GetFormatTime().c_str(),
                                                                        level,
                                                                        filename,
                                                                        function,
                                                                        line);
    int nextlen = vsnprintf(log_str + curlen, sizeof(log_str) - curlen, fmt, list);

    if(curlen + nextlen >= sizeof(log_str))
        *(log_str + sizeof(log_str) - 1) = '\0';

    va_end(list);
    printf("%s\n", log_str);

    if(to_abort)
        abort();
}    

    
#define PRINT(to_abort, level, fmt, ...)  \
                        log (to_abort, "info", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);
        

#define trace(fmt, ...)      PRINT(0, "trace", fmt, ##__VA_ARGS__)

#define debug(fmt, ...)      PRINT(0, "debug", fmt, ##__VA_ARGS__)

#define info(fmt,  ...)      PRINT(0, "info", fmt,  ##__VA_ARGS__)

#define warn(fmt,  ...)      PRINT(0, "warn", fmt,  ##__VA_ARGS__)

#define error(fmt, ...)      PRINT(0, "error", fmt, ##__VA_ARGS__)

#define fatal(fmt, ...)      PRINT(1,  "error", fmt, ##__VA_ARGS__)




#endif