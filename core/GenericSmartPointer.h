#pragma once

#ifndef GENERIC_SMART_POINTER_H
#define GENERIC_SMART_POINTER_H

#include <atomic>           // 引入原子操作库，用于线程安全的引用计数
#include <utility>          // 引入工具库，用于完美转发和std::swap
#include <cassert>          // 引入断言库，用于运行时检查
#include <functional>       // 用于std::hash特化

/**
 * @brief 通用智能指针模板类
 * @details 支持任意C++类型，实现线程安全的引用计数，接口风格对齐vtkSmartPointer
 *          特性：线程安全、支持拷贝/移动语义、兼容STL、禁止裸指针误用、支持const类型
 * @tparam T 指针指向的模板类型（普通类型）
 */
template <typename T>
class GenericSmartPointer
{
public:
    /**
     * @brief 默认构造函数
     * @note 初始化为空指针，引用计数为nullptr
     */
    GenericSmartPointer() noexcept : m_ptr(nullptr), m_refCount(nullptr)
    {
    }

    /**
     * @brief nullptr_t构造函数
     * @note 通过委托给默认构造函数实现，统一空指针初始化逻辑
     */
    GenericSmartPointer(std::nullptr_t) noexcept : GenericSmartPointer()
    {
    }

    /**
     * @brief 接收原始指针的构造函数
     * @param ptr 原始指针（指向已动态分配的对象）
     * @note explicit关键字禁止隐式转换，避免意外将裸指针转为智能指针
     */
    explicit GenericSmartPointer(T* ptr) noexcept : m_ptr(ptr)
    {
        if (m_ptr != nullptr)
        {
            // 新建引用计数，初始值1（relaxed内存序：仅计数增减，无依赖关系）
            m_refCount = new std::atomic<int>(1);
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param other 其他智能指针对象（左值）
     * @note 共享对象所有权，引用计数原子+1
     */
    GenericSmartPointer(const GenericSmartPointer& other) noexcept : m_ptr(other.m_ptr), m_refCount(other.m_refCount)
    {
        if (m_refCount != nullptr)
        {
            m_refCount->fetch_add(1, std::memory_order_relaxed);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 其他智能指针对象（右值）
     * @note 转移对象所有权，不修改引用计数，原对象置空
     */
    GenericSmartPointer(GenericSmartPointer&& other) noexcept : m_ptr(other.m_ptr), m_refCount(other.m_refCount)
    {
        other.m_ptr = nullptr;
        other.m_refCount = nullptr;
    }

    /**
     * @brief 析构函数
     * @note 调用reset()释放资源，确保引用计数正确递减和内存释放
     */
    ~GenericSmartPointer() noexcept
    {
        reset();
    }

    /**
     * @brief 拷贝赋值运算符
     * @param other 其他智能指针对象（左值）
     * @return 自身引用
     * @note 先释放当前资源，再共享目标对象所有权，引用计数+1
     */
    GenericSmartPointer& operator=(const GenericSmartPointer& other) noexcept
    {
        if (this != &other)
        {
            reset();
            m_ptr = other.m_ptr;
            m_refCount = other.m_refCount;
            if (m_refCount != nullptr)
            {
                m_refCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param other 其他智能指针对象（右值）
     * @return 自身引用
     * @note 先释放当前资源，再转移目标对象所有权，原对象置空
     */
    GenericSmartPointer& operator=(GenericSmartPointer&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            m_ptr = other.m_ptr;
            m_refCount = other.m_refCount;
            other.m_ptr = nullptr;
            other.m_refCount = nullptr;
        }
        return *this;
    }

    /**
     * @brief nullptr赋值运算符
     * @return 自身引用
     * @note 将智能指针置空，释放当前持有资源
     */
    GenericSmartPointer& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    /**
     * @brief 禁止裸指针直接赋值
     * @note 避免误将裸指针赋值给智能指针，导致重复释放或内存泄漏
     */
    GenericSmartPointer& operator=(T*) noexcept = delete;

    /**
     * @brief 获取原始指针
     * @return 指向实际对象的原始指针（非所有权转移）
     * @note 谨慎使用：避免用原始指针手动释放或赋值给其他智能指针
     */
    T* get() const noexcept { return m_ptr; }

    /**
     * @brief 重置智能指针
     * @param ptr 可选的新指针（默认为nullptr）
     * @note 1. 释放当前持有资源（引用计数-1，为0则删除对象和计数）
     *       2. 若传入新指针，接管其所有权（引用计数初始化为1）
     *       优化点：先断开自身引用再处理旧资源，避免多线程下重复操作
     */
    void reset(T* ptr = nullptr) noexcept
    {
        // 保存旧资源，先断开自身引用
        T* oldPtr = m_ptr;
        std::atomic<int>* oldRef = m_refCount;
        m_ptr = nullptr;
        m_refCount = nullptr;

        // 处理旧资源：引用计数-1，为0则释放
        if (oldRef != nullptr)
        {
            // acquire内存序：确保析构时能看到对象的所有修改
            if (oldRef->fetch_sub(1, std::memory_order_acquire) == 1)
            {
                delete oldPtr;         // 释放普通对象
                delete oldRef;        // 释放引用计数
            }
        }

        // 接管新资源（若有）
        m_ptr = ptr;
        if (m_ptr != nullptr)
        {
            m_refCount = new std::atomic<int>(1);
        }
    }

    /**
     * @brief 交换两个智能指针的内容
     * @param other 要交换的另一个智能指针
     * @note 无内存分配，O(1)操作，线程安全（仅交换指针，不修改计数）
     */
    void swap(GenericSmartPointer& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_refCount, other.m_refCount);
    }

    /**
     * @brief 获取当前引用计数
     * @return 当前引用计数值（0表示空指针）
     * @note 主要用于调试和日志，relaxed内存序不保证跨线程实时性
     */
    int getReferenceCount() const noexcept
    {
        return (m_refCount != nullptr) ? m_refCount->load(std::memory_order_relaxed) : 0;
    }

    /**
     * @brief 重载解引用运算符
     * @return 对实际对象的引用
     * @note 空指针解引用会触发断言（调试模式），release模式下行为未定义
     */
    T& operator*() const noexcept
    {
        assert(m_ptr != nullptr && "Dereferencing null GenericSmartPointer!");
        return *m_ptr;
    }

    /**
     * @brief 重载成员访问运算符
     * @return 指向实际对象的原始指针
     * @note 空指针访问成员会触发断言（调试模式），release模式下行为未定义
     */
    T* operator->() const noexcept
    {
        assert(m_ptr != nullptr && "Accessing member of null GenericSmartPointer!");
        return m_ptr;
    }

    /**
     * @brief 类型转换运算符（显式）
     * @return 是否非空（true=持有对象，false=空指针）
     * @note 显式转换避免意外类型转换，支持if (sp)、if (!sp)判断
     */
    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    /**
     * @brief 比较运算符（与另一个智能指针比较）
     * @param other 要比较的另一个智能指针
     * @return 是否指向同一对象
     */
    bool operator==(const GenericSmartPointer& other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    /**
     * @brief 不等于比较运算符（与另一个智能指针比较）
     * @param other 要比较的另一个智能指针
     * @return 是否指向不同对象
     */
    bool operator!=(const GenericSmartPointer& other) const noexcept
    {
        return !(*this == other);
    }

    /**
     * @brief 比较运算符（与nullptr比较）
     * @return 是否为空指针
     */
    bool operator==(std::nullptr_t) const noexcept
    {
        return m_ptr == nullptr;
    }

    /**
     * @brief 不等于比较运算符（与nullptr比较）
     * @return 是否非空指针
     */
    bool operator!=(std::nullptr_t) const noexcept
    {
        return m_ptr != nullptr;
    }

private:
    T* m_ptr;                          // 指向实际对象的原始指针
    std::atomic<int>* m_refCount;      // 线程安全的引用计数（原子类型， nullptr表示无计数）
};

/**
 * @brief 数组类型智能指针特化
 * @details 专门用于管理动态数组（T[]），析构时使用delete[]释放
 * @tparam T 数组元素的模板类型
 */
template <typename T>
class GenericSmartPointer<T[]>
{
public:
    /**
     * @brief 默认构造函数
     * @note 初始化为空数组指针，引用计数为nullptr
     */
    GenericSmartPointer() noexcept : m_ptr(nullptr), m_refCount(nullptr)
    {
    }

    /**
     * @brief nullptr_t构造函数
     * @note 委托给默认构造函数，统一空指针初始化
     */
    GenericSmartPointer(std::nullptr_t) noexcept : GenericSmartPointer()
    {
    }

    /**
     * @brief 接收数组原始指针的构造函数
     * @param ptr 数组原始指针（必须是new[]分配的内存）
     * @note explicit禁止隐式转换，避免裸指针误用
     */
    explicit GenericSmartPointer(T* ptr) noexcept : m_ptr(ptr)
    {
        if (m_ptr != nullptr)
        {
            m_refCount = new std::atomic<int>(1);
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param other 其他数组智能指针对象
     * @note 共享数组所有权，引用计数+1
     */
    GenericSmartPointer(const GenericSmartPointer& other) noexcept : m_ptr(other.m_ptr), m_refCount(other.m_refCount)
    {
        if (m_refCount != nullptr)
        {
            m_refCount->fetch_add(1, std::memory_order_relaxed);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 其他数组智能指针对象（右值）
     * @note 转移数组所有权，原对象置空
     */
    GenericSmartPointer(GenericSmartPointer&& other) noexcept : m_ptr(other.m_ptr), m_refCount(other.m_refCount)
    {
        other.m_ptr = nullptr;
        other.m_refCount = nullptr;
    }

    /**
     * @brief 析构函数
     * @note 释放数组资源（使用delete[]）
     */
    ~GenericSmartPointer() noexcept
    {
        reset();
    }

    /**
     * @brief 拷贝赋值运算符
     * @param other 其他数组智能指针对象
     * @return 自身引用
     */
    GenericSmartPointer& operator=(const GenericSmartPointer& other) noexcept
    {
        if (this != &other)
        {
            reset();
            m_ptr = other.m_ptr;
            m_refCount = other.m_refCount;
            if (m_refCount != nullptr)
            {
                m_refCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param other 其他数组智能指针对象（右值）
     * @return 自身引用
     */
    GenericSmartPointer& operator=(GenericSmartPointer&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            m_ptr = other.m_ptr;
            m_refCount = other.m_refCount;
            other.m_ptr = nullptr;
            other.m_refCount = nullptr;
        }
        return *this;
    }

    /**
     * @brief nullptr赋值运算符
     * @return 自身引用
     */
    GenericSmartPointer& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    /**
     * @brief 禁止裸指针直接赋值
     */
    GenericSmartPointer& operator=(T*) noexcept = delete;

    /**
     * @brief 获取数组原始指针
     * @return 数组的原始指针（非所有权转移）
     */
    T* get() const noexcept { return m_ptr; }

    /**
     * @brief 重置数组智能指针
     * @param ptr 新数组指针（必须是new[]分配，默认为nullptr）
     * @note 释放当前数组（delete[]），接管新数组所有权
     */
    void reset(T* ptr = nullptr) noexcept
    {
        T* oldPtr = m_ptr;
        std::atomic<int>* oldRef = m_refCount;
        m_ptr = nullptr;
        m_refCount = nullptr;

        if (oldRef != nullptr)
        {
            if (oldRef->fetch_sub(1, std::memory_order_acquire) == 1)
            {
                delete[] oldPtr;      // 数组专用释放：delete[]
                delete oldRef;
            }
        }

        m_ptr = ptr;
        if (m_ptr != nullptr)
        {
            m_refCount = new std::atomic<int>(1);
        }
    }

    /**
     * @brief STL风格reset函数
     * @param ptr 新数组指针（默认为nullptr）
     */
    void reset(T* ptr = nullptr) noexcept
    {
        reset(ptr);
    }

    /**
     * @brief 交换两个数组智能指针
     * @param other 要交换的对象
     */
    void swap(GenericSmartPointer& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_refCount, other.m_refCount);
    }

    /**
     * @brief 获取引用计数
     * @return 引用计数值
     */
    int getReferenceCount() const noexcept
    {
        return (m_refCount != nullptr) ? m_refCount->load(std::memory_order_relaxed) : 0;
    }

    /**
     * @brief 数组下标访问运算符
     * @param idx 数组索引（需确保不越界）
     * @return 对应索引的元素引用
     * @note 空指针访问会触发断言
     */
    T& operator[](size_t idx) const noexcept
    {
        assert(m_ptr != nullptr && "Accessing null array GenericSmartPointer!");
        return m_ptr[idx];
    }

    /**
     * @brief 类型转换运算符（显式）
     * @return 是否非空数组
     */
    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    /**
     * @brief 比较运算符（与另一个数组智能指针）
     */
    bool operator==(const GenericSmartPointer& other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    /**
     * @brief 不等于比较运算符（与另一个数组智能指针）
     */
    bool operator!=(const GenericSmartPointer& other) const noexcept
    {
        return !(*this == other);
    }

    /**
     * @brief 与nullptr比较运算符
     */
    bool operator==(std::nullptr_t) const noexcept
    {
        return m_ptr == nullptr;
    }

    /**
     * @brief 与nullptr不等于比较运算符
     */
    bool operator!=(std::nullptr_t) const noexcept
    {
        return m_ptr != nullptr;
    }

private:
    T* m_ptr;                          // 指向数组的原始指针
    std::atomic<int>* m_refCount;      // 线程安全的引用计数
};

/**
 * @brief 创建普通类型智能指针的辅助函数
 * @tparam T 指针指向的类型（支持const）
 * @tparam Args 构造函数参数类型
 * @param args 构造函数的完美转发参数
 * @return 初始化后的GenericSmartPointer对象
 * @note 替代手动new，避免内存泄漏，支持任意构造函数参数
 */
template <typename T, typename... Args>
GenericSmartPointer<T> MakeGenericSmartPointer(Args&&... args)
{
    return GenericSmartPointer<T>(new T(std::forward<Args>(args)...));
}

/**
 * @brief 创建数组类型智能指针的辅助函数
 * @tparam T 数组元素类型（支持const）
 * @param size 数组大小
 * @return 初始化后的数组型GenericSmartPointer对象
 * @note 数组元素会默认初始化（内置类型为0，类类型调用默认构造）
 */
template <typename T>
GenericSmartPointer<T[]> MakeGenericSmartPointerArray(size_t size)
{
    return GenericSmartPointer<T[]>(new T[size]());
}

/**
 * @brief 全局swap函数（普通类型）
 * @tparam T 指针指向的类型
 * @param lhs 左侧智能指针
 * @param rhs 右侧智能指针
 * @note 兼容std::swap，调用成员swap()实现
 */
template <typename T>
void swap(GenericSmartPointer<T>& lhs, GenericSmartPointer<T>& rhs) noexcept
{
    lhs.swap(rhs);
}

/**
 * @brief 全局swap函数（数组类型）
 * @tparam T 数组元素类型
 * @param lhs 左侧数组智能指针
 * @param rhs 右侧数组智能指针
 */
template <typename T>
void swap(GenericSmartPointer<T[]>& lhs, GenericSmartPointer<T[]>& rhs) noexcept
{
    lhs.swap(rhs);
}

/**
 * @brief std::hash特化（普通类型）
 * @details 支持将GenericSmartPointer作为std::unordered_map/unordered_set的键
 * @tparam T 指针指向的类型
 */
namespace std
{
template <typename T>
struct hash<GenericSmartPointer<T>>
{
    size_t operator()(const GenericSmartPointer<T>& sp) const noexcept
    {
        // 复用原始指针的hash逻辑，确保键唯一性
        return hash<T*>()(sp.get());
    }
};

/**
 * @brief std::hash特化（数组类型）
 * @tparam T 数组元素类型
 */
template <typename T>
struct hash<GenericSmartPointer<T[]>>
{
    size_t operator()(const GenericSmartPointer<T[]>& sp) const noexcept
    {
        return hash<T*>()(sp.get());
    }
};
}  // namespace std

#endif // GENERIC_SMART_POINTER_H