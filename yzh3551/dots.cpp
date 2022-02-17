#include "dots.h"
#include <QBrush>
#include <QColor>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <polyline.h>

DotSignal::DotSignal(QGraphicsItem *parentItem, QObject *parent) :
    QObject(parent)
{
    setParentItem(parentItem);
    setAcceptHoverEvents(true);
    //setBrush(QBrush(Qt::black));
    setRect(-4,-4,8,8);
    setDotFlags(0);
}

DotSignal::DotSignal(QPointF pos, QGraphicsItem *parentItem, QObject *parent) :
    QObject(parent)
{
    setParentItem(parentItem);
    setAcceptHoverEvents(true);
    //setBrush(QBrush(Qt::black));
    setRect(-4,-4,8,8);
    setPos(pos);
    setPreviousPosition(pos);
    setDotFlags(0);
}

DotSignal::DotSignal(QPointF pos, int id, QGraphicsItem *parentItem, QObject *parent) :
    QObject(parent),
    id(id)
{
    setParentItem(parentItem);
    setAcceptHoverEvents(true);
    //setBrush(QBrush(Qt::black));
    setRect(-4,-4,8,8);
    setPos(pos);
    setPreviousPosition(pos);
    setDotFlags(0);
}

DotSignal::~DotSignal()
{

}

QPointF DotSignal::previousPosition() const
{
    return m_previousPosition;
}

void DotSignal::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged();
}

void DotSignal::setDotFlags(unsigned int flags)
{
    m_flags = flags;
}

void DotSignal::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_flags & Movable){
        auto dx = event->scenePos().x() - m_previousPosition.x();
        auto dy = event->scenePos().y() - m_previousPosition.y();
        moveBy(dx,dy);
        setPreviousPosition(event->scenePos());
        emit signalMove(this, dx, dy);
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void DotSignal::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_flags & Movable){
        setPreviousPosition(event->scenePos());
    } else {
        QGraphicsItem::mousePressEvent(event);
    }
}

void DotSignal::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalMouseRelease();
    QGraphicsItem::mouseReleaseEvent(event);
}

void DotSignal::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setBrush(QBrush(Qt::red));
}

void DotSignal::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setBrush(QBrush(Qt::black));
}


void DotSignal:: mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){

    qreal dx = 0;
    qreal dy = 0;
    bool moved = false;
    {
        if(dot.exec() == QDialog::Accepted)
            {

             dx = dot.getleft()-dot.getright();
            dy = dot.getup()-dot.getdown();
            moved = true;
        }else{
        QGraphicsItem::mouseDoubleClickEvent(event);
        }
    }
    if(moved){
        auto p = ((VEPolyline*)parentItem())->path();
        p.setElementPositionAt(id, p.elementAt(id).x - dx, p.elementAt(id).y - dy);
        ((VEPolyline*)parentItem())->setPath(p);
        ((VEPolyline*)parentItem())->updateDots();
        moveBy(dx,dy);
    }
}
