#include "rectangle.h"
#include <QPainter>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <math.h>
#include "dots.h"
#include "ui_rectangle.h"
#include <QColorDialog>
#include <QBrush>
#include <QPen>
#include <color.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

static qreal normalizeAngle(qreal angle)
{
    while (angle < 0)
        angle += TwoPi;
    while (angle > TwoPi)
        angle -= TwoPi;
    return angle;
}

VERectangle::VERectangle(QObject *parent) :
    QObject(parent),
    m_cornerFlags(0),
    m_actionFlags(ResizeState)
{
    setAcceptHoverEvents(true);
    setFlags(ItemIsSelectable|ItemSendsGeometryChanges);
    for(int i = 0; i < 8; i++){
        cornerGrabber[i] = new DotSignal(this);
    }
    setPositionGrabbers();
}

VERectangle::~VERectangle()
{
    for(int i = 0; i < 8; i++){
        delete cornerGrabber[i];
    }
}

QPointF VERectangle::previousPosition() const
{
    return m_previousPosition;
}

void VERectangle::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged();
}

void VERectangle::setRect(qreal x, qreal y, qreal w, qreal h)
{
    setRect(QRectF(x,y,w,h));
}

void VERectangle::setRect(const QRectF &rect)
{
    QGraphicsRectItem::setRect(rect);
    if(brush().gradient() != 0){
        const QGradient * grad = brush().gradient();
        if(grad->type() == QGradient::LinearGradient){
            auto tmpRect = this->rect();
            const QLinearGradient *lGradient = static_cast<const QLinearGradient *>(grad);
            QLinearGradient g = *const_cast<QLinearGradient*>(lGradient);
            g.setStart(tmpRect.left() + tmpRect.width()/2,tmpRect.top());
            g.setFinalStop(tmpRect.left() + tmpRect.width()/2,tmpRect.bottom());
            setBrush(g);
        }
    }
}

void VERectangle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pt = event->pos();
    if(m_actionFlags == ResizeState){
        switch (m_cornerFlags) {
        case Top:
//            resizeTop(pt);
            sendSignal(0, &pt);
            break;
        case Bottom:
            resizeBottom(pt);
            break;
        case Left:
            resizeLeft(pt);
            break;
        case Right:
            resizeRight(pt);
            break;
        case TopLeft:
//            resizeTop(pt);
            sendSignal(0, &pt);
            resizeLeft(pt);
            break;
        case TopRight:
//            resizeTop(pt);
            sendSignal(0, &pt);
            resizeRight(pt);
            break;
        case BottomLeft:
            resizeBottom(pt);
            resizeLeft(pt);
            break;
        case BottomRight:
            resizeBottom(pt);
            resizeRight(pt);
            break;
        default:
            if (m_leftMouseButtonPressed) {
                setCursor(Qt::ClosedHandCursor);
                auto dx = event->scenePos().x() - m_previousPosition.x();
                auto dy = event->scenePos().y() - m_previousPosition.y();
                moveBy(dx,dy);
                setPreviousPosition(event->scenePos());
                emit signalMove(this, dx, dy);
            }
            break;
        }
    } else {
        switch (m_cornerFlags) {
        case TopLeft:
        case TopRight:
        case BottomLeft:
        case BottomRight: {
            rotateItem(pt);
            break;
        }
        default:
            if (m_leftMouseButtonPressed) {
                setCursor(Qt::ClosedHandCursor);
                auto dx = event->scenePos().x() - m_previousPosition.x();
                auto dy = event->scenePos().y() - m_previousPosition.y();
                moveBy(dx,dy);
                setPreviousPosition(event->scenePos());
                emit signalMove(this, dx, dy);
            }
            break;
        }
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void VERectangle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        setPreviousPosition(event->scenePos());
        emit clicked(this);
    }
    QGraphicsItem::mousePressEvent(event);
}

void VERectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void VERectangle::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    m_actionFlags = (m_actionFlags == ResizeState)?RotationState:ResizeState;
    setVisibilityGrabbers();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void VERectangle::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setPositionGrabbers();
    setVisibilityGrabbers();
    QGraphicsItem::hoverEnterEvent(event);
}

void VERectangle::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_cornerFlags = 0;
    hideGrabbers();
    setCursor(Qt::CrossCursor);
    QGraphicsItem::hoverLeaveEvent( event );
}

