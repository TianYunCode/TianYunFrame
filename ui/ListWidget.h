#pragma once

#include <QListWidget>
#include <QPaintEvent>
#include <QRegion>
#include <QMouseEvent>

class ListWidget : public QListWidget
{
    Q_OBJECT

    Q_PROPERTY(int itemHeight READ itemHeight WRITE setItemHeight)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor gridLineColor READ gridLineColor WRITE setGridLineColor)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor)
    Q_PROPERTY(QColor selectionBackgroundColor READ selectionBackgroundColor WRITE setSelectionBackgroundColor)
    // 悬停颜色属性
    Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setHoverColor)
    Q_PROPERTY(QColor hoverBackgroundColor READ hoverBackgroundColor WRITE setHoverBackgroundColor)
    Q_PROPERTY(QColor hoverGridLineColor READ hoverGridLineColor WRITE setHoverGridLineColor)

public:
    ListWidget(QWidget *parent = 0, int paintType = 0);

    // 设置item的高度
    int itemHeight();
    void setItemHeight(int height);

    // 设置颜色
    const QColor & color() const;
    void setColor(const QColor & color);

    // 设置背景颜色
    const QColor & backgroundColor();
    void setBackgroundColor(const QColor & color);

    // 设置网格线颜色
    const QColor & gridLineColor() const;
    void setGridLineColor(const QColor & color);

    // 设置选择颜色
    const QColor & selectionColor() const;
    void setSelectionColor(const QColor & color);

    // 设置选择背景颜色
    const QColor & selectionBackgroundColor() const;
    void setSelectionBackgroundColor(const QColor & color);

    // 设置悬停颜色
    const QColor & hoverColor() const;
    void setHoverColor(const QColor & color);

    // 设置悬停背景颜色
    const QColor & hoverBackgroundColor() const;
    void setHoverBackgroundColor(const QColor & color);

    // 设置悬停网格线颜色
    const QColor & hoverGridLineColor() const;
    void setHoverGridLineColor(const QColor & color);

    // 获取选中项
    QListWidgetItem* getSelectedItem();

    // 取消选中
    void clearSelection();

signals:
    void signalItemSelectionChanged();

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

    void onItemSelectionChanged();

private:
    int m_paintType;
    int m_itemHeight;
    int m_hoverIndex; // 跟踪当前悬停的项索引

    QColor m_color;
    QColor m_backgroundColor;
    QColor m_gridLineColor;
    QColor m_selectionColor;
    QColor m_selectionBackgroundColor;

    // 悬停颜色
    QColor m_hoverColor;
    QColor m_hoverBackgroundColor;
    QColor m_hoverGridLineColor;
};
