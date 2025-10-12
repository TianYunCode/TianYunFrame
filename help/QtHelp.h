#pragma once

#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QRegularExpression>
#include <QUrl>
#include <QColor>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QImage>
#include <QPixmap>
#include <QBuffer>
#include <QTextStream>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QProcess>
#include <QSettings>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QRegularExpression>
#include <QElapsedTimer>
#include <QRandomGenerator>

/**
 * @brief QtHelper 类提供了一系列静态工具函数，用于常见的 Qt 开发任务
 *
 * 该类是一个集中的工具库，涵盖文件操作、字符串处理、时间处理、网络操作、
 * 图像处理、加密解密、系统信息等功能。所有函数都是静态的，类不能被实例化
 */
class QtHelper
{
public:
    // 构造函数和析构函数
    QtHelper() = delete; ///< 删除构造函数，防止实例化（纯静态类）
    ~QtHelper() = delete; ///< 删除析构函数，防止实例化

    /************************ 文件操作相关 ************************/
    /**
     * @brief 将数据写入文件
     * @param filePath 目标文件路径
     * @param data 要写入的数据
     * @return 成功返回 true，否则返回 false
     */
    static bool fileWrite(const QString &filePath, const QByteArray &data);

    /**
     * @brief 从文件读取数据
     * @param filePath 源文件路径
     * @return 文件内容作为 QByteArray，出错时返回空
     */
    static QByteArray fileRead(const QString &filePath);

    /**
     * @brief 追加数据到文件
     * @param filePath 目标文件路径
     * @param data 要追加的数据
     * @return 成功返回 true，否则返回 false
     */
    static bool fileAppend(const QString &filePath, const QByteArray &data);

    /**
     * @brief 复制文件
     * @param sourcePath 源文件路径
     * @param targetPath 目标文件路径
     * @return 成功返回 true，否则返回 false
     */
    static bool fileCopy(const QString &sourcePath, const QString &targetPath);

    /**
     * @brief 移动文件
     * @param sourcePath 源文件路径
     * @param targetPath 目标文件路径
     * @return 成功返回 true，否则返回 false
     */
    static bool fileMove(const QString &sourcePath, const QString &targetPath);

    /**
     * @brief 删除文件
     * @param filePath 要删除的文件路径
     * @return 成功返回 true，否则返回 false
     */
    static bool fileRemove(const QString &filePath);

    /**
     * @brief 重命名文件
     * @param oldPath 当前文件路径
     * @param newPath 新文件路径
     * @return 成功返回 true，否则返回 false
     */
    static bool fileRename(const QString &oldPath, const QString &newPath);

    /**
     * @brief 检查文件是否存在
     * @param filePath 要检查的路径
     * @return 文件存在返回 true，否则返回 false
     */
    static bool fileExists(const QString &filePath);

    /**
     * @brief 获取文件大小（字节）
     * @param filePath 文件路径
     * @return 文件大小（字节），出错时返回 -1
     */
    static qint64 fileSize(const QString &filePath);

    /**
     * @brief 获取文件创建时间
     * @param filePath 文件路径
     * @return 文件创建的 QDateTime，出错时返回无效值
     */
    static QDateTime fileCreatedTime(const QString &filePath);

    /**
     * @brief 获取文件修改时间
     * @param filePath 文件路径
     * @return 最后修改的 QDateTime，出错时返回无效值
     */
    static QDateTime fileModifiedTime(const QString &filePath);

    /**
     * @brief 获取文件最后访问时间
     * @param filePath 文件路径
     * @return 最后访问的 QDateTime，出错时返回无效值
     */
    static QDateTime fileLastAccessTime(const QString &filePath);

    /**
     * @brief 设置文件权限
     * @param filePath 文件路径
     * @param permissions 要设置的文件权限
     * @return 成功返回 true，否则返回 false
     */
    static bool fileSetPermissions(const QString &filePath, QFile::Permissions permissions);

    /**
     * @brief 获取文件权限
     * @param filePath 文件路径
     * @return 当前文件权限
     */
    static QFile::Permissions filePermissions(const QString &filePath);

    // 目录操作
    /**
     * @brief 创建目录
     * @param dirPath 要创建的目录路径
     * @return 成功返回 true，否则返回 false
     */
    static bool dirCreate(const QString &dirPath);

    /**
     * @brief 删除目录（必须为空）
     * @param dirPath 要删除的目录路径
     * @return 成功返回 true，否则返回 false
     */
    static bool dirRemove(const QString &dirPath);

    /**
     * @brief 检查目录是否存在
     * @param dirPath 要检查的路径
     * @return 目录存在返回 true，否则返回 false
     */
    static bool dirExists(const QString &dirPath);

    /**
     * @brief 列出目录内容
     * @param dirPath 目录路径
     * @param nameFilters 名称过滤器（例如 ["*.txt", "*.jpg"]）
     * @param filters 过滤器标志（例如 QDir::Files | QDir::NoDotAndDotDot）
     * @param sort 排序标志（例如 QDir::Name | QDir::IgnoreCase）
     * @return 目录条目列表
     */
    static QStringList dirEntryList(const QString &dirPath,
                                    const QStringList &nameFilters = QStringList(),
                                    QDir::Filters filters = QDir::NoFilter,
                                    QDir::SortFlags sort = QDir::NoSort);

    /**
     * @brief 递归复制目录
     * @param sourceDir 源目录路径
     * @param targetDir 目标目录路径
     * @return 成功返回 true，否则返回 false
     */
    static bool dirCopy(const QString &sourceDir, const QString &targetDir);

    /**
     * @brief 移动目录
     * @param sourceDir 源目录路径
     * @param targetDir 目标目录路径
     * @return 成功返回 true，否则返回 false
     */
    static bool dirMove(const QString &sourceDir, const QString &targetDir);

    /**
     * @brief 递归计算目录大小
     * @param dirPath 目录路径
     * @return 总大小（字节），出错时返回 -1
     */
    static qint64 dirSize(const QString &dirPath);

    /**
     * @brief 获取当前工作目录
     * @return 当前工作目录路径
     */
    static QString dirCurrent();

    /**
     * @brief 设置当前工作目录
     * @param dirPath 要设置为当前目录的路径
     * @return 成功返回 true，否则返回 false
     */
    static bool dirSetCurrent(const QString &dirPath);

