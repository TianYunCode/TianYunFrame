#pragma once

#include "ThreadPool.hpp"

static ThreadPool global_thread_pool(64);

class Object
{
public:
    Object() : m_threadPool(&global_thread_pool) {}
    virtual ~Object() = default;

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;

    void setThreadPool(ThreadPool* pool)
    {
        if(pool)
        {
            m_threadPool = pool;
        }
    }

    ThreadPool *getThreadPool() const
    {
        return m_threadPool;
    }

private:
    ThreadPool* m_threadPool;
};
