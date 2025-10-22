#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <mutex>
#include "Singleton.hpp"

/**
 * @brief 配置管理器类，继承自单例模板，确保全局唯一实例
 *
 * 该类负责加载、解析、保存配置文件（INI格式），并提供配置项的读写接口。
 * 支持多线程安全访问，通过互斥锁保护共享数据。
 */
class ConfigManager : public Singleton<ConfigManager>
{
    friend class Singleton<ConfigManager>; // 允许单例模板访问私有构造函数

    /**
     * @brief 配置值与备注的结构体
     *
     * 用于存储配置项的值和对应的注释说明。
     */
    struct ConfigValueWithComment
    {
        std::string value;    // 配置项的值
        std::string comment;  // 配置项的注释
    };

public:
    ~ConfigManager() {};

    /**
     * @brief 加载配置文件
     *
     * @param filePath 配置文件路径，默认为"config.ini"
     * @return true 加载成功
     * @return false 加载失败（如文件不存在）
     */
    bool loadConfig(const std::string& filePath = "config.ini")
    {
        std::lock_guard<std::mutex> lock(m_mutex); // 加锁保证线程安全

        m_filePath = filePath;
        m_configData.clear(); // 清空现有配置数据

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "无法打开配置文件：" << filePath << std::endl;
            return false;
        }

        std::string line;
        std::string currentNode; // 当前解析的节点（节）

        while (std::getline(file, line))
        {
            // 去除行首尾空白
            line = trim(line);

            // 跳过空行和注释行（以#或;开头）
            if (line.empty() || line[0] == '#' || line[0] == ';')
            {
                continue;
            }

            // 检查是否为节头（如 [Section]）
            if (line.front() == '[' && line.back() == ']')
            {
                currentNode = line.substr(1, line.length() - 2);
                // 确保节存在，即使它是空的
                m_configData[currentNode];
            }
            else
            {
                // 解析键值对（可能包含注释）
                parseLine(line, currentNode);
            }
        }

