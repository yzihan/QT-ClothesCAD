#include "ellipse.h"
#include <QPainter>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <math.h>
#include "dots.h"
#include <ellchange.h>
#include <QColorDialog>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <ui_ellipse.h>
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

VEEllipse::VEEllipse(QObject *parent) :
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

VEEllipse::~VEEllipse()
{
    for(int i = 0; i < 8; i++){
        delete cornerGrabber[i];
    }
}

QPointF VEEllipse::previousPosition() const
{
    return m_previousPosition;
}

void VEEllipse::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged();
}

void VEEllipse::setElli(qreal x, qreal y, qreal w, qreal h)
{
    setElli(QRectF(x,y,w,h));
}

void VEEllipse::setElli(const QRectF &rect)
{
    QGraphicsEllipseItem::setRect(rect);
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

void VEEllipse::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pt = event->pos();
    if(m_actionFlags == ResizeState){
        switch (m_cornerFlags) {
        case Top:
            resizeTop(pt);
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
            resizeTop(pt);
            resizeLeft(pt);
            break;
        case TopRight:
            resizeTop(pt);
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

void VEEllipse::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        setPreviousPosition(event->scenePos());
        emit clicked(this);
    }
    QGraphicsItem::mousePressEvent(event);
}

void VEEllipse::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void VEEllipse::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    m_actionFlags = (m_actionFlags == ResizeState)?RotationState:ResizeState;
    setVisibilityGrabbers();
    QGraphicsItem::mouseDoubleClickEvent(event);

    if(ed.exec() == QDialog::Accepted)
        {

        QRectF tmpRect = rect();

        tmpRect.setWidth(ed.getWide());
        tmpRect.setHeight(ed.getHigh());

        setRect( tmpRect );

        update();

        }
}

void VEEllipse::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setPositionGrabbers();
    setVisibilityGrabbers();
    QGraphicsItem::hoverEnterEvent(event);
}

void VEEllipse::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_cornerFlags = 0;
    hideGrabbers();
    setCursor(Qt::CrossCursor);
    QGraphicsItem::hoverLeaveEvent( event );
}

void VEEllipse::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pt = event->pos();              // 当前位置
    qreal drx = pt.x() - rect().right();    // 与右侧距离
    qreal dlx = pt.x() - rect().left();     // 于左侧距离

    qreal dby = pt.y() - rect().top();      // 上
    qreal dty = pt.y() - rect().bottom();   // 下

    //如果在四个顶点半径为9的范围内，就设置flag标志

    m_cornerFlags = 0;
    if( dby < 9 && dby > -9 ) m_cornerFlags |= Top;       // Top side
    if( dty < 9 && dty > -9 ) m_cornerFlags |= Bottom;    // Bottom side
    if( drx < 9 && drx > -9 ) m_cornerFlags |= Right;     // Right side
    if( dlx < 9 && dlx > -9 ) m_cornerFlags |= Left;      // Left side

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

QVariant VEEllipse::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
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

void VEEllipse::resizeLeft(const QPointF &pt)
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

void VEEllipse::resizeRight(const QPointF &pt)
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

void VEEllipse::resizeBottom(const QPointF &pt)
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

void VEEllipse::resizeTop(const QPointF &pt)
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

void VEEllipse::rotateItem(const QPointF &pt)
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
    //获取角度
    qreal angleToTarget = ::acos(lineToTarget.dx() / lineToTarget.length());
    qreal angleToCursor = ::acos(lineToCursor.dx() / lineToCursor.length());

    if (lineToTarget.dy() < 0)
        angleToTarget = TwoPi - angleToTarget;
    angleToTarget = normalizeAngle((Pi - angleToTarget) + Pi / 2);

    if (lineToCursor.dy() < 0)
        angleToCursor = TwoPi - angleToCursor;
    angleToCursor = normalizeAngle((Pi - angleToCursor) + Pi / 2);

    //计算角度
    auto resultAngle = angleToTarget - angleToCursor;

    QTransform trans = transform();
    trans.translate( center.x(), center.y());
    trans.rotateRadians(rotation() + resultAngle, Qt::ZAxis);
    trans.translate( -center.x(),  -center.y());
    setTransform(trans);
}

void VEEllipse::setPositionGrabbers()
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

void VEEllipse::setVisibilityGrabbers()
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

void VEEllipse::hideGrabbers()
{
    for(int i = 0; i < 8; i++){
        cornerGrabber[i]->setVisible(false);
    }
}



