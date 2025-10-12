#pragma once

#include <string>
#include <fstream>

// 类型定义
typedef unsigned char byte;    // 字节类型
typedef unsigned long ulong;   // 无符号长整型

class MD5
{
public:
    // 构造函数
    MD5();                                                              // 默认构造函数
    MD5(const void* input, size_t length);                              // 从内存数据构造
    MD5(const std::string& str);                                        // 从字符串构造
    MD5(std::ifstream& in);                                             // 从文件流构造

    // 更新MD5计算的方法
    void update(const void* input, size_t length);                      // 更新内存数据
    void update(const std::string& str);                                // 更新字符串数据
    void update(std::ifstream& in);                                     // 更新文件数据

    // 获取和输出结果
    const byte* digest();                                               // 获取原始字节摘要
    std::string toString();                                             // 获取十六进制字符串表示
    void reset();                                                       // 重置MD5计算状态

private:
    // 内部实现方法
    void update(const byte* input, size_t length);                      // 内部更新方法
    void final();                                                       // 完成MD5计算
    void transform(const byte block[64]);                               // 处理64字节数据块
    void encode(const ulong* input, byte* output, size_t length);       // 编码ulong数组到字节数组
    void decode(const byte* input, ulong* output, size_t length);       // 解码字节数组到ulong数组
    std::string bytesToHexString(const byte* input, size_t length);     // 字节数组转十六进制字符串

    // 禁止拷贝构造和赋值操作
    MD5(const MD5&);
    MD5& operator=(const MD5&);

private:
    // MD5状态变量
    ulong m_state[4];                            // MD5的四个状态寄存器 (A, B, C, D)
    ulong m_count[2];                            // 消息长度的位数计数 (低位, 高位)
    byte m_buffer[64];                           // 输入缓冲区
    byte m_digest[16];                           // 最终的计算结果 (128位)
    bool m_finished;                             // 标记计算是否完成

    // 静态常量
    static const byte PADDING[64];              // 填充字节
    static const char HEX[16];                  // 十六进制字符表
    static const size_t BUFFER_SIZE = 1024;     // 文件读取缓冲区大小
};