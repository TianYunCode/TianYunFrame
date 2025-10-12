#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <future>
#include <memory>
#include <stdexcept>
#include <iostream>

class ThreadPool
{
public:
    /**
  * @brief 线程池构造函数
  *
  * @param[in] threads 线程数量，默认使用硬件并发线程数
  */
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) : stop(false)
    {
        if(threads == 0)
        {
            threads = 1;
        }

        for (size_t i = 0; i < threads; ++i)
        {
            workers.emplace_back([this]
                                 {
                                     while (true)
                                     {
                                         std::function<void()> task;
                                         {
                                             std::unique_lock<std::mutex> lock(this->queue_mutex);
                                             this->condition.wait(lock, [this]
                                                                  {
                                                                      return this->stop || !this->tasks.empty();
                                                                  });

                                             if (this->stop && this->tasks.empty())
                                             {
                                                 return;
                                             }

                                             task = std::move(this->tasks.front());
                                             this->tasks.pop();
                                         }
                                         try
                                         {
                                             task();
                                         }
                                         catch (...)
                                         {
                                             std::cerr << "Thread Pool: something wrong.";
                                         }
                                     }
                                 });
        }
    }

    /**
  * @brief 将任务添加到线程池队列中执行
  *
  * 此函数接收一个可调用对象和其参数，将其包装成 packaged_task 并加入任务队列，
  * 然后通知工作线程有新任务到来。返回一个 future 对象用于获取任务执行结果。
  *
  * @tparam[in] F 可调用对象的类型
  * @tparam[in] Args 可调用对象参数的类型包
  * @param[in] f 可调用对象
  * @param[in] args 可调用对象的参数包
  *
  * @return std::future<typename std::result_of<F(Args...)>::type> 返回一个 future 对象，可用于获取任务执行结果或等待任务完成
  *
  * @throws std::runtime_error 当线程池已停止时抛出异常
  */
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        // 获取可调用对象的返回类型
        using return_type = decltype(f(args...));

        // 将可调用对象和参数绑定，创建 packaged_task 对象
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            [func = std::forward<F>(f), args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable
            {
                return std::apply(func, std::move(args_tuple));
            });

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if(stop)
            {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]() { (*task)(); });
        }

        condition.notify_one();
        return res;
    }

    template<class F>
    void push(F&& f)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop)
            {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace(std::forward<F>(f));
        }

        condition.notify_one();
    }

    /**
  * @brief 线程池析构函数
  *
  * 负责安全地停止所有工作线程并清理资源
  */
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }

        condition.notify_all();

        for(std::thread &worker : workers)
        {
            if(worker.joinable())
            {
                worker.join();
            }
        }
    }

    /**
  * @brief 获取待处理任务的数量
  *
  * 该函数通过加锁访问任务队列，返回当前队列中等待执行的任务数量。
  *
  * @return size_t 返回任务队列中的任务数量
  */
    size_t pending_tasks()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        return tasks.size();
    }

    /**
  * @brief 阻塞等待所有任务执行完成
  *
  * 此函数会等待任务队列中的所有任务执行完毕，但不会停止线程池接收新任务。
  * 它通过检查任务队列是否为空来确定所有任务是否已完成。
  *
  * @note 此函数不会阻塞其他线程向队列中添加新任务
  */
    void wait_until_empty()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        condition.wait(lock, [this] { return tasks.empty(); });
    }

private:
    std::vector<std::thread> workers;           // 工作线程
    std::queue<std::function<void()>> tasks;    // 任务队列

    std::mutex queue_mutex;                     // 互斥锁, stop, tasks
    std::condition_variable condition;          // 条件变量, queue_mutex
    bool stop;                                  // 线程池是否停止
};
