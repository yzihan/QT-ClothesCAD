#ifndef VEELLIPSE_H
#define VEELLIPSE_H

#include <QWidget>
#include <QObject>
#include <QGraphicsEllipseItem>
#include <ellchange.h>
#include <QWidget>

class DotSignal;
class QGraphicsSceneMouseEvent;

class VEEllipse : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

public:
    explicit VEEllipse(QObject * parent = 0);
    ~VEEllipse();

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

    void setElli(qreal x, qreal y, qreal w, qreal h);
    void setElli(const QRectF &rect);

signals:
    void elliChanged(VEEllipse *rect);
    void previousPositionChanged();
    void clicked(VEEllipse *rect);
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

    ellchange ed;
};

#include <ui_ellipse.h>

namespace Ui {
using VEEllipseSettings=Ui::ellipse;
}

class VEEllipseSettings : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color_1 READ color_1 WRITE setColor_1 NOTIFY color_1Changed)
    Q_PROPERTY(QColor color_2 READ color_2 WRITE setColor_2 NOTIFY color_2Changed)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)

public:
    explicit VEEllipseSettings(QWidget *parent = 0);
    ~VEEllipseSettings();

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
    Ui::VEEllipseSettings *ui;
    VEEllipse *currentEllipse;

public slots:
    void newEllipse(VEEllipse *elli);
    void loadEllipse(VEEllipse *elli);
    void deselect();
    void setVisible(bool visible) override;

private slots:
    void comboBoxIndexChanged(int index);

private:
    void setGradient(VEEllipse *ellipse);

private:
    QColor m_color_1;
    QColor m_color_2;
    QColor m_borderColor;
    int m_borderWidth;
};

#endif // VEELLIPSE_H
