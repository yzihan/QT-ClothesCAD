#ifndef VEPOLYLINE_H
#define VEPOLYLINE_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QWidget>

class DotSignal;
class QGraphicsSceneMouseEvent;

class VEPolyline : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

public:
    explicit VEPolyline(QObject *parent = 0);
    ~VEPolyline();

    QPointF previousPosition() const;
    void setPreviousPosition(const QPointF previousPosition);
    void setPath(const QPainterPath &path);

signals:
    void previousPositionChanged();
    void clicked(VEPolyline *rect);
    void signalMove(QGraphicsItem *item, qreal dx, qreal dy);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

public slots:

private slots:
    void slotMove(QGraphicsItem *signalOwner, qreal dx, qreal dy);
    void checkForDeletePoints();

private:
    QPointF m_previousPosition;
    bool m_leftMouseButtonPressed;
    QList<DotSignal *> listDotes;
    int m_pointForCheck;

public:
    void updateDots();
};



class VEPolyline;

#include <ui_polyline.h>

namespace Ui {
class polyline;
}

class polyline : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

public:
    explicit polyline(QWidget *parent = 0);
    ~ polyline();

    QColor lineColor() const;
    int lineWidth() const;

public slots:
    void setLineColor(const QColor &color);
    void setLineWidth(const int &width);

signals:
    void lineColorChanged(const QColor &color);
    void lineWidthChanged(const int &width);

private:
    Ui:: polyline *ui;
    VEPolyline *currentPolyline;

public slots:
    void newPolyline(VEPolyline *polyline);
    void loadPolyline(VEPolyline *polyline);
    void deselect();
    void setVisible(bool visible);

private:
    QColor m_lineColor;
    int m_lineWidth;
};

#endif // VEPOLYLINE_H
