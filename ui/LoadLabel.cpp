#include "LoadLabel.h"
#include <QMovie>
#include <QDebug>

LoadLabel::LoadLabel(QWidget *parent) : QLabel(parent)
{
    m_movie = new QMovie(this);
    this->setAlignment(Qt::AlignCenter);
}

void LoadLabel::setSize(QSize size)
{
    m_size = size;

    // 如果当前正在播放GIF，更新GIF大小
    if (m_loadType == GIF && m_movie && m_isAnimating)
    {
        m_movie->setScaledSize(m_size);
    }
}

// 设置图片路径和总数目
void LoadLabel::setImageCount(const QString &imagePath, int imageCount)
{
    m_imagePath = imagePath;
    m_imageCount = imageCount;
}

void LoadLabel::setInterval(int interval)
{
    m_interval = interval;
}

void LoadLabel::startAnimation()
{
    if (!m_isAnimating)
    {
        switch(m_loadType)
        {
        case LoadType::GIF:
        {
            m_movie->setFileName(m_gifPath);
            m_movie->setScaledSize(m_size);
            this->setMovie(m_movie);
            m_movie->start();
            break;
        }
        case LoadType::PNG:
        {
            // 计算每帧间隔
            int frameInterval = m_interval / m_imageCount;
            m_timerId = startTimer(frameInterval);
            break;
        }
        default:
            break;
        }

        m_isAnimating = true;
    }
}

void LoadLabel::stopAnimation()
{
    if (m_isAnimating)
    {
        switch(m_loadType)
        {
        case LoadType::GIF:
        {
            m_movie->stop();
            this->setMovie(nullptr);
            break;
        }
        case LoadType::PNG:
        {
            killTimer(m_timerId);
            m_timerId = 0;
            break;
        }
        }

        m_isAnimating = false;
    }
}

void LoadLabel::setLoadType(LoadType type)
{
    // 如果当前正在动画，先停止
    if (m_isAnimating)
    {
        stopAnimation();
    }

    m_loadType = type;
}

void LoadLabel::setGifPath(const QString &gifPath)
{
    m_gifPath = gifPath;

    // 如果当前是GIF类型且正在播放，重新设置GIF
    if (m_loadType == GIF && m_isAnimating)
    {
        stopAnimation();
        startAnimation();
    }
}

// 定时器事件
void LoadLabel::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerId)
    {
        // 更新当前图片下标
        m_nIndex = (m_nIndex % m_imageCount) + 1;

        // 加载并显示图片
        QString imageName = QString("%1%2.png").arg(m_imagePath).arg(m_nIndex);
        QPixmap pixmap(imageName);
        if (!pixmap.isNull())
        {
            pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            this->setPixmap(pixmap);
        }
    }
}
