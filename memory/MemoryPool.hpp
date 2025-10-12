#pragma once

#include <mutex>
#include <vector>
#include <cstddef>
#include <iostream>
#include <atomic>
#include <thread>

// 内存池调试模式开关
// #define MEMORY_POOL_DEBUG

// 线程安全的日志输出宏（用于调试）
#ifdef MEMORY_POOL_DEBUG
#define POOL_LOG(msg) do { \
std::lock_guard<std::mutex> log_lock(log_mutex_); \
    std::cout << "[MemoryPool] " << msg << std::endl; \
} while(0)
#else
#define POOL_LOG(msg)
#endif

    // 内存统计数据结构
    struct MemoryStats
    {
        size_t allocated_blocks = 0;        // 已分配块数
        size_t free_blocks = 0;             // 空闲块数
        size_t total_chunks = 0;            // 内存块数量
        size_t total_memory = 0;            // 总内存占用(bytes)
        size_t thread_cache_blocks = 0;     // 线程缓存块数

        // 打印内存统计信息
        void Print() const
        {
            std::cout << "Memory Stats:\n"
                      << "  Allocated Blocks: " << allocated_blocks << "\n"
                      << "  Free Blocks: " << free_blocks << "\n"
                      << "  Thread Cache Blocks: " << thread_cache_blocks << "\n"
                      << "  Total Chunks: " << total_chunks << "\n"
                      << "  Total Memory: " << total_memory / 1024 << " KB\n";
        }
    };

// 内存池类，提供固定大小的内存块分配和回收
class MemoryPool
{
public:
    // 构造函数
    // block_size: 每个内存块的大小
    // alignment: 内存对齐要求
    // thread_cache_size: 每个线程本地缓存的内存块数量
    // chunk_block_count: 每次扩展时分配的内存块数量
    explicit MemoryPool(size_t block_size, size_t alignment = alignof(std::max_align_t), size_t thread_cache_size = 32, size_t chunk_block_count = 256)
        : alignment_(alignment),block_size_(CalculateAlignedSize(block_size, alignment)),chunk_block_count_(chunk_block_count),thread_cache_size_(thread_cache_size)
    {
        Expand();  // 初始化时先扩展一次内存池
    }

    // 析构函数，释放所有内存
    ~MemoryPool()
    {
        ShrinkToFit(0);  // 释放所有内存
#ifdef MEMORY_POOL_DEBUG
        auto stats = GetStats();
        std::cout << "\nMemoryPool Destruction Stats:\n";
        stats.Print();
#endif
    }

    // 分配一个内存块
    void* Allocate()
    {
        // 优先从线程本地缓存分配
        if (!local_cache_.empty())
        {
            void* ptr = local_cache_.back();
            local_cache_.pop_back();
#ifdef MEMORY_POOL_DEBUG
            ++alloc_count_;
#endif
            return ptr;
        }

        // 本地缓存为空，从中央池补充
        RefillLocalCache();

        if (!local_cache_.empty())
        {
            void* ptr = local_cache_.back();
            local_cache_.pop_back();
            return ptr;
        }

        // 中央池也无可用内存（理论上不会发生）
        throw std::bad_alloc();
    }

    // 回收一个内存块
    void Deallocate(void* ptr)
    {
        if (!ptr) return;

        // 如果有全局回收请求，直接归还到中央池
        if (memory_reclaim_requested_.load(std::memory_order_acquire))
        {
            std::lock_guard<std::mutex> lock(central_mutex_);
            Node* node = static_cast<Node*>(ptr);
            node->next = free_list_;
            free_list_ = node;
            ++free_blocks_;
            POOL_LOG("Thread " << std::this_thread::get_id() << " returned block due to reclaim request");
            return;
        }

        // 优先放入线程本地缓存
        if (local_cache_.size() < thread_cache_size_)
        {
            local_cache_.push_back(ptr);
            return;
        }

        // 本地缓存已满，批量归还中央池
        ReturnLocalCache();
        local_cache_.push_back(ptr);
    }

