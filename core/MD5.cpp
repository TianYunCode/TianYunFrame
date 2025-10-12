#include "MD5.h"

// MD5算法中的循环左移步长常量定义
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define UINT4 unsigned int

// MD5的四个非线性函数 F, G, H, I
// F(X,Y,Z) = (X & Y) | (~X & Z)
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
// G(X,Y,Z) = (X & Z) | (Y & ~Z)  
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
// H(X,Y,Z) = X ^ Y ^ Z
#define H(x, y, z) ((x) ^ (y) ^ (z))
// I(X,Y,Z) = Y ^ (X | ~Z)
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// 循环左移宏
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// MD5的四轮变换宏
// 第一轮变换 FF
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
// 第二轮变换 GG  
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
// 第三轮变换 HH
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
// 第四轮变换 II
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

// 静态常量初始化
const byte MD5::PADDING[64] = { 0x80 };  // 填充字节，第一位为1，其余为0
const char MD5::HEX[16] =
{
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

// 默认构造函数
MD5::MD5()
{
    reset();  // 初始化状态
}

// 从内存数据构造
MD5::MD5(const void* input, size_t length)
{
    reset();
    update(input, length);  // 更新数据
}

// 从字符串构造  
MD5::MD5(const std::string& str)
{
    reset();
    update(str);  // 更新字符串数据
}

// 从文件流构造
MD5::MD5(std::ifstream& in)
{
    reset();
    update(in);  // 更新文件数据
}

// 获取MD5摘要
const byte* MD5::digest()
{
    if (!m_finished)  // 如果计算未完成
    {
        m_finished = true;
        final();  // 完成计算
    }

    return m_digest;  // 返回128位摘要
}

// 重置MD5计算状态
void MD5::reset()
{
    m_finished = false;  // 标记未完成

    // 重置消息长度计数器
    m_count[0] = m_count[1] = 0;

    // 初始化MD5状态寄存器 (小端序)
    m_state[0] = 0x67452301;  // A
    m_state[1] = 0xefcdab89;  // B  
    m_state[2] = 0x98badcfe;  // C
    m_state[3] = 0x10325476;  // D
}

// 更新内存数据
void MD5::update(const void* input, size_t length)
{
    update((const byte*)input, length);  // 转换为字节指针调用内部方法
}

// 更新字符串数据
void MD5::update(const std::string& str)
{
    update((const byte*)str.c_str(), str.length());  // 转换为C字符串调用
}

// 更新文件数据
void MD5::update(std::ifstream& in)
{
    if (!in)  // 检查文件是否打开成功
    {
        return;
    }

    std::streamsize length;
    char buffer[BUFFER_SIZE];  // 文件读取缓冲区

    // 循环读取文件直到结束
    while (!in.eof())
    {
        in.read(buffer, BUFFER_SIZE);  // 读取数据
        length = in.gcount();          // 获取实际读取的字节数
        if (length > 0)
        {
            update(buffer, length);    // 更新MD5计算
        }
    }

    in.close();  // 关闭文件
}

// 内部更新方法 - 核心的数据处理函数
void MD5::update(const byte* input, size_t length)
{
    ulong i, index, partLen;

    m_finished = false;  // 标记计算未完成

    // 计算当前缓冲区位置 (字节数转位数再取模64)
    index = (ulong)((m_count[0] >> 3) & 0x3f);

    // 更新消息长度计数器 (以位为单位)
    // 处理64位计数器的进位
    if ((m_count[0] += ((ulong)length << 3)) < ((ulong)length << 3))
    {
        m_count[1]++;  // 低位溢出，高位加1
    }
    m_count[1] += ((ulong)length >> 29);  // 更新高位计数器

    // 计算缓冲区剩余空间
    partLen = 64 - index;

    // 如果输入数据长度大于等于缓冲区剩余空间
    if (length >= partLen)
    {
        // 填充缓冲区并处理
        memcpy(&m_buffer[index], input, partLen);
        transform(m_buffer);  // 处理完整的64字节块

        // 处理剩余的所有完整64字节块
        for (i = partLen; i + 63 < length; i += 64)
        {
            transform(&input[i]);  // 处理每个完整块
        }

        index = 0;  // 重置缓冲区索引
    }
    else
    {
        i = 0;  // 数据不足一个块，直接复制到缓冲区
    }

    // 将剩余数据复制到缓冲区
    memcpy(&m_buffer[index], &input[i], length - i);
}

// 完成MD5计算
void MD5::final()
{
    byte bits[8];           // 存储消息长度的字节表示
    ulong oldState[4];      // 保存当前状态
    ulong oldCount[2];      // 保存当前计数
    ulong index, padLen;

    // 保存当前状态和计数
    memcpy(oldState, m_state, 16);
    memcpy(oldCount, m_count, 8);

    // 将消息长度编码为字节数组
    encode(m_count, bits, 8);

    // 计算当前缓冲区位置
    index = (ulong)((m_count[0] >> 3) & 0x3f);
    // 计算需要填充的字节数
    padLen = (index < 56) ? (56 - index) : (120 - index);

    // 添加填充字节
    update(PADDING, padLen);
    // 添加消息长度
    update(bits, 8);

    // 将最终状态编码为摘要
    encode(m_state, m_digest, 16);

    // 恢复状态和计数 (为了支持继续计算)
    memcpy(m_state, oldState, 16);
    memcpy(m_count, oldCount, 8);
}

// MD5核心变换函数 - 处理64字节数据块
void MD5::transform(const byte block[64])
{
    // 初始化状态寄存器
    ulong a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];
    ulong x[16];  // 16个32位字的工作数组

    // 将64字节块解码为16个32位字
    decode(block, x, 64);

    /* 第一轮变换 (16步操作) */
    // 每步操作: FF(a, b, c, d, x[k], s, ac)
    // 其中: a,b,c,d是状态寄存器，x[k]是消息字，s是循环左移位数，ac是常量
    FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
    FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* 第二轮变换 (16步操作) */
    GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
    GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* 第三轮变换 (16步操作) */
    HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
    HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

    /* 第四轮变换 (16步操作) */
    II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
    II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

    // 更新状态寄存器
    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;
}

