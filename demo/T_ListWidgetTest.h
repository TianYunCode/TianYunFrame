#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QColorDialog>
#include <QMessageBox>
#include "ListWidget.h"

class T_ListWidgetTest : public QMainWindow
{
    Q_OBJECT

public:
    T_ListWidgetTest(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        // 创建自定义列表控件
        listWidget = new ListWidget(this, 0);
        listWidget->setItemHeight(35);
        listWidget->setBackgroundColor(QColor(255, 255, 255));
        listWidget->setGridLineColor(QColor(18, 150, 219));
        listWidget->setSelectionColor(QColor(18, 150, 219));
        listWidget->setSelectionBackgroundColor(QColor(182, 224, 247));

        // 添加一些示例项目
        for (int i = 1; i <= 20; ++i)
        {
            QListWidgetItem *item = new QListWidgetItem(QIcon::fromTheme("document"), QString("项目 %1").arg(i));
            item->setSizeHint(QSize(0, 35));
            listWidget->addItem(item);
        }

        // 连接项目选择变化信号
        connect(listWidget, &QListWidget::itemClicked, this, &T_ListWidgetTest::onItemClicked);
        connect(listWidget, &QListWidget::itemDoubleClicked, this, &T_ListWidgetTest::onItemDoubleClicked);

        // 创建控制面板
        QWidget *controlPanel = new QWidget(this);
        QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

        // 项目高度控制
        QHBoxLayout *heightLayout = new QHBoxLayout();
        QLabel *heightLabel = new QLabel("项目高度:");
        QSpinBox *heightSpinBox = new QSpinBox();
        heightSpinBox->setRange(20, 100);
        heightSpinBox->setValue(35);
        heightLayout->addWidget(heightLabel);
        heightLayout->addWidget(heightSpinBox);
        controlLayout->addLayout(heightLayout);

        // 颜色控制按钮
        QPushButton *textColorBtn = new QPushButton("文字颜色");
        QPushButton *bgColorBtn = new QPushButton("背景颜色");
        QPushButton *gridColorBtn = new QPushButton("网格线颜色");
        QPushButton *selectionColorBtn = new QPushButton("选中项颜色");
        QPushButton *selectionBgColorBtn = new QPushButton("选中项背景色");

        controlLayout->addWidget(textColorBtn);
        controlLayout->addWidget(bgColorBtn);
        controlLayout->addWidget(gridColorBtn);
        controlLayout->addWidget(selectionColorBtn);
        controlLayout->addWidget(selectionBgColorBtn);

        // 间距控制
        QHBoxLayout *spacingLayout = new QHBoxLayout();
        QLabel *spacingLabel = new QLabel("项目间距:");
        QSpinBox *spacingSpinBox = new QSpinBox();
        spacingSpinBox->setRange(0, 20);
        spacingSpinBox->setValue(0);
        spacingLayout->addWidget(spacingLabel);
        spacingLayout->addWidget(spacingSpinBox);
        controlLayout->addLayout(spacingLayout);

        // 添加项目按钮
        QPushButton *addItemBtn = new QPushButton("添加项目");
        QPushButton *removeItemBtn = new QPushButton("删除选中项目");
        controlLayout->addWidget(addItemBtn);
        controlLayout->addWidget(removeItemBtn);

        // 状态标签
        statusLabel = new QLabel("点击列表中的项目...");
        controlLayout->addWidget(statusLabel);

        // 主布局
        QWidget *centralWidget = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(listWidget, 3);
        mainLayout->addWidget(controlPanel, 1);

        setCentralWidget(centralWidget);
        setWindowTitle("自定义 ListWidget Demo");
        resize(800, 500);

        // 连接信号和槽
        connect(heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), listWidget, &ListWidget::setItemHeight);

        connect(spacingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value){ listWidget->setSpacing(value); });

        connect(textColorBtn, &QPushButton::clicked, [this]()
                {
                    QColor color = QColorDialog::getColor(listWidget->color(), this, "选择文字颜色");
                    if (color.isValid())
                    {
                        listWidget->setColor(color);
                    }
                });

        connect(bgColorBtn, &QPushButton::clicked, [this]()
                {
                    QColor color = QColorDialog::getColor(listWidget->backgroundColor(), this, "选择背景颜色");
                    if (color.isValid())
                    {
                        listWidget->setBackgroundColor(color);
                    }
                });

        connect(gridColorBtn, &QPushButton::clicked, [this]()
                {
                    QColor color = QColorDialog::getColor(listWidget->gridLineColor(), this, "选择网格线颜色");
                    if (color.isValid())
                    {
                        listWidget->setGridLineColor(color);
                    }
                });

        connect(selectionColorBtn, &QPushButton::clicked, [this]()
                {
                    QColor color = QColorDialog::getColor(listWidget->selectionColor(), this, "选择选中项颜色");
                    if (color.isValid())
                    {
                        listWidget->setSelectionColor(color);
                    }
                });

        connect(selectionBgColorBtn, &QPushButton::clicked, [this]()
                {
                    QColor color = QColorDialog::getColor(listWidget->selectionBackgroundColor(), this, "选择选中项背景色");
                    if (color.isValid())
                    {
                        listWidget->setSelectionBackgroundColor(color);
                    }
                });

        connect(addItemBtn, &QPushButton::clicked, this, &T_ListWidgetTest::onAddItem);
        connect(removeItemBtn, &QPushButton::clicked, this, &T_ListWidgetTest::onRemoveItem);
    }

private slots:
    void onItemClicked(QListWidgetItem *item)
    {
        statusLabel->setText(QString("单击: %1").arg(item->text()));
    }

    void onItemDoubleClicked(QListWidgetItem *item)
    {
        statusLabel->setText(QString("双击: %1").arg(item->text()));
        QMessageBox::information(this, "项目双击", QString("您双击了: %1").arg(item->text()));
    }

    void onAddItem()
    {
        int count = listWidget->count() + 1;
        QListWidgetItem *item = new QListWidgetItem(QIcon::fromTheme("document"), QString("新项目 %1").arg(count));
        listWidget->addItem(item);
        statusLabel->setText(QString("已添加新项目 %1").arg(count));
    }

    void onRemoveItem()
    {
        QListWidgetItem *item = listWidget->currentItem();
        if (item)
        {
            int row = listWidget->row(item);
            delete listWidget->takeItem(row);
            statusLabel->setText(QString("已删除项目 %1").arg(row + 1));
        }
        else
        {
            statusLabel->setText("没有选中的项目可删除");
        }
    }

private:
    ListWidget *listWidget;
    QLabel *statusLabel;
};