    /**
     * @brief 获取用户主目录
     * @return 主目录路径
     */
    static QString dirHome();

    /**
     * @brief 获取系统临时目录
     * @return 临时目录路径
     */
    static QString dirTemp();

    /**
     * @brief 获取根目录
     * @return 根目录路径
     */
    static QString dirRoot();

    // 路径操作
    /**
     * @brief 连接两个路径组件
     * @param path1 第一个路径组件
     * @param path2 第二个路径组件
     * @return 连接后的路径
     */
    static QString pathJoin(const QString &path1, const QString &path2);

    /**
     * @brief 将路径转换为绝对路径
     * @param path 输入路径
     * @return 绝对路径
     */
    static QString pathAbsolute(const QString &path);

    /**
     * @brief 获取相对于基路径的相对路径
     * @param path 目标路径
     * @param base 基路径
     * @return 从基路径到目标的相对路径
     */
    static QString pathRelative(const QString &path, const QString &base);

    /**
     * @brief 获取路径的基本名称（不带扩展名的文件名）
     * @param path 输入路径
     * @return 基本名称
     */
    static QString pathBaseName(const QString &path);

    /**
     * @brief 从路径获取带扩展名的文件名
     * @param path 输入路径
     * @return 带扩展名的文件名
     */
    static QString pathFileName(const QString &path);

    /**
     * @brief 从文件路径获取目录路径
     * @param path 输入路径
     * @return 目录路径
     */
    static QString pathDirName(const QString &path);

    /**
     * @brief 获取文件扩展名（后缀）
     * @param path 输入路径
     * @return 文件扩展名
     */
    static QString pathSuffix(const QString &path);

    /**
     * @brief 获取完整的文件后缀（所有扩展名）
     * @param path 输入路径
     * @return 完整的文件后缀
     */
    static QString pathCompleteSuffix(const QString &path);

    /**
     * @brief 检查路径是否为绝对路径
     * @param path 输入路径
     * @return 如果是绝对路径返回 true，否则返回 false
     */
    static bool pathIsAbsolute(const QString &path);

    /**
     * @brief 检查路径是否为相对路径
     * @param path 输入路径
     * @return 如果是相对路径返回 true，否则返回 false
     */
    static bool pathIsRelative(const QString &path);

    /**
     * @brief 通过移除冗余分隔符和上级引用来清理路径
     * @param path 输入路径
     * @return 清理后的路径
     */
    static QString pathClean(const QString &path);

    /************************ 字符串操作相关 ************************/
    // 字符串处理
    /**
     * @brief 去除字符串两端的空白字符
     * @param str 输入字符串
     * @return 去除空白后的字符串
     */
    static QString strTrim(const QString &str);

    /**
     * @brief 获取字符串的左侧部分
     * @param str 输入字符串
     * @param n 要提取的字符数
     * @return 左侧 n 个字符
     */
    static QString strLeft(const QString &str, int n);

    /**
     * @brief 获取字符串的右侧部分
     * @param str 输入字符串
     * @param n 要提取的字符数
     * @return 右侧 n 个字符
     */
    static QString strRight(const QString &str, int n);

    /**
     * @brief 从字符串中间获取子字符串
     * @param str 输入字符串
     * @param position 起始位置
     * @param n 要提取的字符数（-1 表示所有剩余字符）
     * @return 子字符串
     */
    static QString strMid(const QString &str, int position, int n = -1);

    /**
     * @brief 将字符串转换为小写
     * @param str 输入字符串
     * @return 小写字符串
     */
    static QString strToLower(const QString &str);

    /**
     * @brief 将字符串转换为大写
     * @param str 输入字符串
     * @return 大写字符串
     */
    static QString strToUpper(const QString &str);

