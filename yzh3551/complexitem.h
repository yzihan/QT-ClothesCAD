#ifndef COMPLEXITEM_H
#define COMPLEXITEM_H


#include "baseitem.h"
class VEComplexShapeItem : public VEBaseItem
{
public:
    enum SegmentType {SEGEMENT_LINE,SEGEMENT_CURVE};
    struct Segment{SegmentType type;Handle *handles;};
    VEComplexShapeItem(QGraphicsScene *scene, QGraphicsItem *parent=0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void addPoint(QPointF point,SegmentType type);
private:
    void recalculateRect();
    QList<Handle *> mShapeHandles;
    QList<Segment *> mSegments;
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // COMPLEXITEM_H
