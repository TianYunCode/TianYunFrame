#include "DemoHead.h"

#if T_ConfigDemo

#include <iostream>
#include <string>
#include "ConfigManager.hpp"
#include <QDebug>

// 自定义结构体示例
struct CustomData
{
    int id;
    std::string name;
    double score;

    // 重载输出操作符，用于转换为字符串
    friend std::ostream& operator<<(std::ostream& os, const CustomData& data)
    {
        os << data.id << "," << data.name << "," << data.score;
        return os;
    }

    // 重载输入操作符，用于从字符串解析
    friend std::istream& operator>>(std::istream& is, CustomData& data)
    {
        char comma;
        is >> data.id >> comma;
        std::getline(is, data.name, ',');
        is >> data.score;
        return is;
    }
};

// 自定义类型的 toString 特化
template<>
inline std::string ConfigManager::toString<CustomData>(const CustomData& value) const
{
    std::ostringstream oss;
    oss << value.id << "," << value.name << "," << value.score;
    return oss.str();
}

// 自定义类型的 fromString 特化
template<>
inline CustomData ConfigManager::fromString<CustomData>(const std::string& str) const
{
    CustomData data;
    std::istringstream iss(str);
    char comma;
    iss >> data.id >> comma;
    std::getline(iss, data.name, ',');
    iss >> data.score;
    return data;
}

int main()
{
    // 获取配置管理器实例
    auto config = ConfigManager::getInstance();

    // 1. 加载配置文件
    qDebug() << "1. 加载配置文件...";
    if (config->loadConfig("demo_config.ini"))
    {
        qDebug() << "  配置文件加载成功";
    }
    else
    {
        qDebug() << "  配置文件不存在，将创建新配置";
    }

    // 2. 设置和获取各种类型的配置值
    qDebug() << "\n2. 设置和获取配置值...";

    // 整数
    config->setValue("Network", "Port", 8080, "服务器端口号");
    int port = config->getValue("Network", "Port", 80);
    qDebug() << "  端口号: " << port;

    // 浮点数
    config->setValue("Graphics", "AspectRatio", 1.777, "画面宽高比");
    double ratio = config->getValue("Graphics", "AspectRatio", 1.333);
    qDebug() << "  宽高比: " << ratio;

    // 布尔值
    config->setValue("System", "EnableLogging", true, "是否启用日志");
    bool logging = config->getValue("System", "EnableLogging", false);
    qDebug() << "  启用日志: " << std::boolalpha << logging;

    // 字符串
    config->setValue("Database", "Host", "localhost", "数据库主机地址");
    std::string host = config->getValue<std::string>("Database", "Host", "127.0.0.1");
    qDebug() << "  数据库主机: " << host;

    // 3. 使用自定义类型
    qDebug() << "\n3. 使用自定义类型...";
    CustomData userData = {1001, "张三", 95.5};
    config->setValue("User", "Data", userData, "用户数据");

    CustomData retrievedData = config->getValue("User", "Data", CustomData{});
    qDebug() << "  用户数据: ID=" << retrievedData.id << ", 姓名=" << retrievedData.name << ", 分数=" << retrievedData.score;

    // 4. 检查配置项是否存在
    qDebug() << "\n4. 检查配置项是否存在...";
    bool nodeExists = config->isExist("Network");
    bool keyExists = config->isExist("Network", "Port");
    qDebug() << "  Network节点存在: " << nodeExists;
    qDebug() << "  Network.Port键存在: " << keyExists;

    // 5. 获取和设置注释
    qDebug() << "\n5. 管理配置注释...";
    std::string comment = config->getComment("Network", "Port");
    qDebug() << "  Network.Port的注释: " << comment;

    config->setComment("Database", "Host", "修改后的数据库主机地址注释");

    // 6. 获取所有节点和键值对
    qDebug() << "\n6. 遍历配置结构...";
    auto nodes = config->getAllNodes();
    qDebug() << "  所有配置节点:";
    for (const auto& node : nodes)
    {
        qDebug() << "    [" << node << "]";
        auto keyValues = config->getAllKeyValueFromNode(node);
        for (const auto& kv : keyValues)
        {
            qDebug() << "      " << kv.first << " = " << kv.second;
        }
    }

    // 7. 删除配置项
    qDebug() << "\n7. 删除配置项...";
    config->removeConfig("User", "Data");
    qDebug() << "  已删除User.Data配置项";

    // 检查是否已删除
    bool userDataExists = config->isExist("User", "Data");
    qDebug() << "  User.Data是否存在: " << userDataExists;

    // 8. 实时保存配置
    qDebug() << "\n8. 实时保存配置...";
    config->setValue("System", "Version", "1.0.0", "系统版本号", true); // 实时写入文件
    qDebug() << "  配置已实时保存到文件";

    // 9. 最终保存所有配置
    qDebug() << "\n9. 保存所有配置到文件...";
    if (config->saveConfig())
    {
        qDebug() << "  配置保存成功";
    }
    else
    {
        qDebug() << "  配置保存失败";
    }

    // 10. 演示默认值功能
    qDebug() << "\n10. 使用默认值...";
    int nonExistentValue = config->getValue("NonExistent", "Value", 999);
    qDebug() << "  不存在的配置项返回值: " << nonExistentValue;

    qDebug() << "\n演示完成!";

    return 0;
}

#endif
