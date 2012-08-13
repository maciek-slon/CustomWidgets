#include "CurvesWidget.hpp"
#include <QtCore>
#include <QtGui>

#include <iostream>

CurvesWidget::CurvesWidget(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(256, 256);

    m_points.push_back(QPointF(0.2,0));
    m_points.push_back(QPointF(1,1));
    m_points.push_back(QPointF(0.5,0.5));

    m_closest = NULL;
    m_grab = false;

    m_spline.setLowBC(Spline::FIXED_1ST_DERIV_BC);
}

CurvesWidget::~CurvesWidget()
{
}

void CurvesWidget::paintBackground(QPainter & painter)
{
    painter.save();
    QBrush brush(Qt::white);
    painter.fillRect(rect(), brush);

    painter.setPen(QColor(192, 192, 192));

    QVector<QPoint> pairs;

    int maxw = width() - 1;
    int maxh = height() - 1;
    for (int i = 0; i < 8; ++i) {
        pairs.push_back(QPoint(i * width() / 8, 0));
        pairs.push_back(QPoint(i * width() / 8, maxh));

        pairs.push_back(QPoint(0, i * height() / 8));
        pairs.push_back(QPoint(maxw, i * height() / 8));
    }

    pairs.push_back(QPoint(maxw, 0));
    pairs.push_back(QPoint(maxw, maxh));

    pairs.push_back(QPoint(0, maxh));
    pairs.push_back(QPoint(maxw, maxh));

    painter.drawLines(pairs);
    painter.restore();
}

void CurvesWidget::paintControlPoints(QPainter & painter)
{
    painter.save();
    Q_FOREACH(QPointF pt, m_points) {
        painter.drawEllipse(pt.x() * width() - 5, (1 - pt.y()) * height() - 5, 10.0, 10.0);
    }

    if (m_closest) {
        QBrush brush(Qt::black);
        painter.setBrush(brush);
        painter.drawEllipse(m_closest->x() * width() - 5, (1 - m_closest->y()) * height() - 5, 10.0, 10.0);
    }

    painter.restore();
}

void CurvesWidget::paintSpline(QPainter &painter)
{
    painter.save();
    QPen pen;
    pen.setWidthF(2);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    m_spline.clear();
    Q_FOREACH (QPointF pt, m_points) {
        m_spline.addPoint(pt.x(), pt.y());
    }

    float lx = 0, ly = m_spline(0);

    for (int i = 1; i < 256; ++i) {
        float x = 1.0 * i / 255;
        float y = m_spline(x);
        painter.drawLine(QPointF(width() * lx, height() * (1-ly)), QPointF(width() * x, height() * (1-y)));
        lx = x;
        ly = y;
    }
    painter.restore();
}

void CurvesWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    paintBackground(painter);
    paintControlPoints(painter);
    paintSpline(painter);
}

void CurvesWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_grab) {
        m_closest = findClosestPoint(event->pos());
    } else {
        *m_closest = project(event->pos());
    }

    repaint();
}

QPointF * CurvesWidget::findClosestPoint(QPoint pt)
{
    QPointF * ret = NULL;
    int len, min_len = 30;
    for (int i = 0; i < m_points.size(); ++i) {
        QPointF p = m_points[i];
        len = (QPoint(width() * p.x(), height() * (1-p.y())) - pt).manhattanLength();
        if (len < min_len) {
            min_len = len;
            ret = &m_points[i];
        }
    }
    return ret;
}

void CurvesWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_closest) {
        m_points.push_back(QPointF(1.0 * event->pos().x() / width(), 1.0 - 1.0 * event->pos().y() / height()));
        m_closest = &m_points[m_points.size() - 1];
    }

    m_grab = true;

    repaint();
}

QPointF CurvesWidget::project(QPoint pt)
{
    if (pt.x() < 0) pt.setX(0);
    if (pt.x() > width()) pt.setX(width());
    if (pt.y() < 0) pt.setY(0);
    if (pt.y() > height()) pt.setY(height());

    return QPointF(1.0 * pt.x() / width(), 1.0 - 1.0 * pt.y() / height());
}

QPoint CurvesWidget::unproject(QPointF pt)
{
    return QPoint(pt.x() * width(), (1.0 - pt.y()) * height());
}

void CurvesWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_grab = false;
}
