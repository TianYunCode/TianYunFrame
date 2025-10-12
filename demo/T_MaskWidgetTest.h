#pragma once

#include <QWidget>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include "MaskWidget.h"

class T_MaskWidgetTest : public QWidget
{
    Q_OBJECT

public:
    explicit T_MaskWidgetTest(QWidget *parent = 0) : QWidget(parent)
    {
        this->resize(600, 400);

        btn = new QPushButton(this);
        btn->setText("弹出窗体");

        connect(btn, &QPushButton::clicked, this, &T_MaskWidgetTest::slotBtnClicked);

        QTimer::singleShot(1000, this, SLOT(initForm()));
    }

    ~T_MaskWidgetTest()
    {

    }

private slots:
    void initForm()
    {
        MaskWidget::Instance()->setMainWidget(this->topLevelWidget());
        MaskWidget::Instance()->setDialogNames(QStringList() << "MaskTest");
    }

    void slotBtnClicked()
    {
        QDialog d;
        d.setObjectName("MaskTest");
        d.setWindowTitle("遮罩层弹出窗体");
        d.resize(400, 300);
        d.exec();
    }

private:
    QPushButton *btn;
};
