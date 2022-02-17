
#include <QUuid>
#include <QSettings>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include "baseitem.h"
#include "handle.h"
#include <QPainter>
#include <QGraphicsScene>


VEBaseItem::VEBaseItem(QGraphicsScene *scene, QGraphicsItem *parent) : QGraphicsItem(parent),mCurrentHandle(0)
{
    QUuid id;
    id = QUuid::createUuid();
    this->mId = id.toString();
    this->mDrawBoundingRect = true;
    this->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges);
    if(scene) {
        scene->addItem(this);
    }
}
void VEBaseItem::setId(QString id) {
    this->mId = id;
}
QString VEBaseItem::id() {
    return this->mId;
}
void VEBaseItem::setDrawBoundingRect(bool draw) {
    this->mDrawBoundingRect = draw;
}

QRectF VEBaseItem::boundingRect() const {
    QSettings settings;
    int size = settings.value("drawing/hanleSize",10).toInt();
    return this->mRect.adjusted(-size/2,-size/2 - 50,size/2,size/2);
}
QPainterPath VEBaseItem::shape() const {
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    if(this->isSelected()) {
        foreach (Handle *handle, mHandles) {
            switch (handle->shape()) {
            case Handle::HANDLE_SHAPE_RECT:
                path.addRect(handle->boundingRect());
                break;
            case Handle::HANDLE_SHAPE_CIRCLE:
                path.addRect(handle->boundingRect());
                break;
            case Handle::HANDLE_SHAPE_TRIANGLE:
                break;
            }
        }
    }
    path.addRect(this->mRect);
    return path;
}
void VEBaseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //If the item is selected draw selection rectangle and handles.
    if(this->isSelected()) {
        if(this->mDrawBoundingRect) {
            //Draw Selection Triangle.
            QPen pen(Qt::black);
            painter->setPen(pen);
            painter->drawRect(this->mRect);
        }
        //Drawing the handles.
        QPointF p1;
        QPointF p2;
        foreach (Handle *handle, mHandles) {
            if(handle->type() == Handle::HANDLE_TYPE_ROTATE) {
                p1 = handle->pos();
            }
            if(handle->type() == Handle::HANDLE_TYPE_TOP) {
                p2 = handle->pos();
            }
            if(handle->type() == Handle::HANDLE_TYPE_CTRL) {
                painter->save();
                QPen pen(Qt::black);
                painter->setPen(pen);
                //painter->setPen(QPen(Qt::black));
                painter->setBackground(QBrush(Qt::black));
                painter->drawRect(handle->boundingRect());
                painter->restore();
            } else {
                switch (handle->shape()) {
                case Handle::HANDLE_SHAPE_RECT:
                    painter->drawRect(handle->boundingRect());
                    break;
                case Handle::HANDLE_SHAPE_CIRCLE:
                    painter->drawEllipse(handle->boundingRect());
                    break;
                case Handle::HANDLE_SHAPE_TRIANGLE:
                    break;
                }
            }
        }
        painter->drawLine(p1,p2);
        painter->drawPoint(this->mOrigin);
    }
}
void VEBaseItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
    if(event->buttons() == Qt::LeftButton) {
        //检测控制点
        foreach (Handle *handle, mHandles) {
            if(handle->boundingRect().contains(event->pos())) {
                this->mCurrentHandle = handle;
            }
        }
    }
}

void VEBaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    if(event->buttons() == Qt::LeftButton && mCurrentHandle) {
        qreal dx = (mOrigin.x() - mRect.center().x()) / mRect.width();
        qreal dy = (mOrigin.y() - mRect.center().y()) / mRect.height();
        switch (mCurrentHandle->type()) {
        case Handle::HANDLE_TYPE_LEFT:
            this->mRect.setLeft(event->pos().x());
            //更新点
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            break;
        case Handle::HANDLE_TYPE_RIGHT:
            this->mRect.setRight(event->pos().x());
            // 更新坐标
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            break;
        case Handle::HANDLE_TYPE_TOP:
            this->mRect.setTop(event->pos().y());
            // 更新坐标
            mOrigin.setY((dy * mRect.height()) + mRect.center().y());
            break;
        case Handle::HANDLE_TYPE_BOTTOM:
            this->mRect.setBottom(event->pos().y());
            // 更新坐标
            mOrigin.setY((dy * mRect.height()) + mRect.center().y());
            break;
        case Handle::HANDLE_TYPE_TOPLEFT:
            this->mRect.setTopLeft(event->pos());
            // 更新坐标
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            mOrigin.setY((dy * mRect.height()) + mRect.center().y());
            break;
        case Handle::HANDLE_TYPE_TOPRIGHT:
            this->mRect.setTopRight(event->pos());
            // 更新坐标
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            mOrigin.setY((dy * mRect.height()) + mRect.center().y());
            break;
        case Handle::HANDLE_TYPE_BOTTOMLEFT:
            this->mRect.setBottomLeft(event->pos());
            // 更新坐标
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            mOrigin.setY((dy * mRect.height()) + mRect.center().y());
            break;
        case Handle::HANDLE_TYPE_BOTTOMRIGHT:
            this->mRect.setBottomRight(event->pos());
            // 更新坐标
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            mOrigin.setY((dy * mRect.height()) + mRect.center().y());
            break;
        case Handle::HANDLE_TYPE_ROTATE:
            //Setting the origin point.
            this->setTransform(QTransform().translate(mOrigin.x(),mOrigin.y()).rotate(-QLineF(event->scenePos(),mapToScene(mOrigin)).angle() + QLineF(event->lastScenePos(),mapToScene(mOrigin)).angle()).translate(-mOrigin.x(),-mOrigin.y()),true);
            break;
        case Handle::HANDLE_TYPE_ORIGIN:
            mCurrentHandle->setPos(event->pos());
            this->mOrigin = mCurrentHandle->pos();
            break;
        case Handle::HANDLE_TYPE_CTRL:
            mCurrentHandle->setPos(event->pos());
            break;
        default:
            QGraphicsItem::mouseMoveEvent(event);
            break;
        }
        //更新坐标
        QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
        QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
        QPointF top(mRect.left()+mRect.width()/2,mRect.top());
        QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
        QPointF rotate(top.x(),top.y()-50);
        foreach (Handle *handle, mHandles) {
            switch (handle->type()) {
            case Handle::HANDLE_TYPE_LEFT:
                handle->setPos(left);
                break;
            case Handle::HANDLE_TYPE_RIGHT:
                handle->setPos(right);
                break;
            case Handle::HANDLE_TYPE_TOP:
                handle->setPos(top);
                break;
            case Handle::HANDLE_TYPE_BOTTOM:
                handle->setPos(bottom);
                break;
            case Handle::HANDLE_TYPE_TOPLEFT:
                handle->setPos(mRect.topLeft());
                break;
            case Handle::HANDLE_TYPE_TOPRIGHT:
                handle->setPos(mRect.topRight());
                break;
            case Handle::HANDLE_TYPE_BOTTOMLEFT:
                handle->setPos(mRect.bottomLeft());
                break;
            case Handle::HANDLE_TYPE_BOTTOMRIGHT:
                handle->setPos(mRect.bottomRight());
                break;
            case Handle::HANDLE_TYPE_ROTATE:
                handle->setPos(rotate);
                break;
            case Handle::HANDLE_TYPE_ORIGIN:
                handle->setPos(mOrigin);
                break;
            default:
                break;
            }
        }
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void VEBaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
    //Reset handles states.
    this->mCurrentHandle = 0;
}