VEEllipseSettings::VEEllipseSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VEEllipseSettings),
    currentEllipse(nullptr)
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
        connect(&dialog, &QColorDialog::colorSelected, this, &VEEllipseSettings::setColor_1);
        dialog.exec();
    });
    connect(ui->color_2, &ColorLabel::clicked,
            [=](){
        QColorDialog dialog;
        connect(&dialog, &QColorDialog::colorSelected, this, &VEEllipseSettings::setColor_2);
        dialog.exec();
    });
    connect(ui->borderColor, &ColorLabel::clicked,
            [=](){
        QColorDialog dialog;
        connect(&dialog, &QColorDialog::colorSelected, this, &VEEllipseSettings::setBorderColor);
        dialog.exec();
    });

    connect(ui->comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &VEEllipseSettings::comboBoxIndexChanged);
    connect(ui->borderWidth, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &VEEllipseSettings::setBorderWidth);
}

VEEllipseSettings::~VEEllipseSettings()
{
    delete ui;
    delete currentEllipse;
}

QColor VEEllipseSettings::color_1() const
{
    return m_color_1;
}

QColor VEEllipseSettings::color_2() const
{
    return m_color_2;
}

QColor VEEllipseSettings::borderColor() const
{
    return m_borderColor;
}

int VEEllipseSettings::borderWidth() const
{
    return m_borderWidth;
}

void VEEllipseSettings::setColor_1(const QColor &color)
{
    m_color_1 = color;
    ui->color_1->setColor(color);
    if(currentEllipse != nullptr){
        if(ui->comboBox->currentIndex() == 0){
            currentEllipse->setBrush(QBrush(m_color_1));
        } else {
             setGradient(currentEllipse);
        }
    }
    emit color_1Changed(m_color_1);
}

void VEEllipseSettings::setColor_2(const QColor &color)
{
    m_color_2 = color;
    ui->color_2->setColor(color);
    if(currentEllipse != nullptr){
         setGradient(currentEllipse);
    }
    emit color_2Changed(m_color_2);
}

void VEEllipseSettings::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    ui->borderColor->setColor(color);
    if(currentEllipse != nullptr){
        QPen pen(color,currentEllipse->pen().width());
        currentEllipse->setPen(pen);
    }
    emit borderColorChanged(m_borderColor);
}

void VEEllipseSettings::setBorderWidth(const int &width)
{
    m_borderWidth = width;
    if(currentEllipse != nullptr){
        if(width == 0){
            currentEllipse->setPen(Qt::NoPen);
        } else {
            QPen pen(currentEllipse->pen().color(), width);
            currentEllipse->setPen(pen);
        }
    }
    emit borderWidthChanged(m_borderWidth);
}

void VEEllipseSettings::newEllipse(VEEllipse *elli)
{
    if(ui->comboBox->currentIndex() == 0){
        elli->setBrush(QBrush(m_color_1));
    } else {
        setGradient(elli);
    }
    if(m_borderWidth == 0){
        elli->setPen(Qt::NoPen);
    } else {
        elli->setPen(QPen(m_borderColor, m_borderWidth));
    }
}

void VEEllipseSettings::loadEllipse(VEEllipse *elli)
{
    currentEllipse = elli;

    m_color_1 = currentEllipse->brush().color();
    m_borderColor = currentEllipse->pen().color();
    if(currentEllipse->pen().style() == Qt::NoPen){
        m_borderWidth = 0;
    } else {
        m_borderWidth = currentEllipse->pen().width();
    }
    ui->color_1->setColor(m_color_1);
    ui->borderColor->setColor(m_borderColor);
    ui->borderWidth->setValue(m_borderWidth);
}

void VEEllipseSettings::deselect()
{
    currentEllipse = nullptr;
}

void VEEllipseSettings::setVisible(bool visible)
{
    if(!visible && currentEllipse != nullptr){
        QWidget::setVisible(true);
    } else {
        QWidget::setVisible(visible);
    }
}

void VEEllipseSettings::comboBoxIndexChanged(int index)
{
    switch (index) {
    case 1:
        ui->color_2->setVisible(true);
        ui->labelColor_2->setVisible(true);
        ui->labelColor_1->setText(trUtf8("<font color='white'>颜色1</font>"));
        if(currentEllipse != nullptr){
            setGradient(currentEllipse);
        }
        break;
    default:
        ui->color_2->setVisible(false);
        ui->labelColor_2->setVisible(false);
        ui->labelColor_1->setText(trUtf8("<font color='white'>背景</font>"));
        if(currentEllipse != nullptr){
            currentEllipse->setBrush(QBrush(m_color_1));
        }
        break;
    }
}

void VEEllipseSettings::setGradient(VEEllipse *ellipse)
{
    auto tmpRect = ellipse->rect();
    QLinearGradient gradient(tmpRect.left() + tmpRect.width()/2,
                             tmpRect.top(),
                             tmpRect.left() + tmpRect.width()/2,
                             tmpRect.bottom());
    gradient.setColorAt(0.0, m_color_1);
    gradient.setColorAt(1.0, m_color_2);
    ellipse->setBrush(QBrush(gradient));
}
