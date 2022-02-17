
#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QDebug>
#include "dots.h"
#include "pointmove.h"
#include "polyline.h"
#include "ui_polyline.h"
#include <QColorDialog>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <color.h>


VEPolyline::VEPolyline(QObject *parent) :
    QObject(parent)
{
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges);
}

VEPolyline::~VEPolyline()
{

}

QPointF VEPolyline::previousPosition() const
{
    return m_previousPosition;
}

void VEPolyline::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged();
}

void VEPolyline::setPath(const QPainterPath &path)
{
    QGraphicsPathItem::setPath(path);
}

void VEPolyline::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_leftMouseButtonPressed) {
        auto dx = event->scenePos().x() - m_previousPosition.x();
        auto dy = event->scenePos().y() - m_previousPosition.y();
        moveBy(dx,dy);
        setPreviousPosition(event->scenePos());
        emit signalMove(this, dx, dy);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void VEPolyline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        setPreviousPosition(event->scenePos());
        emit clicked(this);
    }
    QGraphicsItem::mousePressEvent(event);
}

void VEPolyline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void VEPolyline::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF clickPos = event->pos();
    QLineF checkLineFirst(clickPos.x() - 5, clickPos.y() - 5, clickPos.x() + 5, clickPos.y() + 5);
    QLineF checkLineSecond(clickPos.x() + 5, clickPos.y() - 5, clickPos.x() - 5, clickPos.y() + 5);
    QPainterPath oldPath = path();
    QPainterPath newPath;
    for(int i = 0; i < oldPath.elementCount(); i++){
        QLineF checkableLine(oldPath.elementAt(i), oldPath.elementAt(i+1));
        if(checkableLine.intersect(checkLineFirst,0) == 1 || checkableLine.intersect(checkLineSecond,0) == 1){
            if(i == 0){
                newPath.moveTo(oldPath.elementAt(i));
                newPath.lineTo(clickPos);
            } else {
                newPath.lineTo(oldPath.elementAt(i));
                newPath.lineTo(clickPos);
            }
        } else {
            if(i == 0){
                newPath.moveTo(oldPath.elementAt(i));
            } else {
                newPath.lineTo(oldPath.elementAt(i));
            }
        }
        if(i == (oldPath.elementCount() - 2)) {
            newPath.lineTo(oldPath.elementAt(i + 1));
            i++;
        }
    }
    setPath(newPath);
    updateDots();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void VEPolyline::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!listDotes.isEmpty()){
        foreach (DotSignal *dot, listDotes) {
            dot->deleteLater();
        }
        listDotes.clear();
    }
    QGraphicsItem::hoverLeaveEvent(event);
}

void VEPolyline::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverMoveEvent(event);
}

