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

public slots:
    void setInterpolation(int method);
    void setSnapRange(double range);
    void setGridSize(int size);

protected:
    void paintBackground(QPainter & painter);
    void paintControlPoints(QPainter & painter);
    void paintSpline(QPainter & painter);
    void paintEvent(QPaintEvent *event);

    QPointF * findClosestPoint(QPoint pt, int & id);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QPointF project(QPoint pt);
    QPointF snap(QPointF pt);
    QPoint unproject(QPointF pt);

private:
    QVector<QPointF> m_points;

    Spline m_spline;

    QPointF * m_closest;
    int m_closest_id;
    bool m_grab;

    int m_grid_size;
    float m_snap_rng;
};

#endif // CURVESWIDGET_H
