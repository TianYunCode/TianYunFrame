#pragma once

#include <QObject>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QMessageBox>
#include <QVariant>

#include <QDateTime>
#include <QFile>
#include <QSize>
#include <random>

#include <QProcess>
#include <QRegularExpression>
#include <QDebug>
#include <QTimer>

class StaticTool : public QObject
{
    Q_OBJECT

public:
    explicit StaticTool(QObject* parent = nullptr) = delete;

    static std::function<void(QWidget*)> repolish;

    //读取 filePath 文件全部数据并返回
    static QByteArray readFileAllData(const QString& filePath);

    //返回 str 最后一个 '/' 之后的字符串
    static QString extractVideoName(const QString& str);

    //返回当前日期和时间
    static QString getCurrentDateTimeString(const QString& format = "yyyy_MM_dd_HH_mm_ss");

    //返回当前日期
    static QString getCurrentDateString(const QString& format = "yyyy-MM-dd");

    //返回当前时间
    static QString getCurrentTimeString(const QString& format = "HH-mm-ss");

    static QString convertMacSeparator(const QString& macAddress);

    // 生成随机数
    template<typename T>
    static T generateRandom(T min_val, T max_val)
    {
        static_assert(std::is_arithmetic<T>::value, "T 必须是数值类型（整型或浮点型）");

        // 确保最小值小于最大值
        if (min_val > max_val)
        {
            std::swap(min_val, max_val);
        }

        // 使用线程安全的随机数生成器
        static thread_local std::random_device rd;
        static thread_local std::mt19937 engine(rd());

        // 根据类型选择分布
        if constexpr (std::is_integral<T>::value)
        {
            std::uniform_int_distribution<T> dist(min_val, max_val);
            return dist(engine);
        }
        else
        {
            std::uniform_real_distribution<T> dist(min_val, max_val);
            return dist(engine);
        }
    }

    // 判断当前函数是否在主线程中执行
    static QString isMainThreadRun(const QString& funcName, const qlonglong& threadID);

    // MAC字符串转64位整数
    static quint64 macToInt(const QString& mac);

    // 整数转回MAC字符串
    static QString intToMac(quint64 mac);

    // 显示提示信息
    static void showHint(const QWidget* parent, const QString& hintInfo, const uint& showTime);

public:
    static qlonglong mainThreadID;              // 主线程ID
};