QVariant VEBaseItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // 新的位置
        QSettings settings;
        int gridSize = settings.value("drawing/gridSize").toInt();
        bool gridEnabled = settings.value("drawing/gridEnabled").toBool();

        QPointF newPos = value.toPointF();
        if (gridEnabled) {
            if((int)newPos.x()%gridSize != 0) {
                int x = (int)newPos.x() - (int)newPos.x()%gridSize;
                newPos.setX(x);
            }
            if((int)newPos.y()%gridSize != 0) {
                int y = (int)newPos.y() - (int)newPos.y()%gridSize;
                newPos.setY(y);
            }
        }
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}
void VEBaseItem::createHandles() {
    QSettings settings;
    int size = settings.value("drawing/hanleSize",10).toInt();
    //Resizing rectangle.
    mOrigin = this->mRect.center();
    QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
    QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
    QPointF top(mRect.left()+mRect.width()/2,mRect.top());
    QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
    QPointF rotate(top.x(),top.y()-50);
    mHandles<<new Handle(mRect.topLeft(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_TOPLEFT);
    mHandles<<new Handle(top,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_TOP);
    mHandles<<new Handle(mRect.topRight(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_TOPRIGHT);
    mHandles<<new Handle(left,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_LEFT);
    mHandles<<new Handle(right,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_RIGHT);
    mHandles<<new Handle(mRect.bottomLeft(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_BOTTOMLEFT);
    mHandles<<new Handle(bottom,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_BOTTOM);
    mHandles<<new Handle(mRect.bottomRight(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_BOTTOMRIGHT);
    mHandles<<new Handle(rotate,size,Handle::HANDLE_SHAPE_CIRCLE,Handle::HANDLE_TYPE_ROTATE);
    mHandles<<new Handle(mOrigin,size,Handle::HANDLE_SHAPE_CIRCLE,Handle::HANDLE_TYPE_ORIGIN);
}


VEBasicShapesItem::VEBasicShapesItem(qreal x, qreal y, qreal w, qreal h,ItemType type,QGraphicsScene *scene, QGraphicsItem *parent) : VEBaseItem(scene,parent)
{
    mRect.setRect(x,y,w,h);
    mType = type;
    createHandles();
}

void VEBasicShapesItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //坐标修改
    if(mRect.left() > mRect.right()) {
        qreal left = mRect.left();
        qreal right = mRect.right();
        mRect.setLeft(right);
        mRect.setRight(left);
    }
    if(mRect.top() > mRect.bottom()) {
        qreal top = mRect.top();
        qreal bottom = mRect.bottom();
        mRect.setTop(bottom);
        mRect.setBottom(top);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    switch (mType) {
    case ITEM_RECTANGLE:
        painter->drawRect(this->mRect);
        break;
    case ITEM_ELLIPSE:
        painter->drawEllipse(this->mRect);
        break;
    case ITEM_LINE:
        painter->drawLine(this->mRect.topLeft(),this->mRect.bottomRight());
        break;
    case ITEM_PIXMAP:
        //painter->drawPixmap(this->mRect.topLeft(),this->mPixmap,this->mRect);
        painter->drawPixmap(this->mRect.toRect(),this->mPixmap);
        break;
    default:
        break;
    }
    VEBaseItem::paint(painter,option,widget);
    scene()->update();
}

void VEBasicShapesItem::setPixmap(QString path) {
    this->mPixmap = QPixmap(path);
}

QRectF VEBasicShapesItem::boundingRect() const{
    return VEBaseItem::boundingRect();
}
