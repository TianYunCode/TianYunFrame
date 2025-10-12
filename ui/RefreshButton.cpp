#include "RefreshButton.h"
#include <QPainter>

RefreshButton::RefreshButton(QWidget* parent) : QPushButton(parent)
{
	m_animation = new QVariantAnimation(this);
	m_animation->setStartValue(0.0);
	m_animation->setEndValue(360.0);
	m_animation->setDuration(500);

	connect(m_animation, &QVariantAnimation::valueChanged, this, [=]()
		{
			update();
			this->setAngle(m_animation->currentValue().toDouble());
		});
}

void RefreshButton::setIcon(QString filePath)
{
	m_pixmap = QPixmap(filePath);
	m_pixmap = m_pixmap.scaled(m_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	update();
}

void RefreshButton::setIconSize(const QSize& size)
{
	m_iconSize = size;
	this->setFixedSize(m_iconSize + QSize(10, 10));
}

void RefreshButton::startAnimation()
{
	m_animation->start();
}

void RefreshButton::setAngle(double angle)
{
	m_angle = angle;
}

void RefreshButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	painter.save();

	//偏移中心点到pixmap中心点
	painter.translate(m_pixmap.width() / 2, m_pixmap.height() / 2);

	//旋转角度
	painter.rotate(m_angle);

	//恢复中心点
	painter.translate(-m_pixmap.width() / 2, -m_pixmap.height() / 2);

	//绘制图片
	painter.drawPixmap(0, 0, m_pixmap);
	painter.restore();
}

void RefreshButton::mousePressEvent(QMouseEvent* e)
{
	Q_UNUSED(e);
	m_animation->start();
}