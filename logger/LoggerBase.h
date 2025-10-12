#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstdarg>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#endif

namespace logger
{

// 日志级别枚举
enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERR,
    FATAL,
    OFF
};

// 存储模式枚举
enum class StoragePolicy
{
    FILE_SIZE,  // 按文件大小和数量滚动
    DATE_BASED  // 按日期滚动
};

// 写入模式枚举
enum class WriteMode
{
    SYNC,       // 同步写入
    ASYNC       // 异步写入
};

// 控制台颜色枚举
enum class Color
{
    RESET,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BOLD_BLACK,
    BOLD_RED,
    BOLD_GREEN,
    BOLD_YELLOW,
    BOLD_BLUE,
    BOLD_MAGENTA,
    BOLD_CYAN,
    BOLD_WHITE
};

// 将日志级别转换为字符串
inline const char* levelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::TRACE: return "TRACE";
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO";
    case LogLevel::WARN:  return "WARN";
    case LogLevel::ERR: return "ERROR";
    case LogLevel::FATAL: return "FATAL";
    default: return "UNKNOWN";
    }
}

// 获取日志级别对应的颜色
inline Color levelToColor(LogLevel level)
{
    switch (level)
    {
    case LogLevel::TRACE: return Color::CYAN;
    case LogLevel::DEBUG: return Color::MAGENTA;
    case LogLevel::INFO:  return Color::GREEN;
    case LogLevel::WARN:  return Color::YELLOW;
    case LogLevel::ERR: return Color::RED;
    case LogLevel::FATAL: return Color::BOLD_RED;
    default: return Color::RESET;
    }
}

// 获取当前线程ID
inline uint64_t getThreadId()
{
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return static_cast<uint64_t>(pthread_self());
#endif
}

// 日志事件结构
struct LogEvent
{
    std::chrono::system_clock::time_point time;
    LogLevel level;
    std::string message;
    uint64_t threadId;
    std::string functionName;
    std::string fileName;
    int line;

    LogEvent(LogLevel lvl, const std::string& msg, const std::string& func, const std::string& file, int ln)
        : level(lvl), message(msg), functionName(func), fileName(file), line(ln)
    {
        time = std::chrono::system_clock::now();
        threadId = getThreadId();
    }
};

// 日志格式化器接口
class Formatter
{
public:
    virtual ~Formatter() = default;
    virtual std::string format(const LogEvent& event) = 0;
};

// 默认格式化器
class DefaultFormatter : public Formatter
{
public:
    std::string format(const LogEvent& event) override
    {
        std::ostringstream oss;

        // 转换时间为可读格式
        auto t = std::chrono::system_clock::to_time_t(event.time);
        auto tm = *std::localtime(&t);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(event.time.time_since_epoch()) % 1000;

        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << ms.count()
            << " [" << levelToString(event.level) << "]"
            << " [" << event.threadId << "]"
            << " [" << event.functionName << "]";

        if (!event.fileName.empty())
        {
            // 只保留文件名，去掉路径
            size_t pos = event.fileName.find_last_of("/\\");
            std::string shortName = (pos == std::string::npos) ? event.fileName : event.fileName.substr(pos + 1);
            oss << " [" << shortName << ":" << event.line << "]";
        }

        oss << " " << event.message << std::endl;
        return oss.str();
    }
};

// 日志输出目标接口
class Sink
{
public:
    virtual ~Sink() = default;
    virtual void log(const LogEvent& event) = 0;
    virtual void flush() = 0;
    virtual void setFormatter(std::unique_ptr<Formatter> formatter) = 0;
};

// 控制台输出
class ConsoleSink : public Sink
{
public:
    ConsoleSink()
    {
        formatter_ = std::make_unique<DefaultFormatter>();
        initColors();
    }

    void log(const LogEvent& event) override
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // 设置颜色
        setConsoleColor(levelToColor(event.level));

        // 输出日志
        std::cout << formatter_->format(event);

