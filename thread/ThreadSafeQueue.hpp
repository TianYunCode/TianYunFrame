#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>

/**
 * @brief 线程安全队列模板类
 * @tparam T 队列元素类型（需支持拷贝/移动）
 */
template <typename T>
class ThreadSafeQueue
{
public:
    /**
     * @brief 构造函数（可选容量限制）
     * @param max_size 队列最大容量（0表示无界）
     */
    explicit ThreadSafeQueue(size_t max_size = 0) : max_size_(max_size) {}

    /**
     * @brief 拷贝入队（阻塞直到有空间）
     * @param value 待入队元素
     * @return 成功返回true，队列满返回false（仅当max_size>0时有效）
     */
    bool push(const T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // 容量检查（仅当max_size>0时生效）
        if (max_size_ > 0)
        {
            cond_.wait(lock, [this] { return queue_.size() < max_size_; });
        }

        queue_.push(value);
        cond_.notify_one(); // 通知等待的消费者
        return true;
    }

    /**
     * @brief 移动入队（阻塞直到有空间）
     * @param value 待入队元素（右值引用）
     * @return 成功返回true，队列满返回false（仅当max_size>0时有效）
     */
    bool push(T&& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // 容量检查（仅当max_size>0时生效）
        if (max_size_ > 0)
        {
            cond_.wait(lock, [this] { return queue_.size() < max_size_; });
        }

        queue_.emplace(std::move(value)); // 移动构造避免拷贝
        cond_.notify_one();
        return true;
    }

    /**
     * @brief 尝试非阻塞出队
     * @return 包含元素的std::optional（队列空时返回std::nullopt）
     */
    std::optional<T> try_pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return std::nullopt;
        }

        T value = std::move(queue_.front()); // 移动构造避免拷贝
        queue_.pop();
        return value;
    }

    /**
     * @brief 阻塞出队（带超时）
     * @param timeout 最大等待时间（默认10秒）
     * @return 包含元素的std::optional（超时返回std::nullopt）
     */
    std::optional<T> wait_pop(std::chrono::milliseconds timeout = std::chrono::seconds(10))
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // 等待条件：队列非空 或 超时
        if (!cond_.wait_for(lock, timeout, [this] { return !queue_.empty(); }))
        {
            return std::nullopt; // 超时未获取数据
        }

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    /**
     * @brief 检查队列是否为空（线程安全）
     * @return true为空，false非空
     */
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_); // const成员函数使用lock_guard
        return queue_.empty();
    }

    /**
     * @brief 获取队列大小（线程安全）
     * @return 队列当前元素数量
     */
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex mutex_;          // 互斥锁（mutable允许const成员函数加锁）
    std::condition_variable cond_;      // 条件变量（用于阻塞等待）
    std::queue<T> queue_;               // 底层队列
    size_t max_size_;                   // 最大容量（0表示无界）
};

#endif // THREAD_SAFE_QUEUE_HPP