        file.close();
        return true;
    }

    /**
     * @brief 保存配置文件
     *
     * @param filePath 保存路径，为空则使用加载时的路径
     * @return true 保存成功
     * @return false 保存失败
     */
    bool saveConfig(const std::string& filePath = "")
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string path = filePath.empty() ? m_filePath : filePath;

        if (path.empty())
        {
            std::cerr << "未指定保存文件路径" << std::endl;
            return false;
        }

        return refresh(path); // 刷新到文件
    }

    /**
     * @brief 获取配置值
     *
     * @tparam T 值类型
     * @param node 节点（节）名称
     * @param key 键名
     * @param defaultValue 默认值（当配置项不存在时返回）
     * @return T 配置值或默认值
     */
    template<typename T>
    T getValue(const std::string& node, const std::string& key, const T& defaultValue = T{}) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto sectionIt = m_configData.find(node);
        if (sectionIt != m_configData.end())
        {
            auto keyIt = sectionIt->second.find(key);
            if (keyIt != sectionIt->second.end())
            {
                try
                {
                    return fromString<T>(keyIt->second.value); // 类型转换
                }
                catch (const std::exception& e)
                {
                    std::cerr << "转换失败，使用默认值";
                }
            }
        }

        return defaultValue;
    }

    /**
     * @brief 设置配置值
     *
     * @tparam T 值类型
     * @param node 节点（节）名称
     * @param key 键名
     * @param value 值
     * @param comment 注释（可选）
     * @param isRealTimeWrite 是否实时写入文件（可选）
     */
    template<typename T>
    void setValue(const std::string& node, const std::string& key, const T& value, const std::string& comment = "", bool isRealTimeWrite = false)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        ConfigValueWithComment cv;
        cv.value = toString(value);
        cv.comment = comment;
        m_configData[node][key] = cv;

        if(isRealTimeWrite)
        {
            refresh(m_filePath); // 实时写入文件
        }
    }

    /**
     * @brief 获取配置项的注释
     *
     * @param node 节点名称
     * @param key 键名
     * @return std::string 注释内容（不存在则返回空字符串）
     */
    std::string getComment(const std::string& node, const std::string& key) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto sectionIt = m_configData.find(node);
        if (sectionIt != m_configData.end())
        {
            auto keyIt = sectionIt->second.find(key);
            if (keyIt != sectionIt->second.end())
            {
                return keyIt->second.comment;
            }
        }

        return "";
    }

    /**
     * @brief 设置配置项的注释
     *
     * @param node 节点名称
     * @param key 键名
     * @param comment 注释内容
     */
    void setComment(const std::string& node, const std::string& key, const std::string& comment)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto sectionIt = m_configData.find(node);

        if (sectionIt != m_configData.end())
        {
            auto keyIt = sectionIt->second.find(key);
            if (keyIt != sectionIt->second.end())
            {
                keyIt->second.comment = comment;
            }
            else
            {
                // 如果键不存在，创建一个空值并设置备注
                ConfigValueWithComment cv;
                cv.value = "";
                cv.comment = comment;
                sectionIt->second[key] = cv;
            }
        }
        else
        {
            // 如果节点不存在，创建节点和键并设置备注
            ConfigValueWithComment cv;
            cv.value = "";
            cv.comment = comment;
            m_configData[node][key] = cv;
        }
    }

    /**
     * @brief 检查节点是否存在
     *
     * @param node 节点名称
     * @return true 存在
     * @return false 不存在
     */
    bool isExist(const std::string& node) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_configData.find(node) != m_configData.end();
    }

    /**
     * @brief 检查键是否存在
     *
     * @param node 节点名称
     * @param key 键名
     * @return true 存在
     * @return false 不存在
     */
    bool isExist(const std::string& node, const std::string& key) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto nodeIt = m_configData.find(node);
        if (nodeIt != m_configData.end())
        {
            return nodeIt->second.find(key) != nodeIt->second.end();
        }

        return false;
    }

    /**
     * @brief 获取所有节点名称
     *
     * @return std::vector<std::string> 节点名称列表
     */
    std::vector<std::string> getAllNodes() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::vector<std::string> nodes;
        nodes.reserve(m_configData.size());

        for (const auto& pair : m_configData)
        {
            nodes.push_back(pair.first);
        }

        return nodes;
    }

    /**
     * @brief 获取指定节点下的所有键值对
     *
     * @param node 节点名称
     * @return std::unordered_map<std::string, std::string> 键值对映射（键→值）
     */
    std::unordered_map<std::string, std::string> getAllKeyValueFromNode(const std::string& node) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_configData.find(node);
        if (it != m_configData.end())
        {
            std::unordered_map<std::string, std::string> result;
            for (const auto& keyPair : it->second)
            {
                result[keyPair.first] = keyPair.second.value;
            }

            return result;
        }

        return {};
    }

    /**
     * @brief 删除节点
     *
     * @param node 节点名称
     */
    void removeConfig(const std::string& node)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_configData.erase(node);
    }

    /**
     * @brief 删除键
     *
     * @param node 节点名称
     * @param key 键名
     */
    void removeConfig(const std::string& node, const std::string& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto nodeIt = m_configData.find(node);
        if (nodeIt != m_configData.end())
        {
            nodeIt->second.erase(key);
        }
    }

    /**
     * @brief 类型转换函数（从 string 转换为 T）
     *
     * @tparam T 目标类型
     * @param str 源字符串
     * @return T 转换后的值
     */
    template<typename T>
    T fromString(const std::string& str) const;

    /**
     * @brief 类型转换函数（从 T 转换为 string）
     *
     * @tparam T 源类型
     * @param value 源值
     * @return std::string 转换后的字符串
     */
    template<typename T>
    std::string toString(const T& value) const;

protected:
    ConfigManager() {}; // 保护构造函数，确保只能通过单例模式创建实例

