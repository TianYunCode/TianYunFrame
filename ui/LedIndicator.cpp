#include "ledindicator.h"
#include <QPainter>
#include <QConicalGradient>
#include <cmath>

LedIndicator::LedIndicator(QWidget *parent) : QWidget(parent), m_state(State::Off), m_flashing(false), m_breathing(false), m_flashState(false), m_flashInterval(500), m_breathValue(0.0)
{
    // 设置最小尺寸
    setMinimumSize(24, 24);

    // 允许扩展以适应布局
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    // 连接闪烁和呼吸定时器
    connect(&m_flashTimer, &QTimer::timeout, this, &LedIndicator::updateFlash);
    connect(&m_breathTimer, &QTimer::timeout, this, &LedIndicator::updateBreath);

    // 初始状态为关闭
    setState(State::Off);
}

LedIndicator::State LedIndicator::state() const
{
    return m_state;
}

bool LedIndicator::isFlashing() const
{
    return m_flashing;
}

QSize LedIndicator::sizeHint() const
{
    return QSize(32, 32);
}

QSize LedIndicator::minimumSizeHint() const
{
    return QSize(24, 24);
}

void LedIndicator::setState(State state)
{
    // 状态未改变则不更新
    if (m_state == state)
    {
        return;
    }

    // 更新状态
    m_state = state;

    // 根据状态设置基本颜色
    switch (state)
    {
    case State::Red:
        m_baseColor = Qt::red;

        break;
    case State::Green:
        m_baseColor = Qt::green;

        break;
    case State::Yellow:
        m_baseColor = Qt::yellow;

        break;
    case State::Off:
        m_baseColor = Qt::darkGray;

        break;
    }

    // 计算暗色用于呼吸效果
    m_darkColor = m_baseColor.darker(300);

    // 重新配置定时器和外观
    updateAppearance();
}

void LedIndicator::setFlashing(bool flashing, int interval)
{
    // 参数检查
    if (m_flashing == flashing && m_flashInterval == interval)
    {
        return;
    }

    // 更新闪烁状态和间隔
    m_flashing = flashing;
    m_flashInterval = interval;

    // 重新配置定时器和外观
    m_flashTimer.stop();
    if (m_flashing && m_state != State::Off)
    {
        m_flashTimer.start(m_flashInterval);
        m_flashState = true;
    }

    update();
}

void LedIndicator::setBreathing(bool breathing)
{
    // 参数检查
    if (m_breathing == breathing)
    {
        return;
    }

    // 更新呼吸状态
    m_breathing = breathing;

    // 重新配置定时器和外观
    m_breathTimer.stop();
    if (m_breathing && m_state != State::Off)
    {
        m_breathTimer.start(30);
        m_breathValue = 0.0;
    }

    update();
}

void LedIndicator::toggle()
{
    setState(static_cast<State>((m_state + 1) % 4));
}

void LedIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal radius = qMin(width(), height()) / 2.0 - 2;
    const QPointF center(width() / 2.0, height() / 2.0);

    // 绘制背景
    QRadialGradient bgGradient(center, radius, center);
    bgGradient.setColorAt(0, Qt::white);
    bgGradient.setColorAt(1, QColor(220, 220, 220));
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgGradient);
    painter.drawEllipse(center, radius, radius);

    // 绘制LED主体
    QColor ledColor = currentDisplayColor();

    // 创建更醒目的闪烁效果 - 完全关闭或全亮
    if (m_flashing && !m_flashState)
    {
        ledColor = m_darkColor.darker(200);
    }

    QRadialGradient ledGradient(center, radius * 0.8, center);
    ledGradient.setColorAt(0, ledColor.lighter(150));
    ledGradient.setColorAt(0.7, ledColor);
    ledGradient.setColorAt(1, ledColor.darker(150));

    painter.setPen(QPen(Qt::darkGray, 1));
    painter.setBrush(ledGradient);
    painter.drawEllipse(center, radius * 0.9, radius * 0.9);

    // 添加高光效果
    if (m_state != Off && (!m_flashing || m_flashState))
    {
        QRadialGradient highlight(center - QPointF(radius*0.3, radius*0.3), radius*0.3, center - QPointF(radius*0.3, radius*0.3));
        highlight.setColorAt(0, QColor(255, 255, 255, 180));
        highlight.setColorAt(1, Qt::transparent);

        painter.setPen(Qt::NoPen);
        painter.setBrush(highlight);
        painter.drawEllipse(center - QPointF(radius*0.3, radius*0.3), radius*0.3, radius*0.3);
    }
}

void LedIndicator::updateFlash()
{
    m_flashState = !m_flashState;
    update();
}

void LedIndicator::updateBreath()
{
    m_breathValue += 0.05;
    if (m_breathValue > 2 * M_PI)
    {
        m_breathValue = 0.0;
    }

    update();
}

void LedIndicator::updateAppearance()
{
    // 停止所有定时器
    m_flashTimer.stop();
    m_breathTimer.stop();

    // 根据当前状态重新配置定时器
    if (m_state == Off)
    {
        // 关闭状态不闪烁不呼吸
        m_flashing = false;
        m_breathing = false;
    }
    else
    {
        // 启用闪烁和呼吸效果（如果设置了）
        if (m_flashing)
        {
            m_flashTimer.start(m_flashInterval);
            m_flashState = true;
        }

        if (m_breathing)
        {
            m_breathTimer.start(30);
            m_breathValue = 0.0;
        }
    }

    // 强制重绘
    update();
}

QColor LedIndicator::currentDisplayColor() const
{
    // 关闭状态直接返回暗色
    if (m_state == Off)
    {
        return m_baseColor;
    }

    // 呼吸效果计算颜色
    if (m_breathing)
    {
        // 计算呼吸因子（0到1之间变化）
        qreal factor = 0.5 * (1 + sin(m_breathValue));

        // 线性插值计算当前颜色
        uint redValue = m_baseColor.red() * factor + m_darkColor.red() * (1 - factor);
        uint greenValue = m_baseColor.green() * factor + m_darkColor.green() * (1 - factor);
        uint blueValue = m_baseColor.blue() * factor + m_darkColor.blue() * (1 - factor);

        return QColor(redValue, greenValue, blueValue);
    }

    return m_baseColor;
}
