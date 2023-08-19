#pragma once
#include "lockQueue.h"
#include <string>

// mprpc框架的日志系统

enum LogLevel{
    INFO, //普通信息
    ERROR,//错误信息
};

class Logger {
public:
    //获取日志单例
    static Logger& GetInstance(); 
    void SetLogLevel(LogLevel level);
    void Log(std::string msg);
private:
    int m_logLevel;
    LockQueue<std::string> m_lockqueue;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

//定义宏
#define LOG_INFO(logmsgformat, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0);
    
#define LOG_ERROR(logmsgformat, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0);