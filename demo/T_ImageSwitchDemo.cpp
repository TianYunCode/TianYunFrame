#include "DemoHead.h"

#if T_ImageSwitchDemo

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include "ImageSwitch.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建主窗口
    QWidget window;
    window.setWindowTitle("ImageSwitch 控件示例");
    window.resize(400, 300);

    // 创建垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);

    // 创建样式选择区域
    QGroupBox *styleGroup = new QGroupBox("选择开关样式");
    QHBoxLayout *styleLayout = new QHBoxLayout(styleGroup);

    ImageSwitch *switch1 = new ImageSwitch();
    switch1->setButtonStyle(ImageSwitch::ButtonStyle_1);
    styleLayout->addWidget(switch1);

    ImageSwitch *switch2 = new ImageSwitch();
    switch2->setButtonStyle(ImageSwitch::ButtonStyle_2);
    styleLayout->addWidget(switch2);

    ImageSwitch *switch3 = new ImageSwitch();
    switch3->setButtonStyle(ImageSwitch::ButtonStyle_3);
    styleLayout->addWidget(switch3);

    ImageSwitch *switch4 = new ImageSwitch();
    switch4->setButtonStyle(ImageSwitch::ButtonStyle_4);
    styleLayout->addWidget(switch4);

    mainLayout->addWidget(styleGroup);

    // 创建状态显示区域
    QGroupBox *statusGroup = new QGroupBox("开关状态");
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

    QLabel *statusLabel1 = new QLabel("样式1: 关闭");
    QLabel *statusLabel2 = new QLabel("样式2: 关闭");
    QLabel *statusLabel3 = new QLabel("样式3: 关闭");
    QLabel *statusLabel4 = new QLabel("样式4: 关闭");

    statusLayout->addWidget(statusLabel1);
    statusLayout->addWidget(statusLabel2);
    statusLayout->addWidget(statusLabel3);
    statusLayout->addWidget(statusLabel4);

    mainLayout->addWidget(statusGroup);

    // 连接信号和槽
    QObject::connect(switch1, &ImageSwitch::checkedChanged, [=](bool checked) { statusLabel1->setText(QString("样式1: %1").arg(checked ? "开启" : "关闭")); });
    QObject::connect(switch2, &ImageSwitch::checkedChanged, [=](bool checked) { statusLabel2->setText(QString("样式2: %1").arg(checked ? "开启" : "关闭")); });
    QObject::connect(switch3, &ImageSwitch::checkedChanged, [=](bool checked) { statusLabel3->setText(QString("样式3: %1").arg(checked ? "开启" : "关闭")); });
    QObject::connect(switch4, &ImageSwitch::checkedChanged, [=](bool checked) { statusLabel4->setText(QString("样式4: %1").arg(checked ? "开启" : "关闭")); });

    // 添加一个初始状态为开启的开关示例
    QGroupBox *initialStateGroup = new QGroupBox("初始状态示例");
    QHBoxLayout *initialLayout = new QHBoxLayout(initialStateGroup);

    QLabel *initialLabel = new QLabel("初始开启状态:");
    ImageSwitch *initialSwitch = new ImageSwitch();
    initialSwitch->setChecked(true); // 设置为开启状态

    initialLayout->addWidget(initialLabel);
    initialLayout->addWidget(initialSwitch);
    initialLayout->addStretch();

    mainLayout->addWidget(initialStateGroup);
    mainLayout->addStretch();

    window.show();

    return a.exec();
}

#endif