private:
    std::string m_filePath; // 配置文件路径
    // 配置数据存储结构：节点 →（键 →（值，注释））
    std::unordered_map<std::string, std::unordered_map<std::string, ConfigValueWithComment>> m_configData;
    mutable std::mutex m_mutex; // 可变互斥锁，保证const函数的线程安全

    /**
     * @brief 去除字符串首尾空白字符
     *
     * @param str 输入字符串
     * @return std::string 去除空白后的字符串
     */
    std::string trim(const std::string& str)
    {
        const auto strBegin = str.find_first_not_of(" \t");
        if (strBegin == std::string::npos)
        {
            return "";
        }

        const auto strEnd = str.find_last_not_of(" \t");
        const auto strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
    }

    /**
     * @brief 解析键值对行
     *
     * @param line 一行文本
     * @param currentNode 当前节点名称
     */
    void parseLine(const std::string& line, const std::string& currentNode)
    {
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos)
        {
            std::string key = trim(line.substr(0, eqPos));
            std::string valueAndComment = trim(line.substr(eqPos + 1));

            // 分离值和注释
            std::string value = valueAndComment;
            std::string comment;

            // 查找分号注释（INI标准支持分号注释）
            size_t commentPos = valueAndComment.find(';');
            if (commentPos != std::string::npos)
            {
                value = trim(valueAndComment.substr(0, commentPos));
                comment = trim(valueAndComment.substr(commentPos + 1));
            }

            if (!currentNode.empty())
            {
                ConfigValueWithComment cv;
                cv.value = value;
                cv.comment = comment;
                m_configData[currentNode][key] = cv;
            }
        }
    }

    /**
     * @brief 刷新配置文件（将内存中的数据写入文件）
     *
     * @param filePath 文件路径
     * @return true 写入成功
     * @return false 写入失败
     */
    bool refresh(const std::string& filePath)
    {
        std::ofstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "无法打开文件进行写入：" << filePath << std::endl;
            return false;
        }

        for (const auto& nodePair : m_configData)
        {
            file << "[" << nodePair.first << "]\n";

            for (const auto& keyPair : nodePair.second)
            {
                file << keyPair.first << " = " << keyPair.second.value;
                if (!keyPair.second.comment.empty())
                {
                    file << " ; " << keyPair.second.comment;
                }

                file << "\n";
            }

            file << "\n"; // 节点之间空行分隔
        }

        file.close();
        return true;
    }
};

// 以下为类型转换函数的特化实现

/**
 * @brief 特化：string转int
 */
template<>
inline int ConfigManager::fromString<int>(const std::string& str) const
{
    return std::stoi(str);
}

/**
 * @brief 特化：string转long
 */
template<>
inline long ConfigManager::fromString<long>(const std::string& str) const
{
    return std::stol(str);
}

/**
 * @brief 特化：string转long long
 */
template<>
inline long long ConfigManager::fromString<long long>(const std::string& str) const
{
    return std::stoll(str);
}

/**
 * @brief 特化：string转unsigned long
 */
template<>
inline unsigned long ConfigManager::fromString<unsigned long>(const std::string& str) const
{
    return std::stoul(str);
}

/**
 * @brief 特化：string转unsigned long long
 */
template<>
inline unsigned long long ConfigManager::fromString<unsigned long long>(const std::string& str) const
{
    return std::stoull(str);
}

/**
 * @brief 特化：string转float
 */
template<>
inline float ConfigManager::fromString<float>(const std::string& str) const
{
    return std::stof(str);
}

/**
 * @brief 特化：string转double
 */
template<>
inline double ConfigManager::fromString<double>(const std::string& str) const
{
    return std::stod(str);
}

/**
 * @brief 特化：string转bool
 *
 * 支持"true", "1", "yes", "on"（不区分大小写）为true，其他为false
 */
template<>
inline bool ConfigManager::fromString<bool>(const std::string& str) const
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return (lowerStr == "true" || lowerStr == "1" || lowerStr == "yes" || lowerStr == "on");
}

/**
 * @brief 特化：string转string（直接返回）
 */
template <>
inline std::string ConfigManager::fromString<std::string>(const std::string& value) const
{
    return value;
}

/**
 * @brief 特化：int转string
 */
template<>
inline std::string ConfigManager::toString<int>(const int& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：long转string
 */
template<>
inline std::string ConfigManager::toString<long>(const long& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：long long转string
 */
template<>
inline std::string ConfigManager::toString<long long>(const long long& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：unsigned long转string
 */
template<>
inline std::string ConfigManager::toString<unsigned long>(const unsigned long& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：unsigned long long转string
 */
template<>
inline std::string ConfigManager::toString<unsigned long long>(const unsigned long long& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：float转string
 */
template<>
inline std::string ConfigManager::toString<float>(const float& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：double转string
 */
template<>
inline std::string ConfigManager::toString<double>(const double& value) const
{
    return std::to_string(value);
}

/**
 * @brief 特化：bool转string
 *
 * true转换为"true"，false转换为"false"
 */
template<>
inline std::string ConfigManager::toString<bool>(const bool& value) const
{
    return value ? "true" : "false";
}

/**
 * @brief 通用类型转string（使用ostringstream）
 *
 * 用于处理没有特化的类型，要求类型T支持<<操作符
 */
template<typename T>
std::string ConfigManager::toString(const T& value) const
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