    // 释放多余的空闲内存
    // min_free_blocks: 需要保留的最小空闲块数
    void ShrinkToFit(size_t min_free_blocks = 64)
    {
        // 首先收集所有可用空闲块
        CollectFreeBlocks();

        std::lock_guard<std::mutex> lock(central_mutex_);

        // 计算需要保留的空闲块
        const size_t target_free = min_free_blocks;
        if (free_blocks_ <= target_free)
        {
            return;
        }

        // 释放多余的内存块
        size_t blocks_to_free = free_blocks_ - target_free;
        size_t chunks_to_free = (blocks_to_free + chunk_block_count_ - 1) / chunk_block_count_;
        chunks_to_free = std::min(chunks_to_free, chunks_.size());

        POOL_LOG("Shrinking: freeing " << chunks_to_free << " chunks");

        for (size_t i = 0; i < chunks_to_free; ++i)
        {
            if (!chunks_.empty())
            {
                void* chunk = chunks_.back();
                chunks_.pop_back();
                operator delete(chunk);

                free_blocks_ = (free_blocks_ > chunk_block_count_) ? free_blocks_ - chunk_block_count_ : 0;
            }
        }

        // 重建空闲链表
        RebuildFreeList();
    }

    // 获取内存统计信息
    MemoryStats GetStats() const
    {
        MemoryStats stats;
        {
            std::lock_guard<std::mutex> lock(central_mutex_);
            stats.allocated_blocks = chunk_block_count_ * chunks_.size() - free_blocks_;
            stats.free_blocks = free_blocks_;
            stats.total_chunks = chunks_.size();
            stats.total_memory = chunks_.size() * block_size_ * chunk_block_count_;
        }

        // 线程本地缓存统计（近似值）
        stats.thread_cache_blocks = thread_cache_size_ * active_threads_.load();
        stats.free_blocks += stats.thread_cache_blocks;

        return stats;
    }

    // 禁用拷贝和赋值
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

private:
    // 链表节点结构
    struct Node
    {
        Node* next;
    };

    // 计算对齐后的块大小
    static size_t CalculateAlignedSize(size_t block_size, size_t alignment)
    {
        return (block_size + alignment - 1) & ~(alignment - 1);
    }

    // 扩展中央内存池
    void Expand()
    {
        std::lock_guard<std::mutex> lock(central_mutex_);

        size_t chunk_size = block_size_ * chunk_block_count_;
        char* chunk = static_cast<char*>(operator new(chunk_size));
        chunks_.push_back(chunk);

        POOL_LOG("Expanding: allocated new chunk of " << chunk_size / 1024 << " KB");

        // 添加到空闲链表
        for (size_t i = 0; i < chunk_block_count_; ++i)
        {
            Node* node = reinterpret_cast<Node*>(chunk + i * block_size_);
            node->next = free_list_;
            free_list_ = node;
        }

        free_blocks_ += chunk_block_count_;
    }

    // 填充线程本地缓存
    void RefillLocalCache()
    {
        TrackThreadActivity();

        std::lock_guard<std::mutex> lock(central_mutex_);

        // 中央池无空闲内存时扩展
        if (free_list_ == nullptr)
        {
            Expand();
        }

        // 批量填充本地缓存
        size_t fetch_count = std::min(thread_cache_size_, free_blocks_);
        for (size_t i = 0; i < fetch_count; ++i)
        {
            Node* node = free_list_;
            free_list_ = free_list_->next;
            local_cache_.push_back(node);
            --free_blocks_;
        }
    }

    // 归还本地缓存到中央池
    void ReturnLocalCache()
    {
        if (local_cache_.empty()) return;

        std::lock_guard<std::mutex> lock(central_mutex_);

        for (void* ptr : local_cache_)
        {
            Node* node = static_cast<Node*>(ptr);
            node->next = free_list_;
            free_list_ = node;
            ++free_blocks_;
        }

        POOL_LOG("Thread " << std::this_thread::get_id() << " returned " << local_cache_.size() << " blocks");

        local_cache_.clear();
    }