        // 重置颜色
        setConsoleColor(Color::RESET);
    }

    void flush() override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout.flush();
    }

    void setFormatter(std::unique_ptr<Formatter> formatter) override
    {
        formatter_ = std::move(formatter);
    }

private:
    void initColors()
    {
#ifdef _WIN32
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdout != INVALID_HANDLE_VALUE)
        {
            DWORD mode;
            if (GetConsoleMode(hStdout, &mode))
            {
                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hStdout, mode);
            }
        }
#endif
    }

    void setConsoleColor(Color color)
    {
        const char* code = "";
        switch (color)
        {
        case Color::RESET: code = "\033[0m"; break;
        case Color::BLACK: code = "\033[30m"; break;
        case Color::RED: code = "\033[31m"; break;
        case Color::GREEN: code = "\033[32m"; break;
        case Color::YELLOW: code = "\033[33m"; break;
        case Color::BLUE: code = "\033[34m"; break;
        case Color::MAGENTA: code = "\033[35m"; break;
        case Color::CYAN: code = "\033[36m"; break;
        case Color::WHITE: code = "\033[37m"; break;
        case Color::BOLD_BLACK: code = "\033[1;30m"; break;
        case Color::BOLD_RED: code = "\033[1;31m"; break;
        case Color::BOLD_GREEN: code = "\033[1;32m"; break;
        case Color::BOLD_YELLOW: code = "\033[1;33m"; break;
        case Color::BOLD_BLUE: code = "\033[1;34m"; break;
        case Color::BOLD_MAGENTA: code = "\033[1;35m"; break;
        case Color::BOLD_CYAN: code = "\033[1;36m"; break;
        case Color::BOLD_WHITE: code = "\033[1;37m"; break;
        }
        std::cout << code;
    }

    std::mutex mutex_;
    std::unique_ptr<Formatter> formatter_;
};

// 文件输出基类
class FileSink : public Sink
{
public:
    FileSink(const std::string& baseName) : baseName_(baseName)
    {
        formatter_ = std::make_unique<DefaultFormatter>();
    }

    virtual ~FileSink()
    {
        if (file_.is_open())
        {
            file_.close();
        }
    }

    void log(const LogEvent& event) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!file_.is_open() || shouldRollover(event))
        {
            rollover();
        }

        if (file_.is_open())
        {
            std::string formatted = formatter_->format(event);
            file_ << formatted;
            afterWrite(formatted.size()); // 调用afterWrite
        }
    }

    void flush() override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_.is_open())
        {
            file_.flush();
        }
    }

    void setFormatter(std::unique_ptr<Formatter> formatter) override
    {
        formatter_ = std::move(formatter);
    }

protected:
    virtual bool shouldRollover(const LogEvent& event) = 0;
    virtual void rollover() = 0;
    virtual void afterWrite(std::size_t bytesWritten) {};

    std::string baseName_;
    std::ofstream file_;
    std::mutex mutex_;
    std::unique_ptr<Formatter> formatter_;
};

// 按文件大小滚动的文件输出（使用app.log, app.log.1, app.log.2等命名方式）
class SizeBasedFileSink : public FileSink
{
public:
    SizeBasedFileSink(const std::string& baseName, std::size_t maxSize, std::size_t maxFiles) : FileSink(baseName), maxSize_(maxSize), maxFiles_(maxFiles), currentSize_(0) {}

protected:
    bool shouldRollover(const LogEvent& event) override
    {
        if (!file_.is_open())
        {
            return true;
        }

        // 估算写入后的大小
        std::string formatted = formatter_->format(event);
        if (currentSize_ + formatted.size() > maxSize_)
        {
            return true;
        }

        return false;
    }