    /**
     * @brief 比较两个字符串
     * @param str1 第一个字符串
     * @param str2 第二个字符串
     * @param cs 大小写敏感性
     * @return 如果 str1 < str2 返回负数，相等返回 0，str1 > str2 返回正数
     */
    static int strCompare(const QString &str1, const QString &str2, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * @brief 检查字符串是否以指定子字符串开头
     * @param str 输入字符串
     * @param start 要检查的子字符串
     * @param cs 大小写敏感性
     * @return 如果字符串以子字符串开头返回 true
     */
    static bool strStartsWith(const QString &str, const QString &start, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * @brief 检查字符串是否以指定子字符串结尾
     * @param str 输入字符串
     * @param end 要检查的子字符串
     * @param cs 大小写敏感性
     * @return 如果字符串以子字符串结尾返回 true
     */
    static bool strEndsWith(const QString &str, const QString &end, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * @brief 检查字符串是否包含指定子字符串
     * @param str 输入字符串
     * @param substr 要查找的子字符串
     * @param cs 大小写敏感性
     * @return 如果包含子字符串返回 true
     */
    static bool strContains(const QString &str, const QString &substr, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * @brief 计算子字符串在字符串中出现的次数
     * @param str 输入字符串
     * @param substr 要计数的子字符串
     * @param cs 大小写敏感性
     * @return 子字符串出现的次数
     */
    static int strCount(const QString &str, const QString &substr, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * @brief 替换字符串中的子字符串
     * @param str 输入字符串
     * @param before 要替换的子字符串
     * @param after 替换后的子字符串
     * @param cs 大小写敏感性
     * @return 替换后的字符串
     */
    static QString strReplace(const QString &str, const QString &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * @brief 使用分隔符分割字符串
     * @param str 输入字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串列表
     */
    static QStringList strSplit(const QString &str, const QString &delimiter);

    /**
     * @brief 使用分隔符连接字符串列表
     * @param list 字符串列表
     * @param separator 分隔符
     * @return 连接后的字符串
     */
    static QString strJoin(const QStringList &list, const QString &separator);

    /**
     * @brief 重复字符串指定次数
     * @param str 输入字符串
     * @param times 重复次数
     * @return 重复后的字符串
     */
    static QString strRepeat(const QString &str, int times);

    /**
     * @brief 反转字符串
     * @param str 输入字符串
     * @return 反转后的字符串
     */
    static QString strReverse(const QString &str);

    /**
     * @brief 从字符串中移除子字符串
     * @param str 输入字符串
     * @param substr 要移除的子字符串
     * @param cs 大小写敏感性
     * @return 移除后的字符串
     */
    static QString strRemove(const QString &str, const QString &substr, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    // 字符串编码转换
    /**
     * @brief 将字符串转换为 UTF-8 编码的字节数组
     * @param str 输入字符串
     * @return UTF-8 编码的字节数组
     */
    static QByteArray strToUtf8(const QString &str);

    /**
     * @brief 将字符串转换为本地8位编码的字节数组
     * @param str 输入字符串
     * @return 本地8位编码的字节数组
     */
    static QByteArray strToLocal8Bit(const QString &str);

    /**
     * @brief 从 UTF-8 编码的字节数组创建字符串
     * @param data UTF-8 编码的字节数组
     * @return 创建的字符串
     */
    static QString strFromUtf8(const QByteArray &data);

    /**
     * @brief 从本地8位编码的字节数组创建字符串
     * @param data 本地8位编码的字节数组
     * @return 创建的字符串
     */
    static QString strFromLocal8Bit(const QByteArray &data);

    /**
     * @brief 将字符串转换为 HTML 转义格式
     * @param str 输入字符串
     * @return HTML 转义后的字符串
     */
    static QString strToHtmlEscaped(const QString &str);

    /**
     * @brief 从 HTML 转义格式还原字符串
     * @param str HTML 转义后的字符串
     * @return 还原后的字符串
     */
    static QString strFromHtmlEscaped(const QString &str);

    // 字符串格式化
    /**
     * @brief 格式化字符串（支持最多4个参数）
     * @param format 格式字符串
     * @param arg1 第一个参数
     * @param arg2 第二个参数（可选）
     * @param arg3 第三个参数（可选）
     * @param arg4 第四个参数（可选）
     * @return 格式化后的字符串
     */
    static QString strFormat(const QString &format, const QVariant &arg1, const QVariant &arg2 = QVariant(),
                             const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant());

    /**
     * @brief 将整数转换为字符串
     * @param n 整数
     * @param base 进制（默认为10）
     * @return 字符串表示的整数
     */
    static QString strNumber(int n, int base = 10);

    /**
     * @brief 将浮点数转换为字符串
     * @param n 浮点数
     * @param format 格式字符（'e', 'E', 'f', 'g', 'G'）
     * @param precision 精度
     * @return 字符串表示的浮点数
     */
    static QString strNumber(double n, char format = 'g', int precision = 6);

    /**
     * @brief 使用单个参数格式化字符串
     * @param str 格式字符串
     * @param a 参数
     * @return 格式化后的字符串
     */
    static QString strArg(const QString &str, const QString &a);

    /**
     * @brief 使用整数参数格式化字符串
     * @param str 格式字符串
     * @param a 整数参数
     * @param fieldWidth 字段宽度
     * @param base 进制
     * @param fillChar 填充字符
     * @return 格式化后的字符串
     */
    static QString strArg(const QString &str, int a, int fieldWidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '));

    /**
     * @brief 使用浮点数参数格式化字符串
     * @param str 格式字符串
     * @param a 浮点数参数
     * @param fieldWidth 字段宽度
     * @param format 格式字符
     * @param precision 精度
     * @param fillChar 填充字符
     * @return 格式化后的字符串
     */
    static QString strArg(const QString &str, double a, int fieldWidth = 0, char format = 'g', int precision = -1, QChar fillChar = QLatin1Char(' '));

    // 正则表达式
    /**
     * @brief 使用正则表达式匹配字符串
     * @param str 输入字符串
     * @param pattern 正则表达式模式
     * @return 如果匹配成功返回 true
     */
    static bool regexMatch(const QString &str, const QString &pattern);

    /**
     * @brief 使用正则表达式捕获字符串中的组
     * @param str 输入字符串
     * @param pattern 正则表达式模式
     * @return 捕获的组列表
     */
    static QStringList regexCapture(const QString &str, const QString &pattern);

    /**
     * @brief 使用正则表达式替换字符串
     * @param str 输入字符串
     * @param pattern 正则表达式模式
     * @param after 替换字符串
     * @return 替换后的字符串
     */
    static QString regexReplace(const QString &str, const QString &pattern, const QString &after);

    /************************ 时间操作相关 ************************/
    // 时间获取
    /**
     * @brief 获取当前日期时间
     * @return 当前日期时间
     */
    static QDateTime timeCurrent();

    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 当前时间戳（毫秒）
     */
    static qint64 timeCurrentMSecs();

    /**
     * @brief 获取当前时间戳（秒）
     * @return 当前时间戳（秒）
     */
    static qint64 timeCurrentSecs();

    /**
     * @brief 获取当前时间的字符串表示
     * @param format 时间格式（默认为 "yyyy-MM-dd hh:mm:ss"）
     * @return 格式化后的时间字符串
     */
    static QString timeCurrentString(const QString &format = "yyyy-MM-dd hh:mm:ss");

    // 时间转换
    /**
     * @brief 将日期时间转换为字符串
     * @param datetime 日期时间
     * @param format 时间格式（默认为 "yyyy-MM-dd hh:mm:ss"）
     * @return 格式化后的时间字符串
     */
    static QString timeToString(const QDateTime &datetime, const QString &format = "yyyy-MM-dd hh:mm:ss");

    /**
     * @brief 从字符串解析日期时间
     * @param string 时间字符串
     * @param format 时间格式（默认为 "yyyy-MM-dd hh:mm:ss"）
     * @return 解析后的日期时间
     */
    static QDateTime timeFromString(const QString &string, const QString &format = "yyyy-MM-dd hh:mm:ss");

    /**
     * @brief 将日期时间转换为毫秒时间戳
     * @param datetime 日期时间
     * @return 毫秒时间戳
     */
    static qint64 timeToMSecs(const QDateTime &datetime);

    /**
     * @brief 从毫秒时间戳创建日期时间
     * @param msecs 毫秒时间戳
     * @return 创建的日期时间
     */
    static QDateTime timeFromMSecs(qint64 msecs);

    // 时间计算
    /**
     * @brief 添加天数到日期时间
     * @param datetime 原始日期时间
     * @param days 要添加的天数
     * @return 添加后的日期时间
     */
    static QDateTime timeAddDays(const QDateTime &datetime, int days);

    /**
     * @brief 添加小时到日期时间
     * @param datetime 原始日期时间
     * @param hours 要添加的小时数
     * @return 添加后的日期时间
     */
    static QDateTime timeAddHours(const QDateTime &datetime, int hours);

    /**
     * @brief 添加分钟到日期时间
     * @param datetime 原始日期时间
     * @param minutes 要添加的分钟数
     * @return 添加后的日期时间
     */
    static QDateTime timeAddMinutes(const QDateTime &datetime, int minutes);

    /**
     * @brief 添加秒到日期时间
     * @param datetime 原始日期时间
     * @param seconds 要添加的秒数
     * @return 添加后的日期时间
     */
    static QDateTime timeAddSeconds(const QDateTime &datetime, int seconds);

    /**
     * @brief 添加毫秒到日期时间
     * @param datetime 原始日期时间
     * @param msecs 要添加的毫秒数
     * @return 添加后的日期时间
     */
    static QDateTime timeAddMSecs(const QDateTime &datetime, qint64 msecs);

    /**
     * @brief 计算两个日期时间之间的天数差
     * @param datetime1 第一个日期时间
     * @param datetime2 第二个日期时间
     * @return 天数差
     */
    static qint64 timeDiffDays(const QDateTime &datetime1, const QDateTime &datetime2);

    /**
     * @brief 计算两个日期时间之间的小时差
     * @param datetime1 第一个日期时间
     * @param datetime2 第二个日期时间
     * @return 小时差
     */
    static qint64 timeDiffHours(const QDateTime &datetime1, const QDateTime &datetime2);

    /**
     * @brief 计算两个日期时间之间的分钟差
     * @param datetime1 第一个日期时间
     * @param datetime2 第二个日期时间
     * @return 分钟差
     */
    static qint64 timeDiffMinutes(const QDateTime &datetime1, const QDateTime &datetime2);

    /**
     * @brief 计算两个日期时间之间的秒数差
     * @param datetime1 第一个日期时间
     * @param datetime2 第二个日期时间
     * @return 秒数差
     */
    static qint64 timeDiffSeconds(const QDateTime &datetime1, const QDateTime &datetime2);

    /**
     * @brief 计算两个日期时间之间的毫秒数差
     * @param datetime1 第一个日期时间
     * @param datetime2 第二个日期时间
     * @return 毫秒数差
     */
    static qint64 timeDiffMSecs(const QDateTime &datetime1, const QDateTime &datetime2);

    // 时间判断
    /**
     * @brief 检查时间字符串是否有效
     * @param timeString 时间字符串
     * @param format 时间格式（默认为 "yyyy-MM-dd hh:mm:ss"）
     * @return 如果时间有效返回 true
     */
    static bool timeIsValid(const QString &timeString, const QString &format = "yyyy-MM-dd hh:mm:ss");

    /**
     * @brief 检查年份是否为闰年
     * @param year 年份
     * @return 如果是闰年返回 true
     */
    static bool timeIsLeapYear(int year);

    /**
     * @brief 获取月份中的天数
     * @param year 年份
     * @param month 月份（1-12）
     * @return 该月的天数
     */
    static int timeDaysInMonth(int year, int month);

    /**
     * @brief 获取年份中的天数
     * @param year 年份
     * @return 该年的天数（365或366）
     */
    static int timeDaysInYear(int year);

    /************************ 网络操作相关 ************************/
    // URL操作
    /**
     * @brief 对 URL 进行编码
     * @param url 要编码的 URL
     * @return 编码后的 URL
     */
    static QString urlEncode(const QString &url);

    /**
     * @brief 对 URL 进行解码
     * @param url 要解码的 URL
     * @return 解码后的 URL
     */
    static QString urlDecode(const QString &url);

    /**
     * @brief 将 URL 转换为本地文件路径
     * @param url URL
     * @return 本地文件路径
     */
    static QString urlToLocalFile(const QUrl &url);

    /**
     * @brief 从本地文件路径创建 URL
     * @param filePath 文件路径
     * @return 创建的 URL
     */
    static QUrl urlFromLocalFile(const QString &filePath);

    /**
     * @brief 从 URL 获取文件名
     * @param url URL
     * @return 文件名
     */
    static QString urlFileName(const QUrl &url);

    /**
     * @brief 从 URL 获取路径部分
     * @param url URL
     * @return 路径部分
     */
    static QString urlPath(const QUrl &url);

    /**
     * @brief 从 URL 获取查询部分
     * @param url URL
     * @return 查询部分
     */
    static QString urlQuery(const QUrl &url);

    // 网络请求
    /**
     * @brief 执行 HTTP GET 请求
     * @param url 请求 URL
     * @param timeout 超时时间（毫秒，默认为30000）
     * @return 响应数据
     */
    static QByteArray httpGet(const QUrl &url, int timeout = 30000);

    /**
     * @brief 执行 HTTP POST 请求
     * @param url 请求 URL
     * @param data 要发送的数据
     * @param timeout 超时时间（毫秒，默认为30000）
     * @return 响应数据
     */
    static QByteArray httpPost(const QUrl &url, const QByteArray &data, int timeout = 30000);

    /**
     * @brief 下载文件
     * @param url 文件 URL
     * @param savePath 保存路径
     * @param timeout 超时时间（毫秒，默认为30000）
     * @return 成功返回 true，否则返回 false
     */
    static bool httpDownload(const QUrl &url, const QString &savePath, int timeout = 30000);

    /**
     * @brief 获取当前 HTTP 用户代理
     * @return 用户代理字符串
     */
    static QString httpUserAgent();

    /**
     * @brief 设置 HTTP 用户代理
     * @param userAgent 用户代理字符串
     */
    static void httpSetUserAgent(const QString &userAgent);

    /************************ 图像操作相关 ************************/
    // 图像处理
    /**
     * @brief 从文件加载图像
     * @param filePath 图像文件路径
     * @return 加载的图像
     */
    static QImage imageLoad(const QString &filePath);

    /**
     * @brief 保存图像到文件
     * @param image 要保存的图像
     * @param filePath 文件路径
     * @param format 图像格式（如 "PNG", "JPG"）
     * @param quality 图像质量（-1 表示默认）
     * @return 成功返回 true，否则返回 false
     */
    static bool imageSave(const QImage &image, const QString &filePath, const char *format = nullptr, int quality = -1);

    /**
     * @brief 缩放图像到指定尺寸
     * @param image 原始图像
     * @param size 目标尺寸
     * @param aspectMode 宽高比模式
     * @param transformMode 变换模式
     * @return 缩放后的图像
     */
    static QImage imageScale(const QImage &image, const QSize &size, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
                             Qt::TransformationMode transformMode = Qt::FastTransformation);

    /**
     * @brief 缩放图像到指定宽度和高度
     * @param image 原始图像
     * @param width 目标宽度
     * @param height 目标高度
     * @param aspectMode 宽高比模式
     * @param transformMode 变换模式
     * @return 缩放后的图像
     */
    static QImage imageScale(const QImage &image, int width, int height,
                             Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
                             Qt::TransformationMode transformMode = Qt::FastTransformation);

    /**
     * @brief 裁剪图像
     * @param image 原始图像
     * @param rect 裁剪区域
     * @return 裁剪后的图像
     */
    static QImage imageCrop(const QImage &image, const QRect &rect);

    /**
     * @brief 旋转图像
     * @param image 原始图像
     * @param angle 旋转角度（度）
     * @return 旋转后的图像
     */
    static QImage imageRotate(const QImage &image, double angle);

    /**
     * @brief 镜像图像
     * @param image 原始图像
     * @param horizontal 是否水平镜像
     * @param vertical 是否垂直镜像
     * @return 镜像后的图像
     */
    static QImage imageMirror(const QImage &image, bool horizontal = true, bool vertical = false);

    /**
     * @brief 将图像转换为灰度图
     * @param image 原始图像
     * @return 灰度图像
     */
    static QImage imageGray(const QImage &image);

    /**
     * @brief 模糊图像
     * @param image 原始图像
     * @param radius 模糊半径
     * @return 模糊后的图像
     */
    static QImage imageBlur(const QImage &image, int radius = 5);

    /**
     * @brief 调整图像亮度
     * @param image 原始图像
     * @param delta 亮度增量（负值变暗，正值变亮）
     * @return 调整后的图像
     */
    static QImage imageBrightness(const QImage &image, int delta);

    /**
     * @brief 调整图像对比度
     * @param image 原始图像
     * @param delta 对比度增量
     * @return 调整后的图像
     */
    static QImage imageContrast(const QImage &image, int delta);

    /**
     * @brief 获取图像像素颜色
     * @param image 图像
     * @param x X坐标
     * @param y Y坐标
     * @return 像素颜色
     */
    static QColor imagePixel(const QImage &image, int x, int y);

    /**
     * @brief 获取图像尺寸
     * @param image 图像
     * @return 图像尺寸
     */
    static QSize imageSize(const QImage &image);

    // 图像转换
    /**
     * @brief 将 QImage 转换为 QPixmap
     * @param image 图像
     * @return 转换后的 QPixmap
     */
    static QPixmap imageToPixmap(const QImage &image);

    /**
     * @brief 将 QPixmap 转换为 QImage
     * @param pixmap 像素图
     * @return 转换后的 QImage
     */
    static QImage pixmapToImage(const QPixmap &pixmap);

    /**
     * @brief 将图像转换为字节数组
     * @param image 图像
     * @param format 图像格式
     * @param quality 图像质量
     * @return 包含图像数据的字节数组
     */
    static QByteArray imageToData(const QImage &image, const char *format = "PNG", int quality = -1);

    /**
     * @brief 从字节数组创建图像
     * @param data 包含图像数据的字节数组
     * @return 创建的图像
     */
    static QImage imageFromData(const QByteArray &data);

    /************************ 加密解密相关 ************************/
    // 哈希计算
    /**
     * @brief 计算数据的 MD5 哈希值
     * @param data 输入数据
     * @return MD5 哈希值
     */
    static QByteArray hashMD5(const QByteArray &data);

    /**
     * @brief 计算数据的 SHA1 哈希值
     * @param data 输入数据
     * @return SHA1 哈希值
     */
    static QByteArray hashSHA1(const QByteArray &data);

    /**
     * @brief 计算数据的 SHA256 哈希值
     * @param data 输入数据
     * @return SHA256 哈希值
     */
    static QByteArray hashSHA256(const QByteArray &data);

    /**
     * @brief 计算文件的 MD5 哈希值
     * @param filePath 文件路径
     * @return MD5 哈希值字符串
     */
    static QString hashFileMD5(const QString &filePath);

    /**
     * @brief 计算文件的 SHA1 哈希值
     * @param filePath 文件路径
     * @return SHA1 哈希值字符串
     */
    static QString hashFileSHA1(const QString &filePath);

    /**
     * @brief 计算文件的 SHA256 哈希值
     * @param filePath 文件路径
     * @return SHA256 哈希值字符串
     */
    static QString hashFileSHA256(const QString &filePath);

    // Base64编码
    /**
     * @brief 对数据进行 Base64 编码
     * @param data 输入数据
     * @return Base64 编码的数据
     */
    static QByteArray base64Encode(const QByteArray &data);

    /**
     * @brief 对 Base64 编码的数据进行解码
     * @param data Base64 编码的数据
     * @return 解码后的数据
     */
    static QByteArray base64Decode(const QByteArray &data);

    /**
     * @brief 对字符串进行 Base64 编码
     * @param str 输入字符串
     * @return Base64 编码的字符串
     */
    static QString base64Encode(const QString &str);

    /**
     * @brief 对 Base64 编码的字符串进行解码
     * @param str Base64 编码的字符串
     * @return 解码后的字符串
     */
    static QString base64Decode(const QString &str);

    /************************ 系统操作相关 ************************/
    // 系统信息
    /**
     * @brief 获取操作系统名称
     * @return 操作系统名称
     */
    static QString systemName();

    /**
     * @brief 获取操作系统版本
     * @return 操作系统版本
     */
    static QString systemVersion();

    /**
     * @brief 获取系统架构
     * @return 系统架构
     */
    static QString systemArchitecture();

    /**
     * @brief 获取系统总内存大小
     * @return 总内存大小（字节）
     */
    static qint64 systemMemoryTotal();

    /**
     * @brief 获取系统可用内存大小
     * @return 可用内存大小（字节）
     */
    static qint64 systemMemoryFree();

    /**
     * @brief 获取磁盘总空间
     * @param path 磁盘路径（默认为 "/"）
     * @return 总空间大小（字节）
     */
    static qint64 systemDiskTotal(const QString &path = "/");

    /**
     * @brief 获取磁盘可用空间
     * @param path 磁盘路径（默认为 "/"）
     * @return 可用空间大小（字节）
     */
    static qint64 systemDiskFree(const QString &path = "/");

    /**
     * @brief 获取 CPU 核心数量
     * @return CPU 核心数量
     */
    static int systemCpuCount();

    /**
     * @brief 获取当前用户名
     * @return 用户名
     */
    static QString systemUserName();

    /**
     * @brief 获取计算机名
     * @return 计算机名
     */
    static QString systemMachineName();

    /**
     * @brief 获取系统区域设置
     * @return 区域设置
     */
    static QString systemLocale();

    // 进程操作
    /**
     * @brief 执行外部程序
     * @param program 程序路径
     * @param arguments 参数列表
     * @return 成功返回 true，否则返回 false
     */
    static bool processExecute(const QString &program, const QStringList &arguments = QStringList());

    /**
     * @brief 执行外部程序并获取输出
     * @param program 程序路径
     * @param arguments 参数列表
     * @return 程序输出
     */
    static QByteArray processExecuteOutput(const QString &program, const QStringList &arguments = QStringList());

    /**
     * @brief 获取当前进程 ID
     * @return 进程 ID
     */
    static int processId();

    /**
     * @brief 检查进程是否正在运行
     * @param processName 进程名称
     * @return 如果进程正在运行返回 true
     */
    static bool processIsRunning(const QString &processName);

    /**
     * @brief 终止进程
     * @param processName 进程名称
     * @return 成功返回 true，否则返回 false
     */
    static bool processKill(const QString &processName);

    /************************ 应用程序相关 ************************/
    // 应用信息
    /**
     * @brief 获取应用程序名称
     * @return 应用程序名称
     */
    static QString appName();

    /**
     * @brief 获取应用程序版本
     * @return 应用程序版本
     */
    static QString appVersion();

    /**
     * @brief 获取应用程序组织名称
     * @return 组织名称
     */
    static QString appOrganization();

    /**
     * @brief 获取应用程序域名
     * @return 应用程序域名
     */
    static QString appDomain();

    /**
     * @brief 获取应用程序路径
     * @return 应用程序路径
     */
    static QString appPath();

    /**
     * @brief 获取应用程序文件路径
     * @return 应用程序文件路径
     */
    static QString appFilePath();

    // 应用设置
    /**
     * @brief 设置应用程序配置值
     * @param key 键名
     * @param value 值
     */
    static void appSetValue(const QString &key, const QVariant &value);

    /**
     * @brief 获取应用程序配置值
     * @param key 键名
     * @param defaultValue 默认值
     * @return 配置值
     */
    static QVariant appValue(const QString &key, const QVariant &defaultValue = QVariant());

    /**
     * @brief 移除应用程序配置项
     * @param key 键名
     */
    static void appRemove(const QString &key);

    /**
     * @brief 检查应用程序配置是否包含指定键
     * @param key 键名
     * @return 如果包含返回 true
     */
    static bool appContains(const QString &key);

    // 剪贴板操作
    /**
     * @brief 设置剪贴板文本
     * @param text 文本内容
     */
    static void clipboardSetText(const QString &text);

    /**
     * @brief 获取剪贴板文本
     * @return 剪贴板文本
     */
    static QString clipboardText();

    /**
     * @brief 设置剪贴板图像
     * @param image 图像
     */
    static void clipboardSetImage(const QImage &image);

    /**
     * @brief 获取剪贴板图像
     * @return 剪贴板图像
     */
    static QImage clipboardImage();

    /************************ 界面操作相关 ************************/
    // 屏幕信息
    /**
     * @brief 获取屏幕尺寸
     * @return 屏幕尺寸
     */
    static QSize screenSize();

    /**
     * @brief 获取屏幕宽度
     * @return 屏幕宽度
     */
    static int screenWidth();

    /**
     * @brief 获取屏幕高度
     * @return 屏幕高度
     */
    static int screenHeight();

    /**
     * @brief 获取屏幕 DPI
     * @return 屏幕 DPI
     */
    static int screenDpi();

    /**
     * @brief 获取屏幕缩放比例
     * @return 屏幕缩放比例
     */
    static double screenScale();

    /**
     * @brief 获取屏幕数量
     * @return 屏幕数量
     */
    static int screenCount();

    /**
     * @brief 获取屏幕可用几何区域
     * @param screen 屏幕索引（默认为0）
     * @return 可用几何区域
     */
    static QRect screenAvailableGeometry(int screen = 0);

    // 颜色操作
    /**
     * @brief 从 RGB 值创建颜色
     * @param r 红色分量（0-255）
     * @param g 绿色分量（0-255）
     * @param b 蓝色分量（0-255）
     * @param a 透明度分量（0-255，默认为255）
     * @return 创建的颜色
     */
    static QColor colorFromRgb(int r, int g, int b, int a = 255);

    /**
     * @brief 从 HSV 值创建颜色
     * @param h 色相（0-359）
     * @param s 饱和度（0-255）
     * @param v 明度（0-255）
     * @param a 透明度分量（0-255，默认为255）
     * @return 创建的颜色
     */
    static QColor colorFromHsv(int h, int s, int v, int a = 255);

    /**
     * @brief 从字符串创建颜色
     * @param colorString 颜色字符串（如 "#RRGGBB" 或 "red"）
     * @return 创建的颜色
     */
    static QColor colorFromString(const QString &colorString);

    /**
     * @brief 将颜色转换为字符串
     * @param color 颜色
     * @return 颜色字符串（"#RRGGBB" 格式）
     */
    static QString colorToString(const QColor &color);

    /**
     * @brief 使颜色变亮
     * @param color 原始颜色
     * @param factor 变亮因子（默认为150）
     * @return 变亮后的颜色
     */
    static QColor colorLighter(const QColor &color, int factor = 150);

    /**
     * @brief 使颜色变暗
     * @param color 原始颜色
     * @param factor 变暗因子（默认为150）
     * @return 变暗后的颜色
     */
    static QColor colorDarker(const QColor &color, int factor = 150);

    /**
     * @brief 获取颜色的色相分量
     * @param color 颜色
     * @return 色相值（0-359）
     */
    static int colorHue(const QColor &color);

    /**
     * @brief 获取颜色的饱和度分量
     * @param color 颜色
     * @return 饱和度值（0-255）
     */
    static int colorSaturation(const QColor &color);

    /**
     * @brief 获取颜色的明度分量
     * @param color 颜色
     * @return 明度值（0-255）
     */
    static int colorValue(const QColor &color);

    /**
     * @brief 获取颜色的红色分量
     * @param color 颜色
     * @return 红色值（0-255）
     */
    static int colorRed(const QColor &color);

    /**
     * @brief 获取颜色的绿色分量
     * @param color 颜色
     * @return 绿色值（0-255）
     */
    static int colorGreen(const QColor &color);

    /**
     * @brief 获取颜色的蓝色分量
     * @param color 颜色
     * @return 蓝色值（0-255）
     */
    static int colorBlue(const QColor &color);

    /**
     * @brief 获取颜色的透明度分量
     * @param color 颜色
     * @return 透明度值（0-255）
     */
    static int colorAlpha(const QColor &color);

    /************************ 数据类型转换相关 ************************/
    // 基本类型转换
    /**
     * @brief 将字符串转换为整数
     * @param str 输入字符串
     * @param ok 转换是否成功的标志
     * @param base 进制（默认为10）
     * @return 转换后的整数
     */
    static int toInt(const QString &str, bool *ok = nullptr, int base = 10);

    /**
     * @brief 将字符串转换为双精度浮点数
     * @param str 输入字符串
     * @param ok 转换是否成功的标志
     * @return 转换后的双精度浮点数
     */
    static double toDouble(const QString &str, bool *ok = nullptr);

    /**
     * @brief 将字符串转换为单精度浮点数
     * @param str 输入字符串
     * @param ok 转换是否成功的标志
     * @return 转换后的单精度浮点数
     */
    static float toFloat(const QString &str, bool *ok = nullptr);

    /**
     * @brief 将字符串转换为布尔值
     * @param str 输入字符串
     * @return 转换后的布尔值
     */
    static bool toBool(const QString &str);

    /**
     * @brief 将整数转换为字符串
     * @param n 整数
     * @param base 进制（默认为10）
     * @return 字符串表示的整数
     */
    static QString toString(int n, int base = 10);

    /**
     * @brief 将双精度浮点数转换为字符串
     * @param n 双精度浮点数
     * @param format 格式字符
     * @param precision 精度
     * @return 字符串表示的双精度浮点数
     */
    static QString toString(double n, char format = 'g', int precision = 6);

    /**
     * @brief 将布尔值转换为字符串
     * @param b 布尔值
     * @return 字符串表示的布尔值（"true" 或 "false"）
     */
    static QString toString(bool b);

    // 容器转换
    /**
     * @brief 将整数列表转换为 QVariantList
     * @param list 整数列表
     * @return QVariantList
     */
    static QVariantList toVariantList(const QList<int> &list);

    /**
     * @brief 将字符串列表转换为 QVariantList
     * @param list 字符串列表
     * @return QVariantList
     */
    static QVariantList toVariantList(const QList<QString> &list);

    /**
     * @brief 将 QVariantList 转换为整数列表
     * @param list QVariantList
     * @return 整数列表
     */
    static QList<int> toIntList(const QVariantList &list);

    /**
     * @brief 将 QVariantList 转换为字符串列表
     * @param list QVariantList
     * @return 字符串列表
     */
    static QList<QString> toStringList(const QVariantList &list);

    // JSON转换
    /**
     * @brief 将 JSON 字符串转换为 QJsonObject
     * @param jsonString JSON 字符串
     * @return QJsonObject
     */
    static QJsonObject toJsonObject(const QString &jsonString);

    /**
     * @brief 将 JSON 字符串转换为 QJsonArray
     * @param jsonString JSON 字符串
     * @return QJsonArray
     */
    static QJsonArray toJsonArray(const QString &jsonString);

    /**
     * @brief 将 QJsonObject 转换为 JSON 字符串
     * @param jsonObject QJsonObject
     * @return JSON 字符串
     */
    static QString fromJsonObject(const QJsonObject &jsonObject);

    /**
     * @brief 将 QJsonArray 转换为 JSON 字符串
     * @param jsonArray QJsonArray
     * @return JSON 字符串
     */
    static QString fromJsonArray(const QJsonArray &jsonArray);

    /**
     * @brief 将 QJsonObject 转换为 QVariantMap
     * @param jsonObject QJsonObject
     * @return QVariantMap
     */
    static QVariantMap jsonToMap(const QJsonObject &jsonObject);

    /**
     * @brief 将 QVariantMap 转换为 QJsonObject
     * @param map QVariantMap
     * @return QJsonObject
     */
    static QJsonObject mapToJson(const QVariantMap &map);

    /************************ 其他实用功能 ************************/
    // 随机数生成
    /**
     * @brief 生成指定范围内的随机整数
     * @param min 最小值（包含）
     * @param max 最大值（包含）
     * @return 随机整数
     */
    static int randomInt(int min, int max);

    /**
     * @brief 生成指定范围内的随机浮点数
     * @param min 最小值（包含）
     * @param max 最大值（包含）
     * @return 随机浮点数
     */
    static double randomDouble(double min, double max);

    /**
     * @brief 生成随机字符串
     * @param length 字符串长度
     * @param chars 可用的字符集
     * @return 随机字符串
     */
    static QString randomString(int length, const QString &chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    // 延时操作
    /**
     * @brief 延时指定毫秒数
     * @param msecs 毫秒数
     */
    static void sleep(unsigned long msecs);

    /**
     * @brief 延时指定毫秒数
     * @param msecs 毫秒数
     */
    static void msleep(unsigned long msecs);

    /**
     * @brief 延时指定微秒数
     * @param usecs 微秒数
     */
    static void usleep(unsigned long usecs);

    // 调试输出
    /**
     * @brief 输出调试信息
     * @param message 消息内容
     */
    static void debug(const QString &message);

    /**
     * @brief 输出信息
     * @param message 消息内容
     */
    static void info(const QString &message);

    /**
     * @brief 输出警告信息
     * @param message 消息内容
     */
    static void warning(const QString &message);

    /**
     * @brief 输出错误信息
     * @param message 消息内容
     */
    static void error(const QString &message);

    // 对象操作
    /**
     * @brief 获取对象的类名
     * @param obj QObject 对象
     * @return 类名
     */
    static QString objectClassName(QObject *obj);

    /**
     * @brief 查找子对象
     * @param parent 父对象
     * @param name 对象名称
     * @return 找到的子对象，未找到返回 nullptr
     */
    static QObject* objectFindChild(QObject *parent, const QString &name = QString());

    /**
     * @brief 查找所有子对象
     * @param parent 父对象
     * @param name 对象名称
     * @return 找到的子对象列表
     */
    static QList<QObject*> objectFindChildren(QObject *parent, const QString &name = QString());

    // 信号槽连接
    /**
     * @brief 连接信号和槽
     * @param sender 发送者对象
     * @param signal 信号
     * @param receiver 接收者对象
     * @param method 槽函数
     * @param type 连接类型
     * @return 连接成功返回 true
     */
    static bool connect(const QObject *sender, const char *signal, const QObject *receiver, const char *method, Qt::ConnectionType type = Qt::AutoConnection);

    /**
     * @brief 断开信号和槽的连接
     * @param sender 发送者对象
     * @param signal 信号
     * @param receiver 接收者对象
     * @param method 槽函数
     * @return 断开成功返回 true
     */
    static bool disconnect(const QObject *sender, const char *signal, const QObject *receiver, const char *method);

    // 国际化
    /**
     * @brief 翻译文本
     * @param context 上下文
     * @param sourceText 源文本
     * @param disambiguation 消除歧义文本
     * @param n 数量（用于复数形式）
     * @return 翻译后的文本
     */
    static QString translate(const QString &context, const QString &sourceText, const QString &disambiguation = QString(), int n = -1);

    // 平台相关
    /**
     * @brief 检查是否在 Windows 平台
     * @return 如果是 Windows 平台返回 true
     */
    static bool isWindows();

    /**
     * @brief 检查是否在 macOS 平台
     * @return 如果是 macOS 平台返回 true
     */
    static bool isMac();

    /**
     * @brief 检查是否在 Linux 平台
     * @return 如果是 Linux 平台返回 true
     */
    static bool isLinux();

    /**
     * @brief 检查是否在 Unix 平台
     * @return 如果是 Unix 平台返回 true
     */
    static bool isUnix();

    /**
     * @brief 检查是否在 Android 平台
     * @return 如果是 Android 平台返回 true
     */
    static bool isAndroid();

    /**
     * @brief 检查是否在 iOS 平台
     * @return 如果是 iOS 平台返回 true
     */
    static bool isIOS();

    // 单位转换
    /**
     * @brief 将像素转换为磅
     * @param px 像素值
     * @param dpi DPI（默认为96）
     * @return 磅值
     */
    static int pxToPt(int px, int dpi = 96);

    /**
     * @brief 将磅转换为像素
     * @param pt 磅值
     * @param dpi DPI（默认为96）
     * @return 像素值
     */
    static int ptToPx(int pt, int dpi = 96);

    /**
     * @brief 将毫米转换为像素
     * @param mm 毫米值
     * @param dpi DPI（默认为96）
     * @return 像素值
     */
    static int mmToPx(int mm, int dpi = 96);

    /**
     * @brief 将像素转换为毫米
     * @param px 像素值
     * @param dpi DPI（默认为96）
     * @return 毫米值
     */
    static int pxToMm(int px, int dpi = 96);

    // 验证函数
    /**
     * @brief 验证电子邮件地址是否有效
     * @param email 电子邮件地址
     * @return 如果有效返回 true
     */
    static bool isValidEmail(const QString &email);

    /**
     * @brief 验证 URL 是否有效
     * @param url URL
     * @return 如果有效返回 true
     */
    static bool isValidUrl(const QString &url);

    /**
     * @brief 验证电话号码是否有效
     * @param phone 电话号码
     * @return 如果有效返回 true
     */
    static bool isValidPhone(const QString &phone);

    /**
     * @brief 验证 IP 地址是否有效
     * @param ip IP 地址
     * @return 如果有效返回 true
     */
    static bool isValidIp(const QString &ip);

    /**
     * @brief 验证字符串是否为数字
     * @param str 输入字符串
     * @return 如果是数字返回 true
     */
    static bool isNumber(const QString &str);

    /**
     * @brief 验证字符串是否为整数
     * @param str 输入字符串
     * @return 如果是整数返回 true
     */
    static bool isInteger(const QString &str);

    /**
     * @brief 验证字符串是否为浮点数
     * @param str 输入字符串
     * @return 如果是浮点数返回 true
     */
    static bool isFloat(const QString &str);

    // 数学计算
    /**
     * @brief 四舍五入到指定小数位数
     * @param value 输入值
     * @param decimals 小数位数
     * @return 四舍五入后的值
     */
    static double mathRound(double value, int decimals = 0);

    /**
     * @brief 向上取整
     * @param value 输入值
     * @return 向上取整后的值
     */
    static double mathCeil(double value);

    /**
     * @brief 向下取整
     * @param value 输入值
     * @return 向下取整后的值
     */
    static double mathFloor(double value);

    /**
     * @brief 将整数限制在指定范围内
     * @param value 输入值
     * @param min 最小值
     * @param max 最大值
     * @return 限制后的值
     */
    static int mathClamp(int value, int min, int max);

    /**
     * @brief 将浮点数限制在指定范围内
     * @param value 输入值
     * @param min 最小值
     * @param max 最大值
     * @return 限制后的值
     */
    static double mathClamp(double value, double min, double max);

    /**
     * @brief 整数线性插值
     * @param a 起始值
     * @param b 结束值
     * @param t 插值系数（0.0-1.0）
     * @return 插值结果
     */
    static int mathLerp(int a, int b, double t);

    /**
     * @brief 浮点数线性插值
     * @param a 起始值
     * @param b 结束值
     * @param t 插值系数（0.0-1.0）
     * @return 插值结果
     */
    static double mathLerp(double a, double b, double t);

private:
    // 网络管理器实例（如果需要保持状态）
    /**
     * @brief 获取网络管理器实例
     * @return 网络管理器实例
     */
    static QNetworkAccessManager *networkManager();

    // 应用设置实例
    /**
     * @brief 获取应用设置实例
     * @return 应用设置实例
     */
    static QSettings *appSettings();
};
