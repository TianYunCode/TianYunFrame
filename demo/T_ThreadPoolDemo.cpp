#include "DemoHead.h"

#if T_ThreadDemo

#include "ThreadPool.hpp"
#include <iostream>
#include <chrono>
#include <random>

// 示例函数：计算两个数的和
int add(int a, int b)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟耗时操作
    return a + b;
}

// 示例函数：生成指定范围内的随机数
int random_number(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// 无返回值的示例函数
void print_message(const std::string& msg)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Message: " << msg << std::endl;
}

int main()
{
    // 创建线程池，使用默认线程数（硬件并发数）
    ThreadPool pool;

    std::cout << "Thread pool created with " << std::thread::hardware_concurrency() << " threads" << std::endl;

    // 示例1：提交有返回值的任务
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; ++i)
    {
        // 使用enqueue提交任务并获取future
        futures.emplace_back(pool.enqueue(add, i, i * 2));
    }

    // 获取任务结果
    for (size_t i = 0; i < futures.size(); ++i)
    {
        std::cout << "Task " << i << " result: " << futures[i].get() << std::endl;
    }

    // 示例2：提交无返回值的任务
    for (int i = 0; i < 5; ++i)
    {
        pool.push([i]()
                  {
                      print_message("Hello from task " + std::to_string(i));
                  });
    }

    // 示例3：批量提交随机数生成任务
    std::vector<std::future<int>> random_futures;
    for (int i = 0; i < 8; ++i)
    {
        random_futures.emplace_back(pool.enqueue(random_number, 1, 100));
    }

    // 等待所有任务完成
    std::cout << "Waiting for all tasks to complete..." << std::endl;
    pool.wait_until_empty();

    // 获取随机数结果
    std::cout << "Random numbers: ";
    for (auto& fut : random_futures)
    {
        std::cout << fut.get() << " ";
    }
    std::cout << std::endl;

    // 示例4：查看剩余任务数
    std::cout << "Pending tasks: " << pool.pending_tasks() << std::endl;

    // 线程池会在析构时自动等待所有任务完成
    std::cout << "Exiting..." << std::endl;

    return 0;
}

#endif
