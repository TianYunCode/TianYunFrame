#include "DemoHead.h"

#if T_LoggerDemo

#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace logger;

// 模拟一些工作函数
void workerFunction(int id)
{
    LOG_INFO("Worker %d starting", id);

    // 模拟一些工作
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 记录一些调试信息
    LOG_DEBUG("Worker %d processed item %d", id, id * 10);

    // 模拟可能发生的错误
    if (id % 5 == 0)
    {
        LOG_ERROR("Worker %d encountered an error", id);
    }

    LOG_INFO("Worker %d finished", id);
}

// 一个会抛出异常的函数，用于演示 FATAL 日志
void problematicFunction()
{
    try
    {
        // 模拟一个严重错误
        throw std::runtime_error("A critical error occurred!");
    }
    catch (const std::exception& e)
    {
        LOG_FATAL("Caught exception: %s", e.what());
    }
}

int main(int argc, char** argv)
{
    std::cout << "Logger Demo Application" << std::endl;
    std::cout << "=======================" << std::endl;

    // 初始化日志器 - 使用文件大小滚动策略和异步写入模式
    Logger::getInstance()->init(
        StoragePolicy::FILE_SIZE,  // 按文件大小滚动
        WriteMode::ASYNC,          // 异步写入
        "demo_app.log",            // 基础文件名
        1 * 1024 * 1024,           // 最大文件大小: 1MB
        5,                         // 最多保留5个文件
        0                          // 午夜滚动(对于日期策略)
        );

    // 设置日志级别 - 可以从命令行或配置文件中读取
    Logger::getInstance()->setLevel(LogLevel::DEBUG);

    std::cout << "Log level set to: " << levelToString(Logger::getInstance()->getLevel()) << std::endl;

    // 记录一些不同级别的日志
    LOG_TRACE("This is a trace message - usually very detailed");
    LOG_DEBUG("This is a debug message - useful for development");
    LOG_INFO("This is an info message - general application flow");
    LOG_WARN("This is a warning message - something unexpected but handled");
    LOG_ERROR("This is an error message - something failed");

    // 演示多线程日志记录
    std::cout << "\nStarting multi-threaded logging demo..." << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
    {
        threads.emplace_back(workerFunction, i + 1);
    }

    // 等待所有线程完成
    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "Multi-threaded demo completed." << std::endl;

    // 演示致命错误日志记录
    std::cout << "\nDemonstrating fatal error logging..." << std::endl;
    problematicFunction();

    // 演示日志刷新
    std::cout << "Flushing logs..." << std::endl;
    Logger::getInstance()->flush();

    // 更改日志级别并演示过滤
    std::cout << "\nChanging log level to WARN..." << std::endl;
    Logger::getInstance()->setLevel(LogLevel::WARN);

    // 这些消息不会被记录，因为级别低于 WARN
    LOG_TRACE("This trace won't be logged");
    LOG_DEBUG("This debug won't be logged");
    LOG_INFO("This info won't be logged");

    // 这些消息会被记录
    LOG_WARN("This warning will be logged");
    LOG_ERROR("This error will be logged");

    // 重新设置为 INFO 级别
    Logger::getInstance()->setLevel(LogLevel::INFO);

    // 演示大量日志生成（可能触发文件滚动）
    std::cout << "\nGenerating大量日志以演示文件滚动..." << std::endl;
    for (int i = 0; i < 1000; ++i)
    {
        LOG_INFO("Generating log message %d / 1000", i + 1);

        // 偶尔添加一些错误
        if (i % 100 == 0)
        {
            LOG_ERROR("Error at iteration %d", i);
        }
    }

    std::cout << "大量日志生成完成。" << std::endl;

    // 最后的消息
    LOG_INFO("Application shutting down");

    std::cout << "\nDemo completed. Check the generated log files." << std::endl;
    std::cout << "Main log file: demo_app.log" << std::endl;
    std::cout << "Rolled over files: demo_app.log.1, demo_app.log.2, etc." << std::endl;

    return 0;
}

#endif
