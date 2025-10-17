#include "BorderlessStretchWindow.h"
#include "BorderlessStretchWindow.h"
#include "BorderlessStretchWindow.h"

BorderlessStretchWindow::BorderlessStretchWindow(QWidget* parent) :QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint); //无边框
	setMouseTracking(true); //鼠标移动触发mouseMoveEvent
}

void BorderlessStretchWindow::maximizeIsMove(bool move)
{
	maximizeMove = move;
}

void BorderlessStretchWindow::maximizeIsResize(bool resize)
{
	maximizeResize = resize;
}

void BorderlessStretchWindow::setTitleBar(QWidget* titleBar)
{
	this->titleBar = titleBar;
}

//准备拖动
void BorderlessStretchWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		// 根据点击时的实际位置重新计算区域
		ResizeRegion pressRegion = getResizeRegion(event->pos());

		if (pressRegion != NoEdge)
		{
			if(isMaximized() && !maximizeResize)
			{
				return; // 最大化时不允许缩放
			}

			// 开始缩放
			isResizing = true;
			currentRegion = pressRegion;
			dragStartGlobalPos = event->globalPosition().toPoint();
			originalGeometry = geometry();
		}
		else
		{
			if (isMaximized() && !maximizeMove)
			{
				return; // 最大化时不允许移动
			}

			// 开始移动
			isMoving = true;
			dragStartGlobalPos = event->globalPosition().toPoint();
			dragStartWindowPos = geometry().topLeft();
		}
	}

	QWidget::mousePressEvent(event);
}

//结束拖动
void BorderlessStretchWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (isResizing)
		{
			isResizing = false;

			// 重要修复：释放后立即根据当前鼠标位置更新光标
			QPoint localPos = mapFromGlobal(event->globalPosition().toPoint());
			ResizeRegion newRegion = getResizeRegion(localPos);
			updateCursor(newRegion);
		}
		else if (isMoving)
		{
			isMoving = false;

			// 移动结束后也需要更新光标状态
			QPoint localPos = mapFromGlobal(event->globalPosition().toPoint());
			ResizeRegion newRegion = getResizeRegion(localPos);
			updateCursor(newRegion);
		}
	}

	QWidget::mouseReleaseEvent(event);
}

//设置光标或拖动缩放或移动窗口
void BorderlessStretchWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (isResizing)
	{
		// 缩放处理
		QPoint delta = event->globalPosition().toPoint() - dragStartGlobalPos;
		QRect newGeom = originalGeometry;

		switch (currentRegion)
		{
		case Left:
			newGeom.setLeft(originalGeometry.left() + delta.x());
			break;

		case Right:
			newGeom.setRight(originalGeometry.right() + delta.x());
			break;

		case Top:
			newGeom.setTop(originalGeometry.top() + delta.y());
			break;

		case Bottom:
			newGeom.setBottom(originalGeometry.bottom() + delta.y());
			break;

		case TopLeft:
			newGeom.setTopLeft(originalGeometry.topLeft() + delta);
			break;

		case TopRight:
			newGeom.setTopRight(originalGeometry.topRight() + delta);
			break;

		case BottomLeft:
			newGeom.setBottomLeft(originalGeometry.bottomLeft() + delta);
			break;

		case BottomRight:
			newGeom.setBottomRight(originalGeometry.bottomRight() + delta);
			break;

		default:
			break;
		}

		// 确保新的尺寸不小于最小尺寸
		if (newGeom.width() < minimumWidth())
		{
			if (currentRegion == Left || currentRegion == TopLeft || currentRegion == BottomLeft)
				newGeom.setLeft(newGeom.right() - minimumWidth());
			else if (currentRegion == Right || currentRegion == TopRight || currentRegion == BottomRight)
				newGeom.setRight(newGeom.left() + minimumWidth());
		}

		if (newGeom.height() < minimumHeight())
		{
			if (currentRegion == Top || currentRegion == TopLeft || currentRegion == TopRight)
				newGeom.setTop(newGeom.bottom() - minimumHeight());
			else if (currentRegion == Bottom || currentRegion == BottomLeft || currentRegion == BottomRight)
				newGeom.setBottom(newGeom.top() + minimumHeight());
		}

		setGeometry(newGeom);
	}
	else if (isMoving)
	{
		// 移动处理
		QPoint delta = event->globalPosition().toPoint() - dragStartGlobalPos;
		QPoint newPos = dragStartWindowPos + delta;
		move(newPos);
	}
	else
	{
		//设置鼠标光标形状
		ResizeRegion region = getResizeRegion(event->pos());
		updateCursor(region);
	}

	QWidget::mouseMoveEvent(event);
}

void BorderlessStretchWindow::mouseDoubleClickEvent(QMouseEvent* ev)
{
	// 如果双击的是标题栏，窗口最大化，其他不处理
	if (titleBar && titleBar->geometry().contains(ev->pos()))
	{
		if (isMaximized())
			showNormal();
		else
			showMaximized();
	}
	QWidget::mouseDoubleClickEvent(ev);
}

//鼠标离开窗口，取消高亮
void BorderlessStretchWindow::leaveEvent(QEvent* event)
{
	if (!isResizing && !isMoving)
	{
		currentRegion = NoEdge;
		unsetCursor();
	}
	QWidget::leaveEvent(event);
}

//获取边缘区域
BorderlessStretchWindow::ResizeRegion BorderlessStretchWindow::getResizeRegion(const QPoint& pos)
{
	bool onLeft = pos.x() <= EDGE_MARGIN;
	bool onRight = pos.x() >= width() - EDGE_MARGIN;
	bool onTop = pos.y() <= EDGE_MARGIN;
	bool onBottom = pos.y() >= height() - EDGE_MARGIN;

	if (onTop && onLeft) return TopLeft;
	if (onTop && onRight) return TopRight;
	if (onBottom && onLeft) return BottomLeft;
	if (onBottom && onRight) return BottomRight;
	if (onTop) return Top;
	if (onBottom) return Bottom;
	if (onLeft) return Left;
	if (onRight) return Right;

	return NoEdge;
}

// 更新光标状态的辅助函数
void BorderlessStretchWindow::updateCursor(ResizeRegion region)
{
	currentRegion = region;

	switch (region)
	{
	case Left:
	case Right:
		setCursor(Qt::SizeHorCursor);
		break;

	case Top:
	case Bottom:
		setCursor(Qt::SizeVerCursor);
		break;

	case TopLeft:
	case BottomRight:
		setCursor(Qt::SizeFDiagCursor);
		break;

	case TopRight:
	case BottomLeft:
		setCursor(Qt::SizeBDiagCursor);
		break;

	default:
		unsetCursor();
		break;
	}
}