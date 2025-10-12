#pragma execution_character_set("utf-8")

#include "StaticTool.h"
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include "qstyle.h"

qlonglong StaticTool::mainThreadID;

std::function<void(QWidget*)> StaticTool::repolish =[](QWidget *w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QByteArray StaticTool::readFileAllData(const QString& filePath)
{
	qDebug() << "file:" << filePath;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return "";
	}

	QByteArray data = file.readAll();

	file.close();

	return data;
}

QString StaticTool::extractVideoName(const QString& str)
{
	// 查找最后一个 '/' 的位置
	int lastSlashPos = str.lastIndexOf('/');

	// 如果找到 '/'，则返回其后的子字符串
	if (lastSlashPos != -1) return str.mid(lastSlashPos + 1);

	// 如果没有找到 '/'，则返回整个字符串
	return str;
}

QString StaticTool::getCurrentDateTimeString(const QString& format)
{
	return QDateTime::currentDateTime().toString(format);
}

QString StaticTool::getCurrentDateString(const QString& format)
{
	return QDateTime::currentDateTime().toString(format);
}

QString StaticTool::getCurrentTimeString(const QString& format)
{
	return QDateTime::currentDateTime().toString(format);
}

QString StaticTool::convertMacSeparator(const QString& macAddress)
{
	QString converted = macAddress;

	// 将每个冒号替换为短横线
	converted.replace(':', '-');
	return converted;
}

QString StaticTool::isMainThreadRun(const QString& funcName, const qlonglong& threadID)
{
	//QString thread = threadID == mainThreadID ? "主线程" : "非主线程";
	//return QString("[" + getCurrentDateTimeString() + "] Function：" + funcName + " 在" + thread + "运行，线程ID为：" + QString::number(threadID));

	QString thread = threadID == mainThreadID ? "main" : "not main";
	return QString("Function: %1 Runs in the %2 thread with thread ID:%3").arg(funcName).arg(thread).arg(QString::number(threadID));
}

quint64 StaticTool::macToInt(const QString& mac)
{
	QString cleaned = mac;
	cleaned.remove(QRegularExpression("[:.-]"));
	bool ok;
	quint64 result = cleaned.toULongLong(&ok, 16);
	return ok ? result : 0;
}

QString StaticTool::intToMac(quint64 mac)
{
	return QString("%1:%2:%3:%4:%5:%6")
		.arg((mac >> 40) & 0xFF, 2, 16, QChar('0'))
		.arg((mac >> 32) & 0xFF, 2, 16, QChar('0'))
		.arg((mac >> 24) & 0xFF, 2, 16, QChar('0'))
		.arg((mac >> 16) & 0xFF, 2, 16, QChar('0'))
		.arg((mac >> 8) & 0xFF, 2, 16, QChar('0'))
		.arg(mac & 0xFF, 2, 16, QChar('0'))
		.toUpper();
}

void StaticTool::showHint(const QWidget* parent, const QString& hintInfo, const uint& showTime)
{
	// 创建提示标签
	QLabel* hintLabel = new QLabel(parent == nullptr ? nullptr : const_cast<QWidget*>(parent));

	hintLabel->setText(hintInfo);
	hintLabel->setAlignment(Qt::AlignCenter);
	hintLabel->setStyleSheet("background-color: #FFF3CD; color: #856404; "
		"border: 1px solid #FFEBA3; border-radius: 5px; "
		"padding: 10px; font-weight: bold; font-size: 16pt;");

	// 不允许换行
	hintLabel->setWordWrap(false);

	// 自动调整大小
	hintLabel->adjustSize();

	// 计算中心位置
	QPoint centerPos;
	if (parent) 
	{
		// 相对于父窗口中心
		centerPos = QPoint((parent->width() - hintLabel->width()) / 2,(parent->height() - hintLabel->height()) / 2);
	}
	else 
	{
		// 相对于屏幕中心
		QScreen* screen = QApplication::primaryScreen();
		QRect screenGeometry = screen->geometry();
		centerPos = QPoint((screenGeometry.width() - hintLabel->width()) / 2, (screenGeometry.height() - hintLabel->height()) / 2);
	}

	hintLabel->move(centerPos);

	// 无父组件时设置为工具提示样式
	if (!parent)
	{
		hintLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
	}

	hintLabel->show();
	hintLabel->raise();

	// 不允许小于500毫秒
	uint mTime = (showTime < 500) ? 500 : showTime;
	QTimer::singleShot(mTime, hintLabel, [hintLabel]()
		{
			if (hintLabel)
			{
				hintLabel->deleteLater();
			}
		});
}
