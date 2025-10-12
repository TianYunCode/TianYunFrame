#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QStyle>
#include "BorderlessStretchWindow.h"

class T_BorderlessStretchWindowTest : public BorderlessStretchWindow
{
    Q_OBJECT

public:
    explicit T_BorderlessStretchWindowTest(QWidget* parent = nullptr) : BorderlessStretchWindow(parent)
    {
        // 设置窗口最小大小
        setMinimumSize(400, 300);

        // 创建布局和内容
        QVBoxLayout* layout = new QVBoxLayout(this);

        // 添加标题
        QLabel* titleLabel = new QLabel("无边框可拉伸窗口 Demo");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        layout->addWidget(titleLabel);

        // 添加一些示例内容
        QLabel* contentLabel = new QLabel(
            "这是一个无边框窗口，具有以下特性：\n\n"
            "• 可以通过边缘拖拽调整大小\n"
            "• 鼠标靠近边缘时会显示相应的调整光标\n"
            "• 完全自定义的窗口样式\n"
            "• 支持最小化、最大化和关闭功能\n\n"
            "尝试将鼠标移动到窗口边缘来体验拉伸功能！"
            );
        contentLabel->setAlignment(Qt::AlignLeft);
        contentLabel->setWordWrap(true);
        contentLabel->setStyleSheet("margin: 20px;");
        layout->addWidget(contentLabel);

        // 添加按钮
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* minimizeButton = new QPushButton("最小化");
        QPushButton* maximizeButton = new QPushButton("最大化");
        QPushButton* closeButton = new QPushButton("关闭");

        connect(minimizeButton, &QPushButton::clicked, this, &T_BorderlessStretchWindowTest::showMinimized);
        connect(maximizeButton, &QPushButton::clicked, this, [this, maximizeButton]() {
            if (isMaximized()) {
                showNormal();
                maximizeButton->setText("最大化");
            } else {
                showMaximized();
                maximizeButton->setText("还原");
            }
        });
        connect(closeButton, &QPushButton::clicked, this, &T_BorderlessStretchWindowTest::close);

        buttonLayout->addWidget(minimizeButton);
        buttonLayout->addWidget(maximizeButton);
        buttonLayout->addWidget(closeButton);
        layout->addLayout(buttonLayout);

        // 设置窗口样式
        setStyleSheet(
            "DemoWindow {"
            "   background-color: #f0f0f0;"
            "   border: 2px solid #cccccc;"
            "   border-radius: 5px;"
            "}"
            "QPushButton {"
            "   background-color: #e0e0e0;"
            "   border: 1px solid #cccccc;"
            "   border-radius: 3px;"
            "   padding: 5px 10px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #d0d0d0;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #c0c0c0;"
            "}"
            );
    }
};
