#include "logger.h"
#include <iostream>
#include <time.h>

Logger::Logger(){
    //启动专门的写日志线程
    std::thread writeLogTask([&](){
        for(;;){
            //获取当天的日期，取日志并写入日志文件中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

            //有则打开，没有则创建并打开   a+
            FILE *pf = fopen(file_name, "a+");
            if(pf == nullptr){
                std::cout<< "logger file: " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lockqueue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => [%s]", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec, (m_logLevel == INFO ? "INFO" : "ERROR"));
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), pf);

            fclose(pf);
        }
    });

    //设置分离线程，守护线程

    writeLogTask.detach();
}

Logger& Logger::GetInstance(){
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(LogLevel level){
    m_logLevel = level;
}

//把日志信息写入缓冲区
void Logger::Log(std::string msg){
    m_lockqueue.Push(msg);
}