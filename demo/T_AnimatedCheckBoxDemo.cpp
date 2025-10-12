#include "DemoHead.h"

#if T_AnimatedCheckBoxDemo

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpacerItem>
#include "AnimatedCheckBox.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建主窗口
    QWidget window;
    window.setWindowTitle("AnimatedCheckBox 演示");
    window.resize(400, 300);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);

    // 添加标题
    QLabel *titleLabel = new QLabel("AnimatedCheckBox 演示");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // 添加基本示例组
    QGroupBox *basicGroup = new QGroupBox("基本示例");
    QHBoxLayout *basicLayout = new QHBoxLayout(basicGroup);

    AnimatedCheckBox *checkBox1 = new AnimatedCheckBox;
    checkBox1->setFixedSize(40, 20);
    checkBox1->setText("选项 1");
    basicLayout->addWidget(checkBox1);

    AnimatedCheckBox *checkBox2 = new AnimatedCheckBox;
    checkBox2->setFixedSize(40, 20);
    checkBox2->setText("选项 2");
    checkBox2->setChecked(true);
    basicLayout->addWidget(checkBox2);

    AnimatedCheckBox *checkBox3 = new AnimatedCheckBox;
    checkBox3->setFixedSize(40, 20);
    checkBox3->setText("选项 3");
    basicLayout->addWidget(checkBox3);
    basicLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    mainLayout->addWidget(basicGroup);

    // 添加状态显示组
    QGroupBox *statusGroup = new QGroupBox("状态反馈");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusGroup);

    QLabel *statusLabel = new QLabel("请切换复选框状态");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLayout->addWidget(statusLabel);

    AnimatedCheckBox *statusCheckBox = new AnimatedCheckBox;
    statusCheckBox->setFixedSize(40, 20);
    statusCheckBox->setText("切换我看看状态变化");
    statusLayout->addWidget(statusCheckBox);
    statusLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // 连接状态变化信号
    QObject::connect(statusCheckBox, &QCheckBox::toggled, [statusLabel](bool checked) { statusLabel->setText(checked ? "复选框已选中" : "复选框未选中"); });

    mainLayout->addWidget(statusGroup);

    // 添加禁用状态示例
    QGroupBox *disabledGroup = new QGroupBox("禁用状态示例");
    QHBoxLayout *disabledLayout = new QHBoxLayout(disabledGroup);

    AnimatedCheckBox *disabledChecked = new AnimatedCheckBox;
    disabledChecked->setFixedSize(40, 20);
    disabledChecked->setText("已选中但禁用");
    disabledChecked->setChecked(true);
    disabledChecked->setEnabled(false);
    disabledLayout->addWidget(disabledChecked);

    AnimatedCheckBox *disabledUnchecked = new AnimatedCheckBox;
    disabledUnchecked->setFixedSize(40, 20);
    disabledUnchecked->setText("未选中且禁用");
    disabledUnchecked->setChecked(false);
    disabledUnchecked->setEnabled(false);
    disabledLayout->addWidget(disabledUnchecked);
    disabledLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    mainLayout->addWidget(disabledGroup);

    // 添加一些间距
    mainLayout->addSpacing(20);

    // 显示窗口
    window.show();

    return app.exec();
}

#endif
