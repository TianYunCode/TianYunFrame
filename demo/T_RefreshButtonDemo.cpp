#include "DemoHead.h"

#if T_RefreshButtonDemo

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "RefreshButton.h"
#include <QTimer>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	// 创建主窗口
	QWidget window;
	window.setWindowTitle("Refresh Button Demo");
	window.setFixedSize(300, 200);

	// 创建布局
	QVBoxLayout* layout = new QVBoxLayout(&window);

	// 创建标签显示状态
	QLabel* statusLabel = new QLabel("点击刷新按钮试试看！");
	statusLabel->setAlignment(Qt::AlignCenter);

	// 创建刷新按钮
	RefreshButton* refreshButton = new RefreshButton(&window);
	refreshButton->setIconSize(QSize(32, 32));
	refreshButton->setIcon(":/icon/refresh_icon.svg");
	refreshButton->setFixedSize(QSize(32, 32));

	// 添加到布局
	layout->addWidget(statusLabel);
	layout->addWidget(refreshButton, 0, Qt::AlignCenter);

	// 显示窗口
	window.show();

	return app.exec();
}

#endif