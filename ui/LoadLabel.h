#pragma once

#include <QLabel>
#include <QTimerEvent>
#include <QMovie>

// 自定义加载标签：显示多个设置图片，例如多个加载图片
class LoadLabel : public QLabel
{
    Q_OBJECT

public:
    // 枚举加载类型
    enum LoadType
    {
        GIF = 0,
        PNG = 1
    };

    explicit LoadLabel(QWidget *parent = nullptr);

    void setSize(QSize size);                                        // 设置图片显示大小
    void setImageCount(const QString &imagePath, int imageCount);    // 设置图片路径和总数目
    void setInterval(int interval);                                  // 设置定时间隔时间
    void startAnimation();                                           // 开启动画
    void stopAnimation();                                            // 停止动画
    void setLoadType(LoadType type);                                 // 设置加载类型
    void setGifPath(const QString &gifPath);                         // 设置gif图片路径

protected:
    void timerEvent(QTimerEvent *event) override;                    // 定时器事件

private:
    int m_timerId;                                      // 定时器id
    bool m_isAnimating = false;                         // 是否正在动画
    int m_interval = 100;                               // 定时间隔
    int m_nIndex = 0;                                   // 当前图片下标
    QString m_imagePath = "";                           // 图片路径
    int m_imageCount = 1;                               // 图片总数目
    QSize m_size = QSize(16, 16);                       // 图片显示大小
    QString m_gifPath = ":/loading/icon-loading.gif";   // gif图片路径
    LoadType m_loadType = LoadType::PNG;                // 加载类型
    QMovie *m_movie = nullptr;                          // 用于播放GIF
};
