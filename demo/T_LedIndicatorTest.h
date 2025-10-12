#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include "LedIndicator.h"

class T_LedIndicatorTest : public QMainWindow
{
    Q_OBJECT

public:
    T_LedIndicatorTest(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupConnections();
    }

private:
    void setupUI()
    {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // 创建LED指示灯
        QHBoxLayout *ledLayout = new QHBoxLayout();
        m_led = new LedIndicator(this);
        ledLayout->addStretch();
        ledLayout->addWidget(m_led);
        ledLayout->addStretch();
        mainLayout->addLayout(ledLayout);

        // 创建状态控制区域
        QGroupBox *stateGroup = new QGroupBox("状态控制", this);
        QHBoxLayout *stateLayout = new QHBoxLayout(stateGroup);

        QPushButton *offBtn = new QPushButton("关闭", stateGroup);
        offBtn->setProperty("state", LedIndicator::Off);
        QPushButton *redBtn = new QPushButton("红色", stateGroup);
        redBtn->setProperty("state", LedIndicator::Red);
        QPushButton *greenBtn = new QPushButton("绿色", stateGroup);
        greenBtn->setProperty("state", LedIndicator::Green);
        QPushButton *yellowBtn = new QPushButton("黄色", stateGroup);
        yellowBtn->setProperty("state", LedIndicator::Yellow);
        QPushButton *toggleBtn = new QPushButton("切换", stateGroup);

        stateLayout->addWidget(offBtn);
        stateLayout->addWidget(redBtn);
        stateLayout->addWidget(greenBtn);
        stateLayout->addWidget(yellowBtn);
        stateLayout->addWidget(toggleBtn);

        mainLayout->addWidget(stateGroup);

        // 创建效果控制区域
        QGroupBox *effectGroup = new QGroupBox("效果控制", this);
        QGridLayout *effectLayout = new QGridLayout(effectGroup);

        m_flashCheckBox = new QCheckBox("闪烁", effectGroup);
        m_breathCheckBox = new QCheckBox("呼吸", effectGroup);
        QLabel *intervalLabel = new QLabel("闪烁间隔(ms):", effectGroup);
        m_intervalSpinBox = new QSpinBox(effectGroup);
        m_intervalSpinBox->setRange(100, 2000);
        m_intervalSpinBox->setValue(500);
        m_intervalSpinBox->setSingleStep(100);

        effectLayout->addWidget(m_flashCheckBox, 0, 0);
        effectLayout->addWidget(m_breathCheckBox, 0, 1);
        effectLayout->addWidget(intervalLabel, 1, 0);
        effectLayout->addWidget(m_intervalSpinBox, 1, 1);

        mainLayout->addWidget(effectGroup);
        mainLayout->addStretch();

        setCentralWidget(centralWidget);
        setWindowTitle("LED指示灯Demo");
        resize(400, 300);
    }

    void setupConnections()
    {
        // 连接状态按钮
        QList<QPushButton*> buttons = findChildren<QPushButton*>();
        foreach (QPushButton *btn, buttons)
        {
            if (btn->property("state").isValid())
            {
                connect(btn, &QPushButton::clicked, this, [this, btn]() { m_led->setState(static_cast<LedIndicator::State>(btn->property("state").toInt())); });
            }
        }

        // 连接切换按钮
        QPushButton *toggleBtn = findChild<QPushButton*>(QString(), Qt::FindDirectChildrenOnly);
        connect(toggleBtn, &QPushButton::clicked, m_led, &LedIndicator::toggle);

        // 连接效果控制
        connect(m_flashCheckBox, &QCheckBox::toggled, this, [this](bool checked) { m_led->setFlashing(checked, m_intervalSpinBox->value()); });

        connect(m_breathCheckBox, &QCheckBox::toggled, m_led, &LedIndicator::setBreathing);

        connect(m_intervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) { if (m_flashCheckBox->isChecked()) { m_led->setFlashing(true, value); } });
    }


    LedIndicator *m_led;
    QCheckBox *m_flashCheckBox;
    QCheckBox *m_breathCheckBox;
    QSpinBox *m_intervalSpinBox;
};
