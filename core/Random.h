#pragma once

#include <ctime>
#include <random>

/**
 * @brief 随机数生成类
 *
 * 提供生成指定范围内的浮点数和整数随机数的功能
 */
class Random
{
public:
    Random() = default;
    ~Random() = default;

    // 生成指定范围的浮点数随机数
    void generateFloat(float min,float max)
    {
        m_engine.seed(time(0));
        m_uniformFloat = std::uniform_real_distribution<float>(min, max);
    }

    // 生成指定范围的整数随机数
    void generateInt(int min,int max)
    {
        m_engine.seed(time(0));
        m_uniformInt = std::uniform_int_distribution<int>(min, max);
    }

    // 获取一个浮点数随机数
    float getFloat()
    {
        return m_uniformFloat(m_engine);
    }

    // 获取一个整数随机数
    int getInt()
    {
        return m_uniformInt(m_engine);
    }

private:
    std::default_random_engine m_engine;                    // 随机数引擎
    std::uniform_real_distribution<float> m_uniformFloat;   // 浮点数分布
    std::uniform_int_distribution<int> m_uniformInt;        // 整数分布
};
