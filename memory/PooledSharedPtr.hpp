#pragma once

#include "ObjectPool.hpp"
#include <memory>

// 支持池化的共享指针模板类
template <typename T>
class PooledSharedPtr
{
public:
    // 创建对象并返回共享指针
    // args: 传递给对象构造函数的参数
    template <typename... Args>
    static std::shared_ptr<T> Create(Args&&... args)
    {
        T* obj = pool_.Construct(std::forward<Args>(args)...);
        return std::shared_ptr<T>(obj, [](T* p) { pool_.Destroy(p); });
    }

    // 获取内存统计信息
    static MemoryStats GetStats()
    {
        return pool_.GetStats();
    }

    // 压缩内存池
    static void ShrinkToFit(size_t min_free_blocks = 64)
    {
        pool_.ShrinkToFit(min_free_blocks);
    }

private:
    static ObjectPool<T> pool_;  // 静态对象池实例
};

// 静态成员初始化
template <typename T>
ObjectPool<T> PooledSharedPtr<T>::pool_;
