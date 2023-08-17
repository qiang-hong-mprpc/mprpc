#include <time.h>
#include <cstdio>
#include <iostream>

#include "Logger.h"

Logger::Logger()
{
    std::thread write_log_thread([&](){
        // 写日志
        while(true)
        {
            std::time_t timestamp = std::time(nullptr);
            // 将时间戳转换为 struct tm 结构体
            struct tm *timeInfo = std::localtime(&timestamp);
    
            char file_name[128]; 
            sprintf(file_name,"%d-%d-%d-log.txt",timeInfo->tm_year+1990,timeInfo->tm_mon+1,timeInfo->tm_mday);
            
            FILE *pf = fopen(file_name,"a+");    
            if(pf == nullptr)
            {
                std::cout<<"logger file: "<<file_name<<" open error"<<std::endl;
                exit(EXIT_FAILURE);
            }
            std::string logger_msg = lock_queue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf,"%d:%d:%d => [%s]",timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec,(this->log_level == INFO? "info":"error"));
            logger_msg.insert(0,time_buf);
            logger_msg.append("\n");

            fputs(logger_msg.c_str(),pf);
            fclose(pf);
        } 
    });
}

Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(LoggerLevel logger_level)
{
    this->log_level = logger_level;
}

void Logger::SetLog(std::string msg)
{
    this->lock_queue.PushBack(msg);
}