#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

#include <functional>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <typeindex>
#include <algorithm>

/**
 * @brief 事件总线模板类（发布-订阅模式）
 */
class EventBus
{
public:
    /**
     * @brief 订阅指定类型的事件
     * @tparam EventType 事件类型（需支持拷贝构造）
     * @param callback 事件回调函数（参数为const EventType&）
     * @return 订阅器对象（析构时自动取消订阅）
     */
    template <typename EventType>
    class EventSubscriber; // 前置声明订阅器

    template <typename EventType>
    EventSubscriber<EventType> subscribe(std::function<void(const EventType&)> callback);

    /**
     * @brief 发布事件（通知所有订阅者）
     * @tparam EventType 事件类型
     * @param event 待发布的事件对象
     */
    template <typename EventType>
    void publish(const EventType& event);

private:
    /**
     * @brief 订阅者基类（类型擦除）
     */
    class ISubscriber
    {
    public:
        virtual ~ISubscriber() = default;
        virtual void notify(const void* event) = 0; // 事件通过void*传递（类型安全由订阅器保证）
    };

    /**
     * @brief 具体事件的订阅者实现
     * @tparam EventType 事件类型
     */
    template <typename EventType>
    class SubscriberImpl : public ISubscriber
    {
    public:
        explicit SubscriberImpl(std::function<void(const EventType&)> callback) : callback_(std::move(callback)) {}

        /**
         * @brief 通知事件（由EventBus调用）
         * @param event 事件指针（需转换为EventType*）
         */
        void notify(const void* event) override
        {
            if (event)
            {
                callback_(*static_cast<const EventType*>(event)); // 安全转换
            }
        }

    private:
        std::function<void(const EventType&)> callback_; // 事件回调
    };

    // 存储订阅者（类型索引 -> 订阅者列表）
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<ISubscriber>>> subscribers_;
    mutable std::mutex mutex_; // 保护订阅者列表的互斥锁
};

/**
 * @brief 事件订阅器（管理生命周期）
 * @tparam EventType 订阅的事件类型
 */
template <typename EventType>
class EventBus::EventSubscriber
{
public:
    using Callback = std::function<void(const EventType&)>; // 回调函数类型

    /**
     * @brief 构造函数（自动订阅事件）
     * @param bus 事件总线引用
     * @param callback 事件回调函数
     */
    EventSubscriber(EventBus& bus, Callback callback) : bus_(bus), subscriber_(std::make_shared<typename EventBus::template SubscriberImpl<EventType>>(std::move(callback)))
    {
        // 加锁保护订阅者列表
        std::lock_guard<std::mutex> lock(bus_.mutex_);
        bus_.subscribers_[typeid(EventType)].push_back(subscriber_);
    }

    /**
     * @brief 析构函数（自动取消订阅）
     */
    ~EventSubscriber()
    {
        // 加锁保护订阅者列表
        std::lock_guard<std::mutex> lock(bus_.mutex_);
        auto it = bus_.subscribers_.find(typeid(EventType));
        if (it != bus_.subscribers_.end())
        {
            auto& list = it->second;
            // 移除当前订阅者
            list.erase(std::remove(list.begin(), list.end(), subscriber_), list.end());

            // 如果列表为空，移除整个类型条目
            if (list.empty())
            {
                bus_.subscribers_.erase(it);
            }
        }
    }

private:
    EventBus& bus_;                             // 事件总线引用
    std::shared_ptr<ISubscriber> subscriber_;  // 底层订阅者实现
};

// EventBus模板方法实现（需放在头文件中）
template <typename EventType>
EventBus::EventSubscriber<EventType> EventBus::subscribe(std::function<void(const EventType&)> callback)
{
    return EventSubscriber<EventType>(*this, std::move(callback));
}

template <typename EventType>
void EventBus::publish(const EventType& event)
{
    // 复制订阅者列表（避免遍历时持有锁）
    std::vector<std::weak_ptr<ISubscriber>> copy;
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = subscribers_.find(typeid(EventType));
        if (it == subscribers_.end()) return; // 无订阅者，直接返回

        // 复制weak_ptr避免阻塞
        for (const auto& sub : it->second)
        {
            copy.emplace_back(sub);
        }
    }

    // 派发事件（在发布者线程执行）
    for (auto& weak_sub : copy)
    {
        // 检查订阅者是否存活
        if (auto sub = weak_sub.lock())
        {
            sub->notify(&event); // 传递事件指针
        }
    }
}

#endif // EVENT_BUS_HPP
