#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <string>

enum LoggerLevel
{
    INFO,
    ERROR,
};

template <typename T>
class LocKQueue
{
public:

    void PushBack(const T &msg)
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->queue.push(msg);
        condition_var.notify_one();
    }

    T Pop()
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        while (this->queue.empty())
        {
            this->condition_var.wait(lock);
        }
        T data = this->queue.front();
        this->queue.pop();
        return data;
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition_var;
};

class Logger
{
public:
    // 获取日志的单例
    static Logger &GetInstance();
    void SetLogLevel(LoggerLevel logger_level);
    void SetLog(std::string msg);

private:
    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    LoggerLevel log_level;
    LocKQueue<std::string> lock_queue;
};

#define LOG_INFO(logmsgforamt, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgforamt, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0);

#define LOG_ERROR(logmsgforamt, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgforamt, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0);