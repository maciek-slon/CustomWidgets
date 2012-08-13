#ifndef CURVESWIDGET_H
#define CURVESWIDGET_H

#include <QWidget>
#include "spline.hpp"

class QPainter;

using namespace magnet::math;

class CurvesWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CurvesWidget(QWidget *parent = 0);
    ~CurvesWidget();

protected:
    void paintBackground(QPainter & painter);
    void paintControlPoints(QPainter & painter);
    void paintSpline(QPainter & painter);
    void paintEvent(QPaintEvent *event);

    QPointF * findClosestPoint(QPoint pt);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QPointF project(QPoint pt);
    QPoint unproject(QPointF pt);

private:
    QVector<QPointF> m_points;

    Spline m_spline;

    QPointF * m_closest;
    bool m_grab;
};

#endif // CURVESWIDGET_H
