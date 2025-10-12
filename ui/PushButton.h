#pragma once

#include <QPushButton>
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QFont>
#include <QTimer>
#include <QLinearGradient>

class PushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    explicit PushButton(QWidget *parent = nullptr)
        : QPushButton(parent),
        m_isActive(false),
        m_activeColor(231, 76, 60),    // 红色
        m_inactiveColor(46, 204, 113), // 绿色
        m_animationDuration(300),
        m_buttonEnableDelay(1000)
    {
        // 设置按钮大小和字体
        setFixedSize(220, 50);
        QFont font("Microsoft YaHei", 16, QFont::Bold);
        setFont(font);

        // 添加阴影效果
        auto shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(10);
        shadowEffect->setColor(QColor(0, 0, 0, 160));
        shadowEffect->setOffset(0, 2);
        setGraphicsEffect(shadowEffect);

        // 初始颜色
        setColor(m_inactiveColor);
        setText("开启自动测试");

        // 连接点击信号
        connect(this, &QPushButton::clicked, this, &PushButton::toggleState);
    }

    // 设置按钮大小
    void setButtonSize(int width, int height)
    {
        setFixedSize(width, height);
        updateStyle();
	}

    // 设置按钮文本
    void setButtonText(const QString& activeText, const QString& inactiveText)
    {
        m_activeText = activeText;
        m_inactiveText = inactiveText;
        updateButtonAppearance();
    }

    // 设置按钮颜色
    void setButtonColors(const QColor& activeColor, const QColor& inactiveColor)
    {
        m_activeColor = activeColor;
        m_inactiveColor = inactiveColor;
        updateButtonAppearance();
    }

    // 设置动画持续时间
    void setAnimationDuration(int duration)
    {
        m_animationDuration = duration;
    }

    // 设置按钮启用延迟
    void setButtonEnableDelay(int delay)
    {
        m_buttonEnableDelay = delay;
    }

    QColor color() const
    {
        return m_color;
    }

    void setColor(const QColor &color)
    {
        if (m_color != color)
        {
            m_color = color;
            updateStyle();
        }
    }

    // 设置按钮状态
    void setButtonState(bool state)
    {
        if (m_isActive != state)
        {
            m_isActive = state;
            animateStateChange();
        }
    }

    // 获取当前按钮状态
    bool buttonState() const
    {
        return m_isActive;
    }

public slots:
    void toggleState()
    {
        this->setEnabled(false);
        m_isActive = !m_isActive;
        animateStateChange();

        // 延迟重新启用按钮
        QTimer::singleShot(m_buttonEnableDelay, this, [this]() { this->setEnabled(true); });
    }

private:
    // 动画状态变化
    void animateStateChange()
    {
        QPropertyAnimation* animation = new QPropertyAnimation(this, "color");
        animation->setDuration(m_animationDuration);

        if (m_isActive)
        {
            animation->setStartValue(m_inactiveColor);
            animation->setEndValue(m_activeColor);
        }
        else
        {
            animation->setStartValue(m_activeColor);
            animation->setEndValue(m_inactiveColor);
        }

        animation->start(QAbstractAnimation::DeleteWhenStopped);
        updateButtonAppearance();
    }

    // 更新按钮外观
    void updateButtonAppearance()
    {
        setText(m_isActive ? m_activeText : m_inactiveText);
        updateStyle();
    }

    // 更新样式
    void updateStyle()
    {
        // 创建渐变
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, m_color.lighter(120));
        gradient.setColorAt(1, m_color);

        QString gradientStyle = QString("qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2)").arg(m_color.lighter(120).name(), m_color.name());

        QString hoverGradientStyle = QString("qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2)").arg(m_color.lighter(140).name(), m_color.lighter(120).name());

        QString pressedGradientStyle = QString("qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2)").arg(m_color.darker(120).name(), m_color.darker(140).name());

        QString style = QString(
                            "QPushButton {"
                            "   background: %1;"
                            "   color: white;"
                            "   border: none;"
                            "   border-radius: 10px;"
                            "   padding: 10px;"
                            "}"
                            "QPushButton:hover {"
                            "   background: %2;"
                            "}"
                            "QPushButton:pressed {"
                            "   background: %3;"
                            "}"
                            "QPushButton:disabled {"
                            "   background: %4;"
                            "}"
                            ).arg(gradientStyle, hoverGradientStyle, pressedGradientStyle, m_color.darker(150).name());

        setStyleSheet(style);
    }

private:
    bool m_isActive;
    QColor m_color;
    QColor m_activeColor;
    QColor m_inactiveColor;
    QString m_activeText{"关闭自动测试"};
    QString m_inactiveText{"开启自动测试"};
    int m_animationDuration;
    int m_buttonEnableDelay;
};
