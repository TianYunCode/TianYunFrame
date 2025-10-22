#pragma once

#include "LoggerBase.h"
#include "Singleton.hpp"

namespace logger
{
class Logger : public logger::LoggerBase, public Singleton<Logger>
{
    friend class Singleton<Logger>; // 允许单例模板访问私有构造函数

public:
    ~Logger(){}

private:
    explicit Logger(){}
};

// 辅助宏
#define LOG_INTERNAL(level, format, ...) \
logger::Logger::getInstance()->log(level, __FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_TRACE(format, ...) LOG_INTERNAL(logger::LogLevel::TRACE, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG_INTERNAL(logger::LogLevel::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  LOG_INTERNAL(logger::LogLevel::INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  LOG_INTERNAL(logger::LogLevel::WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_INTERNAL(logger::LogLevel::ERR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) LOG_INTERNAL(logger::LogLevel::FATAL, format, ##__VA_ARGS__)
}

