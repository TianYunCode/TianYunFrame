#pragma once

#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <qpoint.h>

// 无边框可拉伸窗口基类
class BorderlessStretchWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BorderlessStretchWindow(QWidget* parent = nullptr);

	void maximizeIsMove(bool move); // 最大化时是否允许移动

	void maximizeIsResize(bool resize); // 最大化时是否允许拉伸

    void setTitleBar(QWidget* titleBar);   // 用来双击最大化窗口

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event)override;
    void mouseMoveEvent(QMouseEvent* event)override;
    void mouseDoubleClickEvent(QMouseEvent* ev)override;
    void leaveEvent(QEvent* event)override;

private:
    // 缩放区域枚举
    enum ResizeRegion
    {
        NoEdge = 0,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    const int EDGE_MARGIN = 8; //边缘检测范围

    ResizeRegion getResizeRegion(const QPoint& pos);
    void updateCursor(ResizeRegion region);

    bool isResizing = false; //是否正在缩放
    bool isMoving = false;   //是否正在移动

    ResizeRegion currentRegion = NoEdge;
    QPoint dragStartGlobalPos; //鼠标拖动起点
    QPoint dragStartWindowPos; //窗口起始位置（用于移动）
    QRect originalGeometry;  //拖动时窗口原始位置
	bool maximizeMove = false; // 最大化时是否允许移动
	bool maximizeResize = false; // 最大化时是否允许拉伸

    QWidget* titleBar{nullptr};
};