void VERectangle::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pt = event->pos();
    qreal drx = pt.x() - rect().right();    // 右
    qreal dlx = pt.x() - rect().left();     // 左

    qreal dby = pt.y() - rect().top();      // 上
    qreal dty = pt.y() - rect().bottom();   // 下


    m_cornerFlags = 0;
    if( dby < 9 && dby > -9 ) m_cornerFlags |= Top;       // Top
    if( dty < 9 && dty > -9 ) m_cornerFlags |= Bottom;    // Bottom
    if( drx < 9 && drx > -9 ) m_cornerFlags |= Right;     // Right
    if( dlx < 9 && dlx > -9 ) m_cornerFlags |= Left;      // Left

    if(m_actionFlags == ResizeState){
        QPixmap p(":/icons/arrow-up-down.png");
        QPixmap pResult;
        QTransform trans = transform();
        switch (m_cornerFlags) {
        case Top:
        case Bottom:
            pResult = p.transformed(trans);
            setCursor(pResult.scaled(24,24,Qt::KeepAspectRatio));
            break;
        case Left:
        case Right:
            trans.rotate(90);
            pResult = p.transformed(trans);
            setCursor(pResult.scaled(24,24,Qt::KeepAspectRatio));
            break;
        case TopRight:
        case BottomLeft:
            trans.rotate(45);
            pResult = p.transformed(trans);
            setCursor(pResult.scaled(24,24,Qt::KeepAspectRatio));
            break;
        case TopLeft:
        case BottomRight:
            trans.rotate(135);
            pResult = p.transformed(trans);
            setCursor(pResult.scaled(24,24,Qt::KeepAspectRatio));
            break;
        default:
            setCursor(Qt::CrossCursor);
            break;
        }
    } else {
        switch (m_cornerFlags) {
        case TopLeft:
        case TopRight:
        case BottomLeft:
        case BottomRight: {
            QPixmap p(":/icons/rotate-right.png");
            setCursor(QCursor(p.scaled(24,24,Qt::KeepAspectRatio)));
            break;
        }
        default:
            setCursor(Qt::CrossCursor);
            break;
        }
    }
    QGraphicsItem::hoverMoveEvent( event );
}

QVariant VERectangle::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemSelectedChange:
        m_actionFlags = ResizeState;
        break;
    default:
        break;
    }
    return QGraphicsItem::itemChange(change, value);
}

void VERectangle::resizeLeft(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.x() > tmpRect.right() )
        return;
    qreal widthOffset =  ( pt.x() - tmpRect.right() );
    if( widthOffset > -10 )
        return;
    if( widthOffset < 0 )
        tmpRect.setWidth( -widthOffset );
    else
        tmpRect.setWidth( widthOffset );
    tmpRect.translate( rect().width() - tmpRect.width() , 0 );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
}

void VERectangle::resizeRight(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.x() < tmpRect.left() )
        return;
    qreal widthOffset =  ( pt.x() - tmpRect.left() );
    if( widthOffset < 10 )
        return;
    if( widthOffset < 10)
        tmpRect.setWidth( -widthOffset );
    else
        tmpRect.setWidth( widthOffset );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
}

void VERectangle::resizeBottom(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.y() < tmpRect.top() )
        return;
    qreal heightOffset =  ( pt.y() - tmpRect.top() );
    if( heightOffset < 11 )
        return;
    if( heightOffset < 0)
        tmpRect.setHeight( -heightOffset );
    else
        tmpRect.setHeight( heightOffset );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
}

void VERectangle::resizeTop(const QPointF &pt)
{
    QRectF tmpRect = rect();
    if( pt.y() > tmpRect.bottom() )
        return;
    qreal heightOffset =  ( pt.y() - tmpRect.bottom() );
    if( heightOffset > -11 )
        return;
    if( heightOffset < 0)
        tmpRect.setHeight( -heightOffset );
    else
        tmpRect.setHeight( heightOffset );
    tmpRect.translate( 0 , rect().height() - tmpRect.height() );
    prepareGeometryChange();
    setRect( tmpRect );
    update();
    setPositionGrabbers();
}

