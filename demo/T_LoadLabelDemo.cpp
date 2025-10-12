#include "DemoHead.h"

#if T_LoadLabelDemo

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include "LoadLabel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget window;
    window.setWindowTitle("LoadLabel Demo");
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);

    // 创建加载标签
    LoadLabel *loadLabel = new LoadLabel();
    loadLabel->setImageCount(":/loading/loading", 8);
    loadLabel->setSize(QSize(40, 40));
    loadLabel->setInterval(800);

    // 控制区域
    QGroupBox *controlGroup = new QGroupBox("控制选项");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);

    // 类型选择
    QHBoxLayout *typeLayout = new QHBoxLayout();
    QLabel *typeLabel = new QLabel("加载类型:");
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("PNG序列", LoadLabel::PNG);
    typeCombo->addItem("GIF动画", LoadLabel::GIF);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeCombo);
    controlLayout->addLayout(typeLayout);

    // PNG选项
    QGroupBox *pngGroup = new QGroupBox("PNG选项");
    QFormLayout *pngLayout = new QFormLayout(pngGroup);

    QLineEdit *pngPathEdit = new QLineEdit(":/loading/loading");
    QSpinBox *pngCountSpin = new QSpinBox();
    pngCountSpin->setRange(1, 100);
    pngCountSpin->setValue(8);
    QSpinBox *pngIntervalSpin = new QSpinBox();
    pngIntervalSpin->setRange(10, 5000);
    pngIntervalSpin->setValue(800);
    pngIntervalSpin->setSuffix(" ms");

    pngLayout->addRow("图片路径:", pngPathEdit);
    pngLayout->addRow("图片数量:", pngCountSpin);
    pngLayout->addRow("动画周期:", pngIntervalSpin);
    controlLayout->addWidget(pngGroup);

    // GIF选项
    QGroupBox *gifGroup = new QGroupBox("GIF选项");
    QFormLayout *gifLayout = new QFormLayout(gifGroup);

    QLineEdit *gifPathEdit = new QLineEdit(":/loading/icon-loading.gif");
    gifLayout->addRow("GIF路径:", gifPathEdit);
    controlLayout->addWidget(gifGroup);
    gifGroup->setVisible(false); // 默认隐藏GIF选项

    // 大小控制
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    QLabel *sizeLabel = new QLabel("显示大小:");
    QSpinBox *widthSpin = new QSpinBox();
    widthSpin->setRange(10, 200);
    widthSpin->setValue(40);
    QSpinBox *heightSpin = new QSpinBox();
    heightSpin->setRange(10, 200);
    heightSpin->setValue(40);
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(widthSpin);
    sizeLayout->addWidget(new QLabel("x"));
    sizeLayout->addWidget(heightSpin);
    controlLayout->addLayout(sizeLayout);

    // 按钮控制
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *startButton = new QPushButton("开始动画");
    QPushButton *stopButton = new QPushButton("停止动画");
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(stopButton);
    controlLayout->addLayout(buttonLayout);

    mainLayout->addWidget(loadLabel, 0, Qt::AlignCenter);
    mainLayout->addWidget(controlGroup);

    // 连接信号槽
    QObject::connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
                     {
                         LoadLabel::LoadType type = static_cast<LoadLabel::LoadType>(typeCombo->currentData().toInt());
                         loadLabel->setLoadType(type);

                         // 显示/隐藏相关选项
                         pngGroup->setVisible(type == LoadLabel::PNG);
                         gifGroup->setVisible(type == LoadLabel::GIF);

                         // 停止当前动画
                         loadLabel->stopAnimation();
                     });

    QObject::connect(pngPathEdit, &QLineEdit::textChanged, [=](const QString &text) { loadLabel->setImageCount(text, pngCountSpin->value()); });

    QObject::connect(pngCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value)
                     {
                         loadLabel->setImageCount(pngPathEdit->text(), value);
                         loadLabel->setInterval(pngIntervalSpin->value());
                     });

    QObject::connect(pngIntervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) { loadLabel->setInterval(value); });

    QObject::connect(gifPathEdit, &QLineEdit::textChanged, [=](const QString &text) { loadLabel->setGifPath(text); });

    QObject::connect(widthSpin, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value)
                     {
                         QSize newSize(value, heightSpin->value());
                         loadLabel->setSize(newSize);
                     });

    QObject::connect(heightSpin, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value)
                     {
                         QSize newSize(widthSpin->value(), value);
                         loadLabel->setSize(newSize);
                     });

    QObject::connect(startButton, &QPushButton::clicked, [=]() { loadLabel->startAnimation(); });

    QObject::connect(stopButton, &QPushButton::clicked, [=]() { loadLabel->stopAnimation(); });

    window.resize(400, 500);
    window.show();

    return a.exec();
}

#endif
