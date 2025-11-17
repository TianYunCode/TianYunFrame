#pragma once

#ifndef RINGBUFFERMANAGER_H
#define RINGBUFFERMANAGER_H

#include <cstdint>
#include <mutex>

/**
 * @brief 线程安全的环形缓冲区管理器模板类
 *
 * @tparam T 缓冲区存储的数据类型
 */
template<typename T>
class RingBufferManager
{
public:
    /**
     * @brief 构造函数
     * @param bufferSize 缓冲区大小（元素个数）
     */
    RingBufferManager(uint32_t bufferSize = 1);

    /**
     * @brief 析构函数
     */
    ~RingBufferManager();

    /**
     * @brief 获取当前缓冲区中的数据个数
     * @return 数据个数
     */
    uint32_t getDataCount();

    /**
     * @brief 获取缓冲区剩余空间大小
     * @return 剩余空间大小
     */
    uint32_t getReserveCount();

    /**
     * @brief 调整缓冲区大小
     * @param bufferSize 新的缓冲区大小
     * @note 调整大小时会清空现有数据
     */
    void resizeBuffer(uint32_t bufferSize);

    /**
     * @brief 清空缓冲区
     */
    void clear();

    /**
     * @brief 清空指定数量的数据
     * @param count 要清除的数据个数
     */
    void clear(uint32_t count);

    /**
     * @brief 向缓冲区写入数据
     * @param buffer 要写入的数据数组
     * @param count 要写入的数据个数
     * @note 如果缓冲区空间不足，多余的数据将被丢弃
     */
    void writeBuffer(T buffer[], uint32_t count);

    /**
     * @brief 从缓冲区读取数据
     * @param targetBytes 存储读取数据的数组
     * @param count 要读取的数据个数
     * @return 实际读取的数据个数
     */
    uint32_t readBuffer(T targetBytes[], uint32_t count);

private:
    T* Buffer;           ///< 存放数据的数组
    uint32_t BufferLen;  ///< 缓冲区总长度
    uint32_t DataCount;  ///< 当前数据个数
    uint32_t DataStart;  ///< 数据起始索引
    uint32_t DataEnd;    ///< 数据结束索引
    std::mutex lock_rw;  ///< 读写互斥锁
};

/**
 * @brief 构造函数
 * @param bufferSize 缓冲区大小（元素个数）
 */
template<typename T>
RingBufferManager<T>::RingBufferManager(uint32_t bufferSize)
{
    DataCount = 0;
    DataStart = 0;
    DataEnd = 0;
    BufferLen = bufferSize;
    Buffer = new T[bufferSize];
}

// 析构函数实现
template<typename T>
RingBufferManager<T>::~RingBufferManager()
{
    delete[] Buffer;
    Buffer = NULL;
}

/**
 * @brief 调整缓冲区大小
 * @param bufferSize 新的缓冲区大小
 * @note 调整大小时会清空现有数据
 */
template<typename T>
void RingBufferManager<T>::resizeBuffer(uint32_t bufferSize)
{
    if (Buffer)
        delete[] Buffer;
    DataCount = 0;
    DataStart = 0;
    DataEnd = 0;
    BufferLen = bufferSize;
    Buffer = new T[bufferSize];
}

// 获取数据个数实现
template<typename T>
uint32_t RingBufferManager<T>::getDataCount()
{
    return DataCount;
}

// 获取剩余空间实现
template<typename T>
uint32_t RingBufferManager<T>::getReserveCount()
{
    return BufferLen - DataCount;
}

// 清空缓冲区实现
template<typename T>
void RingBufferManager<T>::clear()
{
    lock_rw.lock();
    DataCount = 0;
    DataStart = 0;
    DataEnd = 0;
    lock_rw.unlock();
}

/**
 * @brief 清空指定数量的数据
 * @param count 要清除的数据个数
 */
template<typename T>
void RingBufferManager<T>::clear(uint32_t count)
{
    lock_rw.lock();
    if (count >= DataCount)
    {
        DataCount = 0;
        DataStart = 0;
        DataEnd = 0;
    }
    else
    {
        if (DataStart + count >= BufferLen)
        {
            DataStart = (DataStart + count) - BufferLen;
        }
        else
        {
            DataStart += count;
        }
        DataCount -= count;
    }
    lock_rw.unlock();
}

/**
 * @brief 向缓冲区写入数据
 * @param buffer 要写入的数据数组
 * @param count 要写入的数据个数
 * @note 如果缓冲区空间不足，多余的数据将被丢弃
 */
template<typename T>
void RingBufferManager<T>::writeBuffer(T buffer[], uint32_t count)
{
    lock_rw.lock();
    uint32_t reserveCount = BufferLen - DataCount;
    if (reserveCount >= count)
    {
        if (DataEnd + count < BufferLen)
        {
            memcpy(Buffer + DataEnd, buffer, count * sizeof(T));
            DataEnd += count;
            DataCount += count;
        }
        else
        {
            int32_t overflowIndexLength = (DataEnd + count) - BufferLen;
            int32_t endPushIndexLength = count - overflowIndexLength;
            memcpy(Buffer + DataEnd, buffer, endPushIndexLength * sizeof(T));
            DataEnd = 0;
            DataCount += endPushIndexLength;
            if (overflowIndexLength != 0)
            {
                memcpy(Buffer + DataEnd, buffer + endPushIndexLength, overflowIndexLength * sizeof(T));
            }
            DataEnd += overflowIndexLength;
            DataCount += overflowIndexLength;
        }
    }
    lock_rw.unlock();
}

/**
 * @brief 从缓冲区读取数据
 * @param targetBytes 存储读取数据的数组
 * @param count 要读取的数据个数
 * @return 实际读取的数据个数
 */
template<typename T>
uint32_t RingBufferManager<T>::readBuffer(T targetBytes[], uint32_t count)
{
    if (count > DataCount)
        return 0;

    lock_rw.lock();
    if (DataStart + count < BufferLen)
    {
        memcpy(targetBytes, Buffer + DataStart, count * sizeof(T));
    }
    else
    {
        int32_t overflowIndexLength = (DataStart + count) - BufferLen;
        int32_t endPushIndexLength = count - overflowIndexLength;
        memcpy(targetBytes, Buffer + DataStart, endPushIndexLength * sizeof(T));
        if (overflowIndexLength != 0)
        {
            memcpy(targetBytes + endPushIndexLength, Buffer, overflowIndexLength * sizeof(T));
        }
    }
    lock_rw.unlock();

    return count;
}

#endif // RINGBUFFERMANAGER_H
