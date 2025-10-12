#pragma once

#include <QObject>
#include <QPushButton>
#include <QPixmap>
#include <QVariantAnimation>

class RefreshButton : public QPushButton
{
public:
    RefreshButton(QWidget* parent = nullptr);

	void setIcon(QString filePath);
    void setIconSize(const QSize& size);

    void startAnimation();

protected:
    void setAngle(double angle);
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;

private:
    int m_angle = 0;
    QPixmap m_pixmap;
    QVariantAnimation* m_animation = nullptr;
	QSize m_iconSize = QSize(32, 32);
};