void VERectangle::rotateItem(const QPointF &pt)
{
    QRectF tmpRect = rect();
    QPointF center = boundingRect().center();
    QPointF corner;
    switch (m_cornerFlags) {
    case TopLeft:
        corner = tmpRect.topLeft();
        break;
    case TopRight:
        corner = tmpRect.topRight();
        break;
    case BottomLeft:
        corner = tmpRect.bottomLeft();
        break;
    case BottomRight:
        corner = tmpRect.bottomRight();
        break;
    default:
        break;
    }

    QLineF lineToTarget(center,corner);
    QLineF lineToCursor(center, pt);
    qreal angleToTarget = ::acos(lineToTarget.dx() / lineToTarget.length());
    qreal angleToCursor = ::acos(lineToCursor.dx() / lineToCursor.length());

    if (lineToTarget.dy() < 0)
        angleToTarget = TwoPi - angleToTarget;
    angleToTarget = normalizeAngle((Pi - angleToTarget) + Pi / 2);

    if (lineToCursor.dy() < 0)
        angleToCursor = TwoPi - angleToCursor;
    angleToCursor = normalizeAngle((Pi - angleToCursor) + Pi / 2);

    auto resultAngle = angleToTarget - angleToCursor;

    QTransform trans = transform();
    trans.translate( center.x(), center.y());
    trans.rotateRadians(rotation() + resultAngle, Qt::ZAxis);
    trans.translate( -center.x(),  -center.y());
    setTransform(trans);
}

void VERectangle::setPositionGrabbers()
{
    QRectF tmpRect = rect();
    cornerGrabber[GrabberTop]->setPos(tmpRect.left() + tmpRect.width()/2, tmpRect.top());
    cornerGrabber[GrabberBottom]->setPos(tmpRect.left() + tmpRect.width()/2, tmpRect.bottom());
    cornerGrabber[GrabberLeft]->setPos(tmpRect.left(), tmpRect.top() + tmpRect.height()/2);
    cornerGrabber[GrabberRight]->setPos(tmpRect.right(), tmpRect.top() + tmpRect.height()/2);
    cornerGrabber[GrabberTopLeft]->setPos(tmpRect.topLeft().x(), tmpRect.topLeft().y());
    cornerGrabber[GrabberTopRight]->setPos(tmpRect.topRight().x(), tmpRect.topRight().y());
    cornerGrabber[GrabberBottomLeft]->setPos(tmpRect.bottomLeft().x(), tmpRect.bottomLeft().y());
    cornerGrabber[GrabberBottomRight]->setPos(tmpRect.bottomRight().x(), tmpRect.bottomRight().y());
}

void VERectangle::setVisibilityGrabbers()
{
    cornerGrabber[GrabberTopLeft]->setVisible(true);
    cornerGrabber[GrabberTopRight]->setVisible(true);
    cornerGrabber[GrabberBottomLeft]->setVisible(true);
    cornerGrabber[GrabberBottomRight]->setVisible(true);

    if(m_actionFlags == ResizeState){
        cornerGrabber[GrabberTop]->setVisible(true);
        cornerGrabber[GrabberBottom]->setVisible(true);
        cornerGrabber[GrabberLeft]->setVisible(true);
        cornerGrabber[GrabberRight]->setVisible(true);
    } else {
        cornerGrabber[GrabberTop]->setVisible(false);
        cornerGrabber[GrabberBottom]->setVisible(false);
        cornerGrabber[GrabberLeft]->setVisible(false);
        cornerGrabber[GrabberRight]->setVisible(false);
    }
}

void VERectangle::hideGrabbers()
{
    for(int i = 0; i < 8; i++){
        cornerGrabber[i]->setVisible(false);
    }
}

void VERectangle::handleSignal(int type, void* value) {
    switch (type) {
    case 0:
        resizeTop(*reinterpret_cast<QPointF*>(value));
        break;
    }
}



rectangle::rectangle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rectangle),
    currentRectangle(nullptr)
{
    ui->setupUi(this);
    ui->color_2->setVisible(false);
    ui->labelColor_2->setVisible(false);

    setColor_1(Qt::white);
    setColor_2(Qt::white);
    setBorderColor(Qt::black);
    setBorderWidth(1);

    connect(ui->color_1, &ColorLabel::clicked,
            [=](){
        QColorDialog dialog;
        connect(&dialog, &QColorDialog::colorSelected, this, &rectangle::setColor_1);
        dialog.exec();
    });
    connect(ui->color_2, &ColorLabel::clicked,
            [=](){
        QColorDialog dialog;
        connect(&dialog, &QColorDialog::colorSelected, this, &rectangle::setColor_2);
        dialog.exec();
    });
    connect(ui->borderColor, &ColorLabel::clicked,
            [=](){
        QColorDialog dialog;
        connect(&dialog, &QColorDialog::colorSelected, this, &rectangle::setBorderColor);
        dialog.exec();
    });

    connect(ui->comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &rectangle::comboBoxIndexChanged);
    connect(ui->borderWidth, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &rectangle::setBorderWidth);
}

