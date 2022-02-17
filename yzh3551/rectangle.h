#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QObject>
#include <QGraphicsRectItem>
#include <move.h>

#include <QWidget>
class DotSignal;
class QGraphicsSceneMouseEvent;

class VERectangle : public QObject, public QGraphicsRectItem,public signalable<VERectangle>
{
    Q_OBJECT
    Q_PROPERTY(QPointF previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

public:
    explicit VERectangle(QObject * parent = 0);
    ~VERectangle();

    enum ActionStates {
        ResizeState = 0x01,
        RotationState = 0x02
    };

    enum CornerFlags {
        Top = 0x01,
        Bottom = 0x02,
        Left = 0x04,
        Right = 0x08,
        TopLeft = Top|Left,
        TopRight = Top|Right,
        BottomLeft = Bottom|Left,
        BottomRight = Bottom|Right
    };

    enum CornerGrabbers {
        GrabberTop = 0,
        GrabberBottom,
        GrabberLeft,
        GrabberRight,
        GrabberTopLeft,
        GrabberTopRight,
        GrabberBottomLeft,
        GrabberBottomRight
    };

    QPointF previousPosition() const;
    void setPreviousPosition(const QPointF previousPosition);

    void setRect(qreal x, qreal y, qreal w, qreal h);
    void setRect(const QRectF &rect);
    void handleSignal(int type, void* value) override;

signals:
    void rectChanged(VERectangle *rect);
    void previousPositionChanged();
    void clicked(VERectangle *rect);
    void signalMove(QGraphicsItem *item, qreal dx, qreal dy);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    unsigned int m_cornerFlags;
    unsigned int m_actionFlags;
    QPointF m_previousPosition;
    bool m_leftMouseButtonPressed;
    DotSignal *cornerGrabber[8];

    void resizeLeft( const QPointF &pt);
    void resizeRight( const QPointF &pt);
    void resizeBottom(const QPointF &pt);
    void resizeTop(const QPointF &pt);

    void rotateItem(const QPointF &pt);
    void setPositionGrabbers();
    void setVisibilityGrabbers();
    void hideGrabbers();

};


class VERectangle;

namespace Ui {
class rectangle;
}

class rectangle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color_1 READ color_1 WRITE setColor_1 NOTIFY color_1Changed)
    Q_PROPERTY(QColor color_2 READ color_2 WRITE setColor_2 NOTIFY color_2Changed)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
public:
    explicit rectangle(QWidget *parent = 0);
    ~rectangle();
    QColor color_1() const;
    QColor color_2() const;
    QColor borderColor() const;
    int borderWidth() const;
public slots:
    void setColor_1(const QColor &color);
    void setColor_2(const QColor &color);
    void setBorderColor(const QColor &color);
    void setBorderWidth(const int &width);
signals:
    void color_1Changed(const QColor &color);
    void color_2Changed(const QColor &color);
    void borderColorChanged(const QColor &color);
    void borderWidthChanged(const int &width);
private:
    Ui::rectangle *ui;
    VERectangle *currentRectangle;
public slots:
    void newRectangle(VERectangle *rect);
    void loadRectangle(VERectangle *rect);
    void deselect();
    void setVisible(bool visible) override;
private slots:
    void comboBoxIndexChanged(int index);
private:
    void setGradient(VERectangle *rectangle);
private:
    QColor m_color_1;
    QColor m_color_2;
    QColor m_borderColor;
    int m_borderWidth;
};


#endif // RECTANGLE_H