    void rollover() override
    {
        if (file_.is_open())
        {
            file_.close();
            currentSize_ = 0;
        }

        // 如果基础文件不存在，直接重新打开
        if (!fileExists(baseName_))
        {
            file_.open(baseName_, std::ios::out | std::ios::app);
            if (file_.is_open())
            {
                currentSize_ = getFileSize(baseName_);
            }
            return;
        }

        // 重命名现有文件
        for (int i = maxFiles_ - 1; i > 0; i--)
        {
            std::string oldName = getIndexedFileName(i);
            std::string newName = getIndexedFileName(i + 1);

            if (fileExists(oldName))
            {
                if (fileExists(newName))
                {
                    std::remove(newName.c_str());
                }
                std::rename(oldName.c_str(), newName.c_str());
            }
        }

        // 将当前文件重命名为app.log.1
        std::string firstIndexedName = getIndexedFileName(1);
        if (fileExists(firstIndexedName))
        {
            std::remove(firstIndexedName.c_str());
        }
        std::rename(baseName_.c_str(), firstIndexedName.c_str());

        // 重新打开基础文件
        file_.open(baseName_, std::ios::out | std::ios::app);
        if (file_.is_open())
        {
            currentSize_ = 0;
        }

        // 删除超过数量限制的文件
        std::string lastFileName = getIndexedFileName(maxFiles_);
        if (fileExists(lastFileName))
        {
            std::remove(lastFileName.c_str());
        }
    }

    void afterWrite(std::size_t bytesWritten) override
    {
        currentSize_ += bytesWritten;
    }

private:
    bool fileExists(const std::string& filename)
    {
        std::ifstream f(filename);
        return f.good();
    }

    std::size_t getFileSize(const std::string& filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }

    std::string getIndexedFileName(int index)
    {
        if (index == 0)
        {
            return baseName_;
        }
        return baseName_ + "." + std::to_string(index);
    }

    std::size_t maxSize_;
    std::size_t maxFiles_;
    std::size_t currentSize_;
};

// 按日期滚动的文件输出
class DateBasedFileSink : public FileSink
{
public:
    DateBasedFileSink(const std::string& baseName, int rolloverHour = 0) : FileSink(baseName), rolloverHour_(rolloverHour)
    {
        lastRolloverTime_ = getStartOfDay();
    }

protected:
    bool shouldRollover(const LogEvent& event) override
    {
        if (!file_.is_open())
        {
            return true;
        }

        auto now = std::chrono::system_clock::now();
        if (now >= lastRolloverTime_ + std::chrono::hours(24))
        {
            return true;
        }

        return false;
    }

    void rollover() override
    {
        if (file_.is_open())
        {
            file_.close();
        }

        // 生成基于日期的文件名
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << baseName_ << "." << std::put_time(&tm, "%Y-%m-%d") << ".log";

        file_.open(oss.str(), std::ios::out | std::ios::app);
        lastRolloverTime_ = getStartOfDay() + std::chrono::hours(24);
    }

private:
    std::chrono::system_clock::time_point getStartOfDay()
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&t);
        tm.tm_hour = rolloverHour_;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    int rolloverHour_;
    std::chrono::system_clock::time_point lastRolloverTime_;
};

// 异步日志器
class AsyncLogger
{
public:
    AsyncLogger() : running_(false) {}

    ~AsyncLogger()
    {
        stop();
    }

    void start()
    {
        if (!running_)
        {
            running_ = true;
            thread_ = std::thread(&AsyncLogger::run, this);
        }
    }

    void stop()
    {
        if (running_)
        {
            running_ = false;
            cond_.notify_all();
            if (thread_.joinable())
            {
                thread_.join();
            }

            // 处理剩余日志
            drainQueue();
        }
    }

    void addEvent(std::unique_ptr<LogEvent> event)
    {
        if (!running_)
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(event));
        }

        cond_.notify_one();
    }

    void addSink(std::unique_ptr<Sink> sink)
    {
        sinks_.push_back(std::move(sink));
    }

