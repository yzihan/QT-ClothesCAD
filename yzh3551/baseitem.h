#ifndef VEBACEITEM_H
#define VEBACEITEM_H

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include "handle.h"

class VEBaseItem : public QGraphicsItem
{
public:
    VEBaseItem(QGraphicsScene *scene = 0,QGraphicsItem *parent = 0);
    virtual QRectF boundingRect() const;
    QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setId(QString id);
    QString id();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void createHandles();
    void setDrawBoundingRect(bool draw);

    QList<Handle *> mHandles;
    QRectF mRect;//矩形边框
    QPointF mOrigin;//原点
    bool mDrawBoundingRect; //是否画边框
private:
    Handle *mCurrentHandle;
    QString mId;
};


class VEBasicShapesItem : public VEBaseItem
{
public:
    enum ItemType {ITEM_LINE,ITEM_CIRCLE,ITEM_RECTANGLE,ITEM_ELLIPSE,ITEM_PIXMAP};
    VEBasicShapesItem(qreal x, qreal y, qreal w, qreal h,ItemType type,QGraphicsScene *scene, QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void setType();
    void setPixmap(QString path);
private:
    QPixmap mPixmap;
    ItemType mType;
};

#endif // VEBACEITEM_H