    // 重建空闲链表
    void RebuildFreeList()
    {
        free_list_ = nullptr;
        free_blocks_ = 0;

        for (void* chunk : chunks_)
        {
            for (size_t i = 0; i < chunk_block_count_; ++i)
            {
                Node* node = reinterpret_cast<Node*>(static_cast<char*>(chunk) + i * block_size_);
                node->next = free_list_;
                free_list_ = node;
                ++free_blocks_;
            }
        }
    }

    // 收集所有空闲块到中央池
    void CollectFreeBlocks()
    {
        // 1. 收集中央池现有的空闲块
        RebuildFreeList();

        // 2. 收集当前线程的本地缓存
        if (!local_cache_.empty())
        {
            std::lock_guard<std::mutex> lock(central_mutex_);
            for (void* ptr : local_cache_)
            {
                Node* node = static_cast<Node*>(ptr);
                node->next = free_list_;
                free_list_ = node;
                ++free_blocks_;
            }
            POOL_LOG("Thread " << std::this_thread::get_id() << " returned " << local_cache_.size() << " blocks");
            local_cache_.clear();
        }

        // 3. 请求其他线程归还本地缓存
        RequestOtherThreadsReturn();
    }

    // 请求其他线程归还本地缓存
    void RequestOtherThreadsReturn()
    {
        POOL_LOG("Requesting other threads to return cache blocks");

        // 标记全局需要回收内存
        memory_reclaim_requested_.store(true, std::memory_order_release);

        // 给其他线程时间响应请求
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // 重置请求标志
        memory_reclaim_requested_.store(false, std::memory_order_release);
    }

    // 跟踪线程活动
    void TrackThreadActivity()
    {
        static thread_local bool is_tracked = false;
        if (!is_tracked)
        {
            active_threads_.fetch_add(1, std::memory_order_relaxed);
            is_tracked = true;

            // 注册线程退出回调
            static thread_local struct ThreadExitNotifier
            {
                ~ThreadExitNotifier()
                {
                    if (pool_)
                    {
                        // 归还当前线程缓存
                        pool_->ReturnCurrentThreadCache();
                        pool_->active_threads_.fetch_sub(1, std::memory_order_relaxed);
                    }
                }
                MemoryPool* pool_ = nullptr;
            } notifier;

            notifier.pool_ = this;
        }
    }

    // 归还当前线程缓存
    void ReturnCurrentThreadCache()
    {
        if (local_cache_.empty()) return;

        std::lock_guard<std::mutex> lock(central_mutex_);

        for (void* ptr : local_cache_)
        {
            Node* node = static_cast<Node*>(ptr);
            node->next = free_list_;
            free_list_ = node;
            ++free_blocks_;
        }

        POOL_LOG("Thread " << std::this_thread::get_id() << " exited and returned " << local_cache_.size() << " blocks");

        local_cache_.clear();
    }

    // 中央池成员
    size_t alignment_;           // 内存对齐要求
    size_t block_size_;          // 对齐后的块大小
    size_t chunk_block_count_;   // 每块内存包含的块数
    size_t thread_cache_size_;   // 线程本地缓存大小

    Node* free_list_ = nullptr;  // 中央空闲链表
    size_t free_blocks_ = 0;     // 中央空闲块计数
    std::vector<void*> chunks_;  // 分配的内存块
    mutable std::mutex central_mutex_; // 中央池互斥锁

    // 线程本地缓存
    thread_local static std::vector<void*> local_cache_;

    // 活动线程计数
    std::atomic<size_t> active_threads_{0};

    // 内存回收请求标志
    std::atomic<bool> memory_reclaim_requested_{false};

#ifdef MEMORY_POOL_DEBUG
    mutable std::atomic<size_t> alloc_count_{0};
    static std::mutex log_mutex_;
#endif
};

// 静态成员初始化
thread_local std::vector<void*> MemoryPool::local_cache_;
#ifdef MEMORY_POOL_DEBUG
std::mutex MemoryPool::log_mutex_;
#endif
