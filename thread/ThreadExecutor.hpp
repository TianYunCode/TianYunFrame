#ifndef THREAD_EXECUTOR_HPP
#define THREAD_EXECUTOR_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <stdexcept>
#include <atomic>
#include <future>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

/**
 * @brief 线程任务执行器（单线程任务队列）
 */
class ThreadExecutor
{
public:
    using Task = std::function<void()>; // 任务类型（无参数无返回值）

    /**
     * @brief 构造函数（可选线程名称）
     * @param name 线程名称（用于调试，Windows/Linux不同实现）
     */
    explicit ThreadExecutor(const std::string& name = "") : name_(name), running_(false) {}

    /**
     * @brief 析构函数（自动停止并等待线程退出）
     */
    ~ThreadExecutor()
    {
        stop();
        if (thread_.joinable())
        {
            thread_.join(); // 等待线程结束
        }
    }

    /**
     * @brief 启动线程（必须调用post前调用）
     */
    void start()
    {
        if (!running_.exchange(true))
        {
            thread_ = std::thread(&ThreadExecutor::run, this); // 启动工作线程
            set_thread_name(name_); // 设置线程名称（跨平台）
        }
    }

    /**
     * @brief 停止线程（不再接受新任务，执行完剩余任务后退出）
     */
    void stop()
    {
        if (running_.exchange(false))
        {
            cond_.notify_one(); // 唤醒等待的线程
        }
    }

    /**
     * @brief 提交任务（异步执行）
     * @param task 待执行的任务
     * @throw std::runtime_error 线程未启动时抛出异常
     */
    void post(Task task)
    {
        if (!running_.load())
        {
            throw std::runtime_error("ThreadExecutor not started");
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push(std::move(task)); // 移动任务到队列
        }

        cond_.notify_one(); // 通知工作线程有新任务
    }

    /**
     * @brief 提交任务并等待完成（同步执行）
     * @param task 待执行的任务
     * @throw std::runtime_error 线程未启动时抛出异常
     */
    void postAndWait(Task task)
    {
        if (!running_.load())
        {
            throw std::runtime_error("ThreadExecutor not started");
        }

        // 使用shared_ptr包装promise，使其可拷贝
        auto promise_ptr = std::make_shared<std::promise<void>>();
        std::future<void> future = promise_ptr->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);

            // 使用可拷贝的lambda包装器
            tasks_.push([task = std::move(task), promise_ptr]() mutable
                        {
                            try
                            {
                                task(); // 执行原始任务
                                promise_ptr->set_value(); // 通知等待完成
                            }
                            catch (...)
                            {
                                promise_ptr->set_exception(std::current_exception()); // 传递异常
                            }
                        });
        }

        cond_.notify_one(); // 通知工作线程
        future.wait(); // 阻塞当前线程直到任务完成
    }

    /**
     * @brief 提交带返回值的任务并等待结果（同步执行）
     * @tparam ResultType 任务返回值类型
     * @param task 待执行的任务
     * @return 任务返回值
     * @throw std::runtime_error 线程未启动时抛出异常
     */
    template <typename ResultType>
    std::future<ResultType> postAndWaitWithResult(std::function<ResultType()> task)
    {
        if (!running_.load())
        {
            throw std::runtime_error("ThreadExecutor not started");
        }

        auto promise_ptr = std::make_shared<std::promise<ResultType>>();
        std::future<ResultType> future = promise_ptr->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push([task = std::move(task), promise_ptr]() mutable
                        {
                            try
                            {
                                ResultType result = task(); // 执行原始任务
                                promise_ptr->set_value(result); // 设置结果
                            }
                            catch (...)
                            {
                                promise_ptr->set_exception(std::current_exception()); // 传递异常
                            }
                        });
        }

        cond_.notify_one(); // 通知工作线程
        return future;
    }

private:
    /**
     * @brief 工作线程主函数（循环执行任务）
     */
    void run()
    {
        while (running_.load())
        {
            Task task;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                // 等待条件：线程停止 或 有新任务
                cond_.wait(lock, [this] { return !running_.load() || !tasks_.empty(); });

                // 退出条件：线程停止且任务队列为空
                if (!running_.load() && tasks_.empty())
                {
                    break;
                }

                if (!tasks_.empty())
                {
                    task = std::move(tasks_.front()); // 取出任务
                    tasks_.pop();
                }
            }

            if (task)
            {
                try
                {
                    task(); // 执行任务（可能抛出异常）
                }
                catch (const std::exception& e)
                {
                    // 此处可集成日志模块记录异常（示例：输出到控制台）
                    fprintf(stderr, "Task execution failed: %s\n", e.what());
                }
            }
        }
    }

    /**
     * @brief 跨平台设置线程名称（用于调试）
     * @param name 线程名称（最大长度：Linux 16字节，Windows 256字节）
     */
    void set_thread_name(const std::string& name)
    {
        if (name.empty()) return;

#ifdef _WIN32
        typedef HRESULT (WINAPI *SetThreadDescriptionFn)(HANDLE, PCWSTR);
        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        if (kernel32)
        {
            auto setThreadDescription = reinterpret_cast<SetThreadDescriptionFn>(GetProcAddress(kernel32, "SetThreadDescription"));
            if (setThreadDescription)
            {
                // 转换字符串为宽字符
                int wideSize = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
                if (wideSize > 0)
                {
                    std::vector<wchar_t> wideName(wideSize);
                    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wideName.data(), wideSize);
                    setThreadDescription(thread_.native_handle(), wideName.data());
                }
            }
        }
#else
        // Linux：使用pthread_setname_np（最大16字节）
        std::string truncated_name = name.substr(0, 15); // 截断到15字节
        pthread_setname_np(thread_.native_handle(), truncated_name.c_str());
#endif
    }

    std::string name_;              // 线程名称
    std::thread thread_;            // 工作线程
    std::mutex mutex_;              // 保护任务队列的互斥锁
    std::condition_variable cond_;  // 条件变量（用于任务通知）
    std::queue<Task> tasks_;        // 任务队列
    std::atomic<bool> running_;     // 线程运行状态（原子操作）
};

#endif // THREAD_EXECUTOR_HPP
