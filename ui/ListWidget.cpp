#pragma execution_character_set("utf-8")

#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "ListWidget.h"

const int C_TEXT_LEFT_MARGIN = 50;  //文字开始位置
const int C_INDICATOR_WIDTH = 5;    //指示条宽度

ListWidget::ListWidget(QWidget *parent, int paintType) : QListWidget(parent), m_paintType(paintType)
{
    this->setFrameShape(NoFrame);
    this->setSpacing(0);

    connect(this, &ListWidget::itemSelectionChanged, this, &ListWidget::onItemSelectionChanged);

    m_itemHeight = 35;
    m_hoverIndex = -1; // 初始化为无悬停

    m_color = QColor(198, 198, 198);
    m_backgroundColor = QColor(76, 83, 89);
    m_selectionColor = QColor(99, 176, 72);
    m_selectionBackgroundColor =  QColor(76, 83, 89);
    m_gridLineColor = QColor(87, 95, 103);

    // 初始化悬停颜色
    m_hoverColor = QColor(220, 220, 220);
    m_hoverBackgroundColor = QColor(86, 93, 99);
    m_hoverGridLineColor = QColor(97, 105, 113);

    this->setIconSize(QSize(16, 16));
    this->setMouseTracking(true); // 启用鼠标跟踪
}

int ListWidget::itemHeight()
{
    return m_itemHeight;
}

void ListWidget::setItemHeight(int height)
{
    m_itemHeight = height;

    QListWidgetItem * pItem = NULL;
    QSize size;

    for (int i = 0; i < this->count(); i++)
    {
        pItem = this->item(i);

        if (i == 0)
        {
            size = pItem->sizeHint();
            size.setHeight(m_itemHeight);
        }

        pItem->setSizeHint(size);
    }
}

const QColor &ListWidget::color() const
{
    return m_color;
}

void ListWidget::setColor(const QColor &color)
{
    if (color != m_color)
    {
        m_color = color;
        this->update();
    }
}

const QColor &ListWidget::backgroundColor()
{
    return m_backgroundColor;
}

void ListWidget::setBackgroundColor(const QColor &color)
{
    if (color != m_backgroundColor)
    {
        m_backgroundColor = color;
        this->update();
    }
}

const QColor &ListWidget::gridLineColor() const
{
    return m_gridLineColor;
}

void ListWidget::setGridLineColor(const QColor &color)
{
    if (color != m_gridLineColor)
    {
        m_gridLineColor = color;
        this->update();
    }
}

const QColor &ListWidget::selectionColor() const
{
    return m_selectionColor;
}

void ListWidget::setSelectionColor(const QColor &color)
{
    if (color != m_selectionColor)
    {
        m_selectionColor = color;
        this->update();
    }
}

const QColor &ListWidget::selectionBackgroundColor() const
{
    return m_selectionBackgroundColor;
}

void ListWidget::setSelectionBackgroundColor(const QColor &color)
{
    if (color != m_selectionBackgroundColor)
    {
        m_selectionBackgroundColor = color;
        this->update();
    }
}

const QColor &ListWidget::hoverColor() const
{
    return m_hoverColor;
}

void ListWidget::setHoverColor(const QColor &color)
{
    if (color != m_hoverColor)
    {
        m_hoverColor = color;
        this->update();
    }
}

const QColor &ListWidget::hoverBackgroundColor() const
{
    return m_hoverBackgroundColor;
}

void ListWidget::setHoverBackgroundColor(const QColor &color)
{
    if (color != m_hoverBackgroundColor)
    {
        m_hoverBackgroundColor = color;
        this->update();
    }
}

const QColor &ListWidget::hoverGridLineColor() const
{
    return m_hoverGridLineColor;
}

void ListWidget::setHoverGridLineColor(const QColor &color)
{
    if (color != m_hoverGridLineColor)
    {
        m_hoverGridLineColor = color;
        this->update();
    }
}

QListWidgetItem *ListWidget::getSelectedItem()
{
    QList<QListWidgetItem *> items = this->selectedItems();

    if (items.size() > 0)
    {
        return items.at(0);
    }

    return NULL;
}

void ListWidget::clearSelection()
{
    QListWidget::clearSelection();
    this->viewport()->repaint();
}

void ListWidget::showEvent(QShowEvent *event)
{
    if (this->count() > 0)
    {
        QListWidgetItem * pItem = this->item(0);

        if (pItem->sizeHint().height() != m_itemHeight)
        {
            setItemHeight(m_itemHeight);
        }
    }

    QListWidget::showEvent(event);
}

void ListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent(event);

    // 获取当前鼠标位置对应的项索引
    QPoint pos = event->pos();
    int index = this->indexAt(pos).row();

    // 如果悬停的项发生变化，更新并重绘
    if (index != m_hoverIndex)
    {
        m_hoverIndex = index;
        this->viewport()->repaint();
    }
}

void ListWidget::leaveEvent(QEvent *event)
{
    QListWidget::leaveEvent(event);

    // 鼠标离开时清除悬停状态
    if (m_hoverIndex != -1)
    {
        m_hoverIndex = -1;
        this->viewport()->repaint();
    }
}

void ListWidget::onItemSelectionChanged()
{
    //当spacing大于0时，如果不repaint，spacing区域不会更新，导致显示不正常
    if (this->spacing() > 0)
    {
        this->viewport()->repaint();
    }

    emit signalItemSelectionChanged();
}

void ListWidget::paintEvent(QPaintEvent * event)
{
    if (m_paintType != 0)
    {
        QListWidget::paintEvent(event);
        return;
    }

    QPainter painter(this->viewport());
    QRect paintRect = this->rect();
    QListWidgetItem * pItem = NULL;
    QSize size(paintRect.width(), m_itemHeight);
    QPoint point;

    //绘制背景色
    painter.fillRect(paintRect.x(), paintRect.y(), paintRect.width(), paintRect.height(), m_backgroundColor);

    //设置图标区域
    int iconX = (C_TEXT_LEFT_MARGIN - this->iconSize().width() - C_INDICATOR_WIDTH) / 2 + C_INDICATOR_WIDTH;
    int iconY = 0;
    int iSpacing = this->spacing();
    int y = iSpacing;

    point.setX(C_TEXT_LEFT_MARGIN);

    //确定要显示的item范围
    int iFrom = this->indexAt(QPoint(paintRect.x() + iSpacing, paintRect.y() + iSpacing)).row();
    int iTo = this->indexAt(QPoint(paintRect.x() + iSpacing, paintRect.height() - iSpacing)).row();

    // 检查是否有可见项，没有则直接返回
    if (iFrom == -1)
    {
        return;  // 列表为空或没有可见项，跳过绘制
    }

    iTo = iTo == -1 ? this->count()-1 : iTo;
    iTo = iTo < iFrom ? iFrom : iTo;
    QFont font = this->font();

    for (int i = iFrom; i <= iTo ; i++)
    {
        pItem = this->item(i);

        // 判断是否悬停
        bool isHovered = (i == m_hoverIndex);

        //画背景
        if (pItem->isSelected())
        {
            painter.fillRect(0, y, size.width(), size.height(), m_selectionBackgroundColor);
            painter.fillRect(0, y, C_INDICATOR_WIDTH, size.height(), m_selectionColor);
            painter.setPen(m_selectionColor);
            font.setBold(true);
        }
        else if (isHovered)
        {
            // 使用悬停颜色
            painter.fillRect(0, y, size.width(), size.height(), m_hoverBackgroundColor);
            painter.fillRect(0, y, C_INDICATOR_WIDTH, size.height(), m_hoverColor);
            painter.setPen(m_hoverColor);
            font.setBold(false);
        }
        else
        {
            painter.fillRect(0, y, size.width(), size.height(), m_backgroundColor);
            painter.setPen(m_color);
            font.setBold(false);
        }

        //画图标
        QIcon icon = pItem->icon();
        if (!icon.isNull())
        {
            iconY = (m_itemHeight - this->iconSize().height()) / 2 + y;
            QRect iconRect(QPoint(iconX, iconY), this->iconSize());
            if (pItem->isSelected())
            {
                icon.paint(&painter, iconRect, Qt::AlignCenter, QIcon::Selected, QIcon::Off);
            }
            else if (isHovered)
            {
                // 使用悬停状态绘制图标
                icon.paint(&painter, iconRect, Qt::AlignCenter, QIcon::Active, QIcon::Off);
            }
            else
            {
                icon.paint(&painter, iconRect);
            }
        }

        //写字
        point.setY(y);
        painter.setFont(font);
        QRect rectItem(point, size);
        painter.drawText(rectItem, Qt::AlignLeft | Qt::AlignVCenter, pItem->text(), NULL);

        //画分隔线
        if (i > 0)
        {
            if (isHovered)
            {
                painter.setPen(m_hoverGridLineColor);
            }
            else
            {
                painter.setPen(m_gridLineColor);
            }
            painter.drawLine(0, y - this->spacing(), paintRect.width(), y - this->spacing());
        }

        y += size.height() + this->spacing() * 2;
    }

    painter.end();
}
