#pragma once

#include <filesystem>
#include <vector>

/**
 * @brief 文件系统操作类
 *
 * 封装了常用的文件系统操作，提供跨平台的文件和目录管理功能
 */
class FileSystem
{
public:
    /**
     * @brief 路径类型枚举
     *
     * 定义不同类型的文件系统路径
     */
    enum class PathType : uint8_t
    {
        None,           // 无效路径
        NotFound,       // 路径不存在
        Regular,        // 普通文件
        Directory,      // 目录
        Symlink,        // 符号链接
        Block,          // 块设备
        Character,      // 字符设备
        Fifo,           // 管道
        Socket,         // 套接字
        Unknown,        // 未知类型
        Junction        // 连接点（Windows特有）
    };

    /**
     * @brief 构造函数
     * @param url 文件或目录路径
     */
    FileSystem(const std::string &url);

    /**
     * @brief 析构函数
     */
    ~FileSystem();

    /**
     * @brief 获取当前路径
     * @return 当前路径字符串
     */
    const std::string &getUrl();

    /**
     * @brief 重置路径
     * @param url 新的文件或目录路径
     */
    void reset(const std::string &url);

    /**
     * @brief 检查路径是否存在
     * @return 存在返回true，否则返回false
     */
    bool exits();

    /**
     * @brief 检查是否为目录
     * @return 是目录返回true，否则返回false
     */
    bool isDir();

    /**
     * @brief 检查是否为普通文件
     * @return 是文件返回true，否则返回false
     */
    bool isFile();

    /**
     * @brief 创建目录（递归创建）
     * @param url 目录路径
     * @return 创建成功返回true，否则返回false
     */
    static bool createDir(const std::string &url);

    /**
     * @brief 创建空文件
     * @param url 文件路径
     * @return 创建成功返回true，否则返回false
     */
    static bool createNullFile(const std::string &url);

    /**
     * @brief 删除文件或空目录
     * @param url 路径
     * @return 删除成功返回true，否则返回false
     */
    static bool remove(const std::string &url);

    /**
     * @brief 递归删除目录及其所有内容
     * @param url 目录路径
     * @return 删除成功返回true，否则返回false
     */
    static bool removeAll(const std::string &url);

    /**
     * @brief 获取文件大小
     * @param url 文件路径
     * @return 文件大小（字节），文件不存在返回-1
     */
    static int64_t getFileSize(const std::string &url);

    /**
     * @brief 复制文件或目录
     * @param srcUrl 源路径
     * @param dstUrl 目标路径
     */
    static void copy(const std::string &srcUrl, const std::string &dstUrl);

    /**
     * @brief 重命名或移动文件/目录
     * @param srcUrl 源路径
     * @param dstUrl 目标路径
     */
    static void rename(const std::string &srcUrl, const std::string &dstUrl);

    /**
     * @brief 获取路径类型
     * @param url 路径
     * @return 路径类型枚举值
     */
    static PathType getPathType(const std::string &url);

    /**
     * @brief 递归获取目录下所有文件
     * @param url 目录路径
     * @return 文件路径列表
     */
    static std::vector<std::string> getFiles(const std::string &url);

    /**
     * @brief 列出目录下符合正则表达式的文件
     * @param path 目录路径
     * @param regular 正则表达式
     * @return 符合条件的文件路径列表
	 */
    static std::vector<std::string> listFiles(const std::string& path, const std::string& regular);

    /**
     * @brief 路径类型转字符串
     * @param type 路径类型
     * @return 类型描述字符串
     */
    static const std::string toString(PathType type);

private:
    std::string m_url;                          // 路径字符串
    std::filesystem::path m_path;               // 文件系统路径对象
    std::filesystem::directory_entry m_entry;   // 目录条目对象
};
