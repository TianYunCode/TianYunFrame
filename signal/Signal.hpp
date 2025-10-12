#pragma once

#include <iostream>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <list>
#include <atomic>
#include <tuple>

#include "Connection.hpp"
#include "Object.h"
#include "ThreadPool.hpp"

template<typename... SignalArgs>
class Signal
{
private:
    // 存储连接的槽信息
    struct ConnectedSlot
    {
        std::function<void(SignalArgs...)> slot;
        std::shared_ptr<typename Connection<SignalArgs...>::ConnectionData> connection_data;
        size_t id;
        ThreadPool *target_thread_pool; // 接收者关联的线程池
    };

    mutable std::shared_mutex mutex_;
    std::list<ConnectedSlot> slots_;
    std::atomic<size_t> current_id_{0};

    // 连接实现辅助函数
    template<typename Callable>
    Connection<SignalArgs...> connect_impl(ThreadPool* pool, Callable&& callable)
    {
        auto con_data = std::make_shared<typename Connection<SignalArgs...>::ConnectionData>();
        con_data->disconnect_callback = [this](size_t slot_id)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex_);
            this->slots_.remove_if([slot_id](const ConnectedSlot & info) { return info.id == slot_id; });
        };

        std::unique_lock<std::shared_mutex> lock(mutex_);
        size_t id = ++current_id_;
        con_data->id = id;

        slots_.emplace_back(ConnectedSlot { std::function<void(SignalArgs...)>(std::forward<Callable>(callable)), con_data, id, pool });

        Connection<SignalArgs...> conn;
        conn.m_connectionData = con_data;

        return conn;
    }


public:
    Signal() = default;
    ~Signal() = default;

    template<typename T>
    Connection<SignalArgs...> connect(T&& slot)
    {
        return connect_impl(nullptr, std::forward<T>(slot));
    }

    // 连接成员函数到 Object 派生类实例
    template<typename T, typename R>
    Connection<SignalArgs...> connect(R* receiver, T&& slot)
    {
        static_assert(std::is_base_of_v<Object, R>, "Receiver must inherit from Object");
        ThreadPool* pool = (receiver) ? receiver->getThreadPool() : nullptr;

        // 将成员函数调用包装成 std::function
        auto wrapper = [receiver, slot](SignalArgs... args)
        {
            (receiver->*slot)(args...);
        };

        return connect_impl(pool, std::move(wrapper));
    }

    // 同步发射信号
    void emit(SignalArgs... args)
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);

        // 拷贝连接列表以避免在迭代时因槽断开而修改列表
        auto local_slots = slots_;
        lock.unlock();

        for(const auto& slot_info : local_slots)
        {
            if(slot_info.connection_data->connected)
            {
                try
                {
                    // 直接调用已包装好的 std::function
                    slot_info.slot(args...);
                }
                catch(...)
                {
                    std::cerr << "[Signal] Exception caught in slot (sync)." << std::endl;
                }
            }
        }
    }

    // 异步发射信号
    void emitAsync(SignalArgs... args)
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);

        // 拷贝连接列表以避免在迭代时因槽断开而修改列表
        auto local_slots = slots_;
        lock.unlock();

        for(const auto& slot_info : local_slots)
        {
            if(slot_info.connection_data->connected)
            {
                // 1. 移动 std::function 副本
                auto slot_copy = std::move(slot_info.slot);

                // 2. 移动参数打包成的 tuple
                auto args_tuple = std::make_tuple(std::forward<SignalArgs>(args)...);

                // 3. 确定目标线程池
                ThreadPool* target_pool = slot_info.target_thread_pool ? slot_info.target_thread_pool : &global_thread_pool;

                // 4. 创建最终的无参任务 lambda 捕获所有需要的数据
                auto task = [slot_copy = std::move(slot_copy), args_tuple = std::move(args_tuple)]() mutable
                {
                    try
                    {
                        // 5. 使用 std::apply 展开 tuple 并调用槽函数
                        std::apply(slot_copy, std::move(args_tuple));
                    }
                    catch(...)
                    {
                        std::cerr << "[Signal] Exception caught in slot (async)." << std::endl;
                    }
                };

                // 6. 将这个无参任务提交到正确的线程池
                try
                {
                    target_pool->push(std::move(task));
                }
                catch(const std::exception& e)
                {
                    std::cerr << "[Signal] Failed to enqueue task: " << e.what() << std::endl;
                }
            }
        }
    }

    size_t getConnectionCount() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return slots_.size();
    }
};

// --- 宏定义 ---
#define SLOT(...) (&__VA_ARGS__)

// --- 全局 connect 函数模板 ---
// 为自由函数/Lambda连接提供便利
template<typename... SigArgs, typename SlotFunc>
Connection<SigArgs...> connect(Signal<SigArgs...> &signal, SlotFunc&& slot)
{
    return signal.connect(std::forward<SlotFunc>(slot));
}

// 通过让编译器推断 ReceiverType 和自动匹配 SignalArgs
template<typename SignalType, typename ReceiverType, typename MemberFuncType>
auto connect(SignalType& signal, ReceiverType* receiver, MemberFuncType slot) -> decltype(signal.connect(receiver, slot))
{
    // 只有当 signal.connect(...) 有效时才启用此重载
    static_assert(std::is_base_of_v<Object, ReceiverType>, "Receiver must inherit from Object");

    return signal.connect(receiver, slot);
}
