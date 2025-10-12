#pragma once

#include <QWidget>
#include <QTimer>
#include <QColor>

// LED指示灯控件，支持多种颜色、闪烁和呼吸效果
class LedIndicator : public QWidget
{
    Q_OBJECT

public:
    // LED状态枚举
    enum State
    {
        Off,        // 关闭
        Red,        // 红色
        Green,      // 绿色
        Yellow      // 黄色
    };
    Q_ENUM(State)

    explicit LedIndicator(QWidget *parent = nullptr);

    // 获取当前状态
    State state() const;

    // 是否正在闪烁
    bool isFlashing() const;

    // 正常尺寸（32, 32）
    QSize sizeHint() const override;

    // 最小尺寸（24, 24）
    QSize minimumSizeHint() const override;

public slots:
    // 设置状态
    void setState(State state);

    // 设置闪烁效果，interval为闪烁间隔（毫秒）
    void setFlashing(bool flashing, int interval = 500);

    // 设置呼吸效果
    void setBreathing(bool breathing);

    // 切换状态（Off -> Red -> Green -> Yellow -> Off）
    void toggle();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 更新闪烁状态
    void updateFlash();

    // 更新呼吸状态
    void updateBreath();

private:
    // 更新外观
    void updateAppearance();

    // 获取当前显示颜色（考虑闪烁和呼吸效果）
    QColor currentDisplayColor() const;

    State m_state;          // 当前状态
    bool m_flashing;        // 是否闪烁
    bool m_breathing;       // 是否呼吸
    bool m_flashState;      // 当前闪烁状态（亮/灭）
    int m_flashInterval;    // 闪烁间隔（毫秒）
    qreal m_breathValue;    // 呼吸周期值（0到2π）
    QTimer m_flashTimer;    // 闪烁定时器
    QTimer m_breathTimer;   // 呼吸定时器
    QColor m_baseColor;     // 基础颜色
    QColor m_darkColor;     // 暗色（用于呼吸效果）
};