void VEPolyline::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPainterPath linePath = path();
    for(int i = 0; i < linePath.elementCount(); i++){
        QPointF point = linePath.elementAt(i);
        DotSignal *dot = new DotSignal(point, i, this);
        connect(dot, &DotSignal::signalMove, this, &VEPolyline::slotMove);
        connect(dot, &DotSignal::signalMouseRelease, this, &VEPolyline::checkForDeletePoints);
        dot->setDotFlags(DotSignal::Movable);
        listDotes.append(dot);
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void VEPolyline::slotMove(QGraphicsItem *signalOwner, qreal dx, qreal dy)
{
    QPainterPath linePath = path();
    for(int i = 0; i < linePath.elementCount(); i++){
        if(listDotes.at(i) == signalOwner){
            QPointF pathPoint = linePath.elementAt(i);
            linePath.setElementPositionAt(i, pathPoint.x() + dx, pathPoint.y() + dy);
            m_pointForCheck = i;
        }
    }
    setPath(linePath);
}

void VEPolyline::checkForDeletePoints()
{
    if(m_pointForCheck != -1){
        QPainterPath linePath = path();

        QPointF pathPoint = linePath.elementAt(m_pointForCheck);
        if(m_pointForCheck > 0){
            QLineF lineToNear(linePath.elementAt(m_pointForCheck-1),pathPoint);
            if(lineToNear.length() < 6.0) {
                QPainterPath newPath;
                newPath.moveTo(linePath.elementAt(0));
                for(int i = 1; i < linePath.elementCount(); i++){
                    if(i != m_pointForCheck){
                        newPath.lineTo(linePath.elementAt(i));
                    }
                }
                setPath(newPath);
            }
        }
        if(m_pointForCheck < linePath.elementCount() - 1){
            QLineF lineToNear(linePath.elementAt(m_pointForCheck+1),pathPoint);
            if(lineToNear.length() < 6.0) {
                QPainterPath newPath;
                newPath.moveTo(linePath.elementAt(0));
                for(int i = 1; i < linePath.elementCount(); i++){
                    if(i != m_pointForCheck){
                        newPath.lineTo(linePath.elementAt(i));
                    }
                }
                setPath(newPath);
            }
        }
        updateDots();
        m_pointForCheck = -1;
    }
}

void VEPolyline::updateDots()
{
    if(!listDotes.isEmpty()){
        foreach (DotSignal *dot, listDotes) {
            dot->deleteLater();
        }
        listDotes.clear();
    }
    QPainterPath linePath = path();
    for(int i = 0; i < linePath.elementCount(); i++){
        QPointF point = linePath.elementAt(i);
        DotSignal *dot = new DotSignal(point, i, this);
        connect(dot, &DotSignal::signalMove, this, &VEPolyline::slotMove);
        connect(dot, &DotSignal::signalMouseRelease, this, &VEPolyline::checkForDeletePoints);
        dot->setDotFlags(DotSignal::Movable);
        listDotes.append(dot);
    }
}



 polyline:: polyline(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::polyline),
    currentPolyline(nullptr)
{
    ui->setupUi(this);
    setLineColor(QColor(Qt::black));
    setLineWidth(1);

    connect(ui->lineColor, &ColorLabel::clicked,
            [=](){
        QColorDialog dialog;
        connect(&dialog, &QColorDialog::colorSelected, this, &polyline::setLineColor);
        dialog.exec();
    });

    connect(ui->lineWidth, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, & polyline::setLineWidth);
}

 polyline::~ polyline()
{
    delete ui;
    delete currentPolyline;
}

QColor  polyline::lineColor() const
{
    return m_lineColor;
}

int  polyline::lineWidth() const
{
    return m_lineWidth;
}

void  polyline::setLineColor(const QColor &color)
{
    m_lineColor = color;
    ui->lineColor->setColor(m_lineColor);
    if(currentPolyline != nullptr){
        QPen pen(color,currentPolyline->pen().width());
        currentPolyline->setPen(pen);
    }
    emit lineColorChanged(m_lineColor);
}

void  polyline::setLineWidth(const int &width)
{
    m_lineWidth = width;
    ui->lineWidth->setValue(m_lineWidth);
    if(currentPolyline != nullptr){
        QPen pen(currentPolyline->pen().color(),width);
        currentPolyline->setPen(pen);
    }
    emit lineWidthChanged(m_lineWidth);
}

void  polyline::newPolyline(VEPolyline *polyline)
{
    polyline->setBrush(QBrush(Qt::transparent));
    polyline->setPen(QPen(m_lineColor, m_lineWidth));
}

void  polyline::loadPolyline(VEPolyline *polyline)
{
    currentPolyline = polyline;
    m_lineColor = currentPolyline->pen().color();
    m_lineWidth = currentPolyline->pen().width();
    ui->lineColor->setColor(m_lineColor);
    ui->lineWidth->setValue(m_lineWidth);
}

void  polyline::deselect()
{
    currentPolyline = nullptr;
}

void  polyline::setVisible(bool visible)
{
    if(!visible && currentPolyline != nullptr){
        QWidget::setVisible(true);
    } else {
        QWidget::setVisible(visible);
    }
}