private:
    void run()
    {
        while (running_)
        {
            std::unique_ptr<LogEvent> event;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_.wait_for(lock, std::chrono::seconds(1), [this]() { return !queue_.empty() || !running_; });

                if (!queue_.empty())
                {
                    event = std::move(queue_.front());
                    queue_.pop();
                }
            }

            if (event)
            {
                for (auto& sink : sinks_)
                {
                    sink->log(*event);
                }

                // 定期刷新
                static int count = 0;
                if (++count % 100 == 0)
                {
                    for (auto& sink : sinks_)
                    {
                        sink->flush();
                    }
                }
            }
        }
    }

    void drainQueue()
    {
        std::unique_ptr<LogEvent> event;
        while (!queue_.empty())
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                event = std::move(queue_.front());
                queue_.pop();
            }

            if (event)
            {
                for (auto& sink : sinks_)
                {
                    sink->log(*event);
                }
            }
        }

        for (auto& sink : sinks_)
        {
            sink->flush();
        }
    }

    std::queue<std::unique_ptr<LogEvent>> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> running_;
    std::thread thread_;
    std::vector<std::unique_ptr<Sink>> sinks_;
};

// 主日志器类
class LoggerBase
{
public:
    LoggerBase() : initialized_(false), level_(LogLevel::INFO) {}
    ~LoggerBase()
    {
        if (writeMode_ == WriteMode::ASYNC)
        {
            asyncLogger_.stop();
        }
    }

    void init(StoragePolicy storagePolicy = StoragePolicy::FILE_SIZE,
              WriteMode writeMode = WriteMode::ASYNC,
              const std::string& baseName = "app",
              std::size_t maxSize = 10 * 1024 * 1024, // 10MB
              std::size_t maxFiles = 10,
              int rolloverHour = 0)
    {
        storagePolicy_ = storagePolicy;
        writeMode_ = writeMode;
        baseName_ = baseName;

        // 创建文件sink
        std::unique_ptr<FileSink> fileSink;
        if (storagePolicy_ == StoragePolicy::FILE_SIZE)
        {
            fileSink = std::make_unique<SizeBasedFileSink>(baseName_, maxSize, maxFiles);
        }
        else
        {
            fileSink = std::make_unique<DateBasedFileSink>(baseName_, rolloverHour);
        }

        // 创建控制台sink
        auto consoleSink = std::make_unique<ConsoleSink>();

        // 设置异步或同步模式
        if (writeMode_ == WriteMode::ASYNC)
        {
            asyncLogger_.addSink(std::move(fileSink));
            asyncLogger_.addSink(std::move(consoleSink));
            asyncLogger_.start();
        }
        else
        {
            sinks_.push_back(std::move(fileSink));
            sinks_.push_back(std::move(consoleSink));
        }

        initialized_ = true;
    }

    void setLevel(LogLevel level)
    {
        level_.store(level, std::memory_order_relaxed);
    }

    LogLevel getLevel() const
    {
        return level_.load(std::memory_order_relaxed);
    }

    bool shouldLog(LogLevel level) const
    {
        return level >= level_.load(std::memory_order_relaxed);
    }

    void log(LogLevel level, const char* function, const char* file, int line, const char* format, ...)
    {
        if (!initialized_ || !shouldLog(level))
        {
            return;
        }

        // 格式化消息
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        auto event = std::make_unique<LogEvent>(level, buffer, function, file, line);

        if (writeMode_ == WriteMode::ASYNC)
        {
            asyncLogger_.addEvent(std::move(event));
        }
        else
        {
            for (auto& sink : sinks_)
            {
                sink->log(*event);
            }

            // 对于ERROR和FATAL级别，立即刷新
            if (level >= LogLevel::ERR)
            {
                flush();
            }
        }
    }

    void flush()
    {
        if (writeMode_ == WriteMode::SYNC)
        {
            for (auto& sink : sinks_)
            {
                sink->flush();
            }
        }

        // 异步模式下flush操作由后台线程处理
    }

private:
    StoragePolicy storagePolicy_;
    WriteMode writeMode_;
    std::string baseName_;
    std::atomic<bool> initialized_;
    std::atomic<LogLevel> level_;
    std::vector<std::unique_ptr<Sink>> sinks_;
    AsyncLogger asyncLogger_;
};
} // namespace logger
