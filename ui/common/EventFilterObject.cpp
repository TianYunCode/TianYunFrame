#pragma execution_character_set("utf-8")

#include "EventFilterObject.h"
#include <QDebug>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

EventFilterObject::EventFilterObject(QObject* parent) : QObject(parent)
{
}

EventFilterObject::~EventFilterObject()
{
	//qInfo() << __FUNCTION__;
}

bool EventFilterObject::eventFilter(QObject *watched,QEvent* ev)
{
	auto w = dynamic_cast<QWidget*>(watched);
    if (!w)
    {
		return false;
	}

	if (ev->type() == QEvent::MouseButtonPress)
	{
		auto mev = dynamic_cast<QMouseEvent*>(ev);
        if(mev->button() == Qt::LeftButton) m_pressPos = mev->pos();
	}
	else if (ev->type() == QEvent::MouseButtonRelease)
	{
		auto mev = dynamic_cast<QMouseEvent*>(ev);
        if(mev->button() == Qt::LeftButton) m_pressPos = {0,0};
	}
	else if (ev->type() == QEvent::MouseMove)
	{
		auto mev = dynamic_cast<QMouseEvent*>(ev);
        if (mev->buttons() & Qt::LeftButton && !m_pressPos.isNull())
        {
			//无边框窗口拖动
            if (w->windowFlags() & Qt::FramelessWindowHint)
            {
				w->move(mev->globalPos() - m_pressPos);
			}
			//有边框窗口的拖动
            else
            {
				auto frameHeight = w->frameSize() - w->size();
				w->move(mev->globalPos() - m_pressPos - QPoint(0, frameHeight.height()));
			}

			return true;
		}
	}

	return false;
}
