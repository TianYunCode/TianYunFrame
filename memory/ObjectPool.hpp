#pragma once

#include "MemoryPool.hpp"

// 对象池模板类，提供特定类型对象的分配和回收
template <typename T>
class ObjectPool
{
public:
    // 构造函数
    // thread_cache_size: 每个线程本地缓存的对象数量
    // chunk_block_count: 每次扩展时分配的对象数量
    ObjectPool(size_t thread_cache_size = 32, size_t chunk_block_count = 256)
        : pool_(sizeof(T), std::alignment_of_v<T>, thread_cache_size, chunk_block_count)
    {
        static_assert(!std::is_array_v<T>, "ObjectPool does not support array types");
    }

    // 构造一个对象
    // args: 传递给对象构造函数的参数
    template <typename... Args>
    T* Construct(Args&&... args)
    {
        void* mem = pool_.Allocate();
        try
        {
            return new (mem) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            pool_.Deallocate(mem);
            throw;
        }
    }

    // 销毁一个对象
    void Destroy(T* obj)
    {
        if (obj)
        {
            obj->~T();
            pool_.Deallocate(obj);
        }
    }

    // 释放多余的空闲内存
    void ShrinkToFit(size_t min_free_blocks = 64)
    {
        pool_.ShrinkToFit(min_free_blocks);
    }

    // 获取内存统计信息
    MemoryStats GetStats() const
    {
        return pool_.GetStats();
    }

private:
    MemoryPool pool_;  // 底层内存池
};
