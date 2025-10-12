#pragma once

#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QPixmap>
#include <QList>
#include <QSize>
#include <QSizeF>

class ScreenUtils
{
public:
    // 初始化所有屏幕参数（应在应用程序启动时调用）
    static void initialize()
    {
        primaryResolution = getPrimaryScreenResolution();
        allResolutions = getAllScreensResolution();
        availableSize = getPrimaryScreenAvailableSize();
        physicalSize = getPrimaryScreenPhysicalSize();
        dpi = getPrimaryScreenDpi();

        // 预计算屏幕尺寸阈值
        screenSizeThreshold = primaryResolution.width() + primaryResolution.height();
    }

    // 获取主屏幕分辨率
    static QSize getPrimaryScreenResolution()
    {
        if (const QScreen* primaryScreen = QGuiApplication::primaryScreen())
            return primaryScreen->size();
        return QSize(0, 0);
    }

    // 获取所有屏幕的分辨率
    static QList<QSize> getAllScreensResolution()
    {
        QList<QSize> resolutions;
        const auto screens = QGuiApplication::screens();

        resolutions.reserve(screens.size());
        for (const QScreen* screen : screens)
            resolutions.append(screen->size());

        return resolutions;
    }

    // 获取屏幕的可用分辨率(除去任务栏等)
    static QSize getPrimaryScreenAvailableSize()
    {
        if (const QScreen* primaryScreen = QGuiApplication::primaryScreen())
            return primaryScreen->availableSize();
        return QSize(0, 0);
    }

    // 获取屏幕的物理尺寸(mm)
    static QSizeF getPrimaryScreenPhysicalSize()
    {
        if (const QScreen* primaryScreen = QGuiApplication::primaryScreen())
            return primaryScreen->physicalSize();
        return QSizeF(0.0, 0.0);
    }

    // 获取屏幕的DPI
    static qreal getPrimaryScreenDpi()
    {
        if (const QScreen* primaryScreen = QGuiApplication::primaryScreen())
            return primaryScreen->logicalDotsPerInch();
        return 96.0; // 返回默认DPI值
    }

    // 根据屏幕尺寸获取适配后的大小
    static uint getAdaptiveSize(uint baseSize)
    {
        if (screenSizeThreshold < (1920 + 1080))
            return baseSize * 2 / 3;  // 减小1/3

        if (screenSizeThreshold > (1920 + 1080))
            return baseSize * 4 / 3;  // 增大1/3

        return baseSize;
    }

    // 获取适配后的字体
    static QFont getAdaptiveFont(uint baseSize)
    {
        QFont font;
        font.setPointSize(getAdaptiveSize(baseSize));
        return font;
    }

    // 获取适配后的位图
    static QPixmap getAdaptivePixmap(const QString& iconPath, uint width, uint height)
    {
        QPixmap pixmap(iconPath);
        if (pixmap.isNull()) {
            qWarning() << "Failed to load pixmap from:" << iconPath;
            return pixmap;
        }

        const uint adaptiveWidth = getAdaptiveSize(width);
        const uint adaptiveHeight = getAdaptiveSize(height);

        return pixmap.scaled(adaptiveWidth, adaptiveHeight,
                             Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 屏幕参数访问器
    static QSize primaryScreenResolution() { return primaryResolution; }
    static QList<QSize> allScreenResolutions() { return allResolutions; }
    static QSize primaryScreenAvailableSize() { return availableSize; }
    static QSizeF primaryScreenPhysicalSize() { return physicalSize; }
    static qreal primaryScreenDpi() { return dpi; }

private:
    ScreenUtils() = delete; // 禁止实例化
    ~ScreenUtils() = delete;

    inline static QSize primaryResolution;
    inline static QList<QSize> allResolutions;
    inline static QSize availableSize;
    inline static QSizeF physicalSize;
    inline static qreal dpi;
    inline static int screenSizeThreshold = 0;
};
