#ifndef LOG_H_
#define LOG_H_

#include<stdio.h>
#include<string.h>
#include<errno.h>

#define PRINT(title, fileName, funcName, line, tick, fmt, ...)  \
    printf("[%s] - %s:%s:%d %s, " fmt, title, fileName, funcName, line, tick, ##__VA_ARGS__)
#define PRINT_INFO(title, tick, fmt, ...) \
    PRINT(title, __FILE__, __FUNCTION__, __LINE__, tick, fmt, ##__VA_ARGS__)

#define trace(fmt, ...)  PRINT_INFO("trace", "", fmt, ##__VA_ARGS__)
#define errsys(fmt, ...) PRINT_INFO("error", strerror(errno), fmt, ##__VA_ARGS__)
#endif

