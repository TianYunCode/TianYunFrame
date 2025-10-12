#include "FileSystem.h"
#include <fstream>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;

/**
 * @brief 构造函数
 * @param url 文件或目录路径
 */
FileSystem::FileSystem(const std::string &url)
{
    reset(url);
}

/**
 * @brief 析构函数
 */
FileSystem::~FileSystem()
{
    // 目前没有需要手动释放的资源
}

/**
 * @brief 获取当前路径
 * @return 当前路径字符串
 */
const std::string &FileSystem::getUrl()
{
    return m_url;
}

/**
 * @brief 重置路径
 * @param url 新的文件或目录路径
 */
void FileSystem::reset(const std::string &url)
{
    m_url = url;
    m_path = fs::path(url);
    m_entry = fs::directory_entry(m_path);
}

/**
 * @brief 检查路径是否存在
 * @return 存在返回true，否则返回false
 */
bool FileSystem::exits()
{
    return fs::exists(m_path);
}

/**
 * @brief 检查是否为目录
 * @return 是目录返回true，否则返回false
 */
bool FileSystem::isDir()
{
    return fs::is_directory(m_path);
}

/**
 * @brief 检查是否为普通文件
 * @return 是文件返回true，否则返回false
 */
bool FileSystem::isFile()
{
    return fs::is_regular_file(m_path);
}

/**
 * @brief 创建目录（递归创建）
 * @param url 目录路径
 * @return 创建成功返回true，否则返回false
 */
bool FileSystem::createDir(const std::string &url)
{
    return fs::create_directories(url);
}

/**
 * @brief 创建空文件
 * @param url 文件路径
 * @return 创建成功返回true，否则返回false
 */
bool FileSystem::createNullFile(const std::string &url)
{
    std::ofstream ofs;
    ofs.open(url, std::ios::out);
    if (ofs.is_open())
    {
        ofs.close();
        return true;
    }

    return false;
}

/**
 * @brief 删除文件或空目录
 * @param url 路径
 * @return 删除成功返回true，否则返回false
 */
bool FileSystem::remove(const std::string &url)
{
    fs::path path(url);

    if (!fs::exists(url)) return false;

    if (fs::directory_entry(path).status().type() == fs::file_type::directory)
    {
        for (const auto &entry : fs::directory_iterator(path))
        {
            return false;
        }
    }

    return fs::remove(url);
}

/**
 * @brief 递归删除目录及其所有内容
 * @param url 目录路径
 * @return 删除成功返回true，否则返回false
 */
bool FileSystem::removeAll(const std::string &url)
{
    if (!fs::exists(url)) return false;

    return fs::remove_all(url);
}

/**
 * @brief 获取文件大小
 * @param url 文件路径
 * @return 文件大小（字节），文件不存在返回-1
 */
int64_t FileSystem::getFileSize(const std::string &url)
{
    if (!fs::exists(url)) return -1;

    fs::path path(url);
    return fs::file_size(path);
}

/**
 * @brief 复制文件或目录
 * @param srcUrl 源路径
 * @param dstUrl 目标路径
 */
void FileSystem::copy(const std::string &srcUrl, const std::string &dstUrl)
{
    if (!fs::exists(srcUrl) || fs::exists(dstUrl)) return;

    fs::path srcPath(srcUrl), dstPath(dstUrl);
    fs::copy(srcPath, dstPath);
}

/**
 * @brief 重命名或移动文件/目录
 * @param srcUrl 源路径
 * @param dstUrl 目标路径
 */
void FileSystem::rename(const std::string &srcUrl, const std::string &dstUrl)
{
    if (!fs::exists(srcUrl) || fs::exists(dstUrl)) return;

    fs::path srcPath(srcUrl), dstPath(dstUrl);
    fs::rename(srcPath, dstPath);
}

/**
 * @brief 获取路径类型
 * @param url 路径
 * @return 路径类型枚举值
 */
FileSystem::PathType FileSystem::getPathType(const std::string &url)
{
    fs::path path(url);
    fs::directory_entry entry = fs::directory_entry(path);
    PathType type = static_cast<PathType>(entry.status().type());
    return type;
}

/**
 * @brief 递归获取目录下所有文件
 * @param url 目录路径
 * @return 文件路径列表
 */
std::vector<std::string> FileSystem::getFiles(const std::string &url)
{
    const fs::path path(url);
    std::vector<std::string> files;

    if (!fs::exists(path)) return files;

    for (auto const &directory_entry : fs::recursive_directory_iterator{ path })
    {
        if (directory_entry.is_regular_file())
        {
            files.push_back(directory_entry.path().string());
        }
    }

    return files;
}

/**
 * @brief 列出目录下符合正则表达式的文件
 * @param path 目录路径
 * @param regular 正则表达式
 * @return 符合条件的文件路径列表
 */
std::vector<std::string> FileSystem::listFiles(const std::string& path, const std::string& regular)
{
    std::vector<std::string> files;
	std::regex regexPattern(regular);

    fs::path dirPath(path);
	if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
    {
        return files;
	}

    for (const auto& entry : fs::directory_iterator(dirPath))
    {
        if (entry.is_regular_file())
        {
            const std::string fileName = entry.path().filename().string();
            if (std::regex_match(fileName, regexPattern))
            {
                files.push_back(entry.path().string());
            }
        }
    }

	return files;
}

/**
 * @brief 路径类型转字符串
 * @param type 路径类型
 * @return 类型描述字符串
 */
const std::string FileSystem::toString(PathType type)
{
    switch (type)
    {
    case PathType::None:
        return "None";
    case PathType::Regular:
        return "Regular";
    case PathType::Directory:
        return "Directory";
    case PathType::Symlink:
        return "Symlink";
    case PathType::Block:
        return "Block";
    case PathType::Character:
        return "Character";
    case PathType::Fifo:
        return "Fifo";
    case PathType::Socket:
        return "Socket";
    case PathType::Unknown:
        return "Unknown";
    case PathType::Junction:
        return "Junction";
    case PathType::NotFound:
        return "NotFound";
    }

    return "Unknown";
}