rectangle::~rectangle()
{
    delete ui;
    delete currentRectangle;
}

QColor rectangle::color_1() const
{
    return m_color_1;
}

QColor rectangle::color_2() const
{
    return m_color_2;
}

QColor rectangle::borderColor() const
{
    return m_borderColor;
}

int rectangle::borderWidth() const
{
    return m_borderWidth;
}

void rectangle::setColor_1(const QColor &color)
{
    m_color_1 = color;
    ui->color_1->setColor(color);
    if(currentRectangle != nullptr){
        if(ui->comboBox->currentIndex() == 0){
            currentRectangle->setBrush(QBrush(m_color_1));
        } else {
            setGradient(currentRectangle);
        }
    }
    emit color_1Changed(m_color_1);
}

void rectangle::setColor_2(const QColor &color)
{
    m_color_2 = color;
    ui->color_2->setColor(color);
    if(currentRectangle != nullptr){
        setGradient(currentRectangle);
    }
    emit color_2Changed(m_color_2);
}

void rectangle::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    ui->borderColor->setColor(color);
    if(currentRectangle != nullptr){
        QPen pen(color,currentRectangle->pen().width());
        currentRectangle->setPen(pen);
    }
    emit borderColorChanged(m_borderColor);
}

void rectangle::setBorderWidth(const int &width)
{
    m_borderWidth = width;
    if(currentRectangle != nullptr){
        if(width == 0){
            currentRectangle->setPen(Qt::NoPen);
        } else {
            QPen pen(currentRectangle->pen().color(), width);
            currentRectangle->setPen(pen);
        }
    }
    emit borderWidthChanged(m_borderWidth);
}

void rectangle::newRectangle(VERectangle *rect)
{
    if(ui->comboBox->currentIndex() == 0){
        rect->setBrush(QBrush(m_color_1));
    } else {
        setGradient(rect);
    }
    if(m_borderWidth == 0){
        rect->setPen(Qt::NoPen);
    } else {
        rect->setPen(QPen(m_borderColor, m_borderWidth));
    }
}

void rectangle::loadRectangle(VERectangle *rect)
{
    currentRectangle = rect;

    m_color_1 = currentRectangle->brush().color();
    m_borderColor = currentRectangle->pen().color();
    if(currentRectangle->pen().style() == Qt::NoPen){
        m_borderWidth = 0;
    } else {
        m_borderWidth = currentRectangle->pen().width();
    }
    ui->color_1->setColor(m_color_1);
    ui->borderColor->setColor(m_borderColor);
    ui->borderWidth->setValue(m_borderWidth);
}

void rectangle::deselect()
{
    currentRectangle = nullptr;
}

void rectangle::setVisible(bool visible)
{
    if(!visible && currentRectangle != nullptr){
        QWidget::setVisible(true);
    } else {
        QWidget::setVisible(visible);
    }
}

void rectangle::comboBoxIndexChanged(int index)
{
    switch (index) {
    case 1:
        ui->color_2->setVisible(true);
        ui->labelColor_2->setVisible(true);
        ui->labelColor_1->setText(trUtf8("<font color='white'>颜色1</font>"));
        if(currentRectangle != nullptr){
            setGradient(currentRectangle);
        }
        break;
    default:
        ui->color_2->setVisible(false);
        ui->labelColor_2->setVisible(false);
        ui->labelColor_1->setText(trUtf8("<font color='white'>背景</font>"));
        if(currentRectangle != nullptr){
            currentRectangle->setBrush(QBrush(m_color_1));
        }
        break;
    }
}

void rectangle::setGradient(VERectangle *rectangle)
{
    auto tmpRect = rectangle->rect();
    QLinearGradient gradient(tmpRect.left() + tmpRect.width()/2,
                             tmpRect.top(),
                             tmpRect.left() + tmpRect.width()/2,
                             tmpRect.bottom());
    gradient.setColorAt(0.0, m_color_1);
    gradient.setColorAt(1.0, m_color_2);
    rectangle->setBrush(QBrush(gradient));
}

