#include "DemoHead.h"

#if T_PushButtonDemo

#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include "PushButton.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建主窗口
    QWidget window;
    window.setWindowTitle("PushButton Demo");
    window.resize(300, 200);

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // 创建默认按钮
    PushButton *button1 = new PushButton();
    layout->addWidget(button1);

    // 创建自定义按钮
    PushButton *button2 = new PushButton();
    button2->setButtonText("停止", "开始");
    button2->setButtonColors(QColor(41, 128, 185), QColor(52, 152, 219)); // 蓝色系
    button2->setAnimationDuration(500); // 更长的动画时间
    layout->addWidget(button2);

    // 创建另一个自定义按钮
    PushButton *button3 = new PushButton();
    button3->setButtonText("禁用", "启用");
    button3->setButtonColors(QColor(243, 156, 18), QColor(241, 196, 15)); // 橙色系
    button3->setButtonEnableDelay(500); // 更短的禁用时间
    layout->addWidget(button3);

    // 显示窗口
    window.show();

    return a.exec();
}

#endif
