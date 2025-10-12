#pragma once

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "MacAddressEdit.h"

class T_MacAddressEditTest : public QWidget
{
    Q_OBJECT

public:
    T_MacAddressEditTest(QWidget *parent = nullptr) : QWidget(parent)
    {
        // 创建界面组件
        QLabel *titleLabel = new QLabel("MAC地址输入示例");
        titleLabel->setAlignment(Qt::AlignCenter);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);

        QLabel *macLabel = new QLabel("MAC地址:");
        macEdit = new MacAddressEdit();

        QLabel *statusLabel = new QLabel("状态:");
        statusValue = new QLabel("等待输入");
        statusValue->setStyleSheet("color: gray;");

        QPushButton *validateButton = new QPushButton("验证MAC地址");
        QPushButton *clearButton = new QPushButton("清空");

        // 布局
        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addWidget(titleLabel);
        mainLayout->addSpacing(20);

        QHBoxLayout *macLayout = new QHBoxLayout();
        macLayout->addWidget(macLabel);
        macLayout->addWidget(macEdit);
        mainLayout->addLayout(macLayout);

        QHBoxLayout *statusLayout = new QHBoxLayout();
        statusLayout->addWidget(statusLabel);
        statusLayout->addWidget(statusValue);
        statusLayout->addStretch();
        mainLayout->addLayout(statusLayout);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(validateButton);
        buttonLayout->addWidget(clearButton);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);
        setWindowTitle("MAC地址输入演示");
        resize(400, 200);

        // 连接信号和槽
        connect(macEdit, &MacAddressEdit::textChanged, this, &T_MacAddressEditTest::updateStatus);
        connect(validateButton, &QPushButton::clicked, this, &T_MacAddressEditTest::validateMac);
        connect(clearButton, &QPushButton::clicked, macEdit, &MacAddressEdit::clear);
    }

private slots:
    void updateStatus()
    {
        if (macEdit->text().isEmpty())
        {
            statusValue->setText("等待输入");
            statusValue->setStyleSheet("color: gray;");
        }
        else if (macEdit->isValid())
        {
            statusValue->setText("有效");
            statusValue->setStyleSheet("color: green;");
        }
        else
        {
            statusValue->setText("无效");
            statusValue->setStyleSheet("color: red;");
        }
    }

    void validateMac()
    {
        if (macEdit->isValid())
        {
            QMessageBox::information(this, "验证成功", QString("MAC地址有效: %1").arg(macEdit->macAddress()));
        }
        else
        {
            QMessageBox::warning(this, "验证失败", "请输入有效的MAC地址(12个十六进制字符)");
        }
    }

private:
    MacAddressEdit *macEdit;
    QLabel *statusValue;
};
