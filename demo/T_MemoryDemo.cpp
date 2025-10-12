#include "DemoHead.h"

#if T_MemoryDemo

#include <iostream>
#include <string>
#include "MemoryPool.hpp"
#include "ObjectPool.hpp"
#include "PooledSharedPtr.hpp"

// 内存池测试函数
void MemoryPoolTest()
{
    // 创建字符串对象池，线程缓存大小为16，每个内存块包含128个对象
    ObjectPool<std::string> pool(16, 128);
    pool.GetStats().Print(); // 打印内存使用情况

    // 第一阶段：创建和使用对象
    std::string* strs[10]; // 存储字符串指针的数组
    for (int i = 0; i < 10; ++i)
    {
        // 使用对象池构造字符串对象
        strs[i] = pool.Construct("String_" + std::to_string(i));
        std::cout << *strs[i] << " "; // 打印字符串内容
    }
    std::cout << "\n\n";

    // 显示内存统计信息
    auto stats = pool.GetStats();
    stats.Print(); // 打印内存使用情况

    // 第二阶段：部分释放对象
    for (int i = 0; i < 5; ++i)
    {
        pool.Destroy(strs[i]); // 销毁对象并返回内存到池中
        strs[i] = nullptr;     // 将指针设置为nullptr避免悬空指针
    }

    // 创建更多对象
    for (int i = 10; i < 20; ++i)
    {
        // 重用之前释放的内存创建新对象
        strs[i - 10] = pool.Construct("NewString_" + std::to_string(i));
    }

    // 释放所有对象
    for (int i = 0; i < 10; ++i)
    {
        if (strs[i])
        {
            pool.Destroy(strs[i]); // 销毁所有对象
        }
    }

    // 内存回收测试
    std::cout << "\nBefore shrink:\n";
    pool.GetStats().Print(); // 打印收缩前的内存状态

    pool.ShrinkToFit(50); // 尝试收缩内存池，保留50个空闲块

    std::cout << "\nAfter shrink:\n";
    pool.GetStats().Print(); // 打印收缩后的内存状态

    // 测试智能指针集成
    // 使用池化共享指针创建字符串对象
    auto smartStr = PooledSharedPtr<std::string>::Create("Smart String");
    std::cout << "\nSmart pointer value: " << *smartStr << "\n";
    // 当smartStr离开作用域时，会自动调用自定义删除器将内存返回对象池
}

int main(int argc, char** argv)
{
    MemoryPoolTest(); // 运行测试
    return 0;
}

#endif