// 将ulong数组编码为字节数组 (小端序)
void MD5::encode(const ulong* input, byte* output, size_t length)
{
    for (size_t i = 0, j = 0; j < length; i++, j += 4)
    {
        output[j] = (byte)(input[i] & 0xff);           // 最低字节
        output[j + 1] = (byte)((input[i] >> 8) & 0xff);  // 次低字节
        output[j + 2] = (byte)((input[i] >> 16) & 0xff); // 次高字节  
        output[j + 3] = (byte)((input[i] >> 24) & 0xff); // 最高字节
    }
}

// 将字节数组解码为ulong数组 (小端序)
void MD5::decode(const byte* input, ulong* output, size_t length)
{
    for (size_t i = 0, j = 0; j < length; i++, j += 4)
    {
        output[i] = ((ulong)input[j]) | (((ulong)input[j + 1]) << 8) | (((ulong)input[j + 2]) << 16) | (((ulong)input[j + 3]) << 24);
    }
}

// 将字节数组转换为十六进制字符串
std::string MD5::bytesToHexString(const byte* input, size_t length)
{
    std::string str;
    str.reserve(length << 1);  // 预分配空间，每个字节对应2个十六进制字符

    for (size_t i = 0; i < length; i++)
    {
        int t = input[i];           // 当前字节值
        int a = t / 16;             // 高4位
        int b = t % 16;             // 低4位
        str.append(1, HEX[a]);      // 添加高4位对应的十六进制字符
        str.append(1, HEX[b]);      // 添加低4位对应的十六进制字符
    }

    return str;
}

// 获取MD5的十六进制字符串表示
std::string MD5::toString()
{
    return bytesToHexString(digest(), 16);  // 将128位摘要转换为32字符十六进制字符串
}