#include "CurvesEditor.hpp"
#include <QtCore>
#include <QtGui>

#include <iostream>

CurvesEditor::CurvesEditor(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(256, 256);

    m_points.push_back(QPointF(0.2,0));
    m_points.push_back(QPointF(1,1));
    m_points.push_back(QPointF(0.5,0.5));

    m_closest = NULL;
    m_grab = false;

    m_spline.setType(Spline::LINEAR);

    m_grid_size = 8;
    m_snap_rng = 0.2;
}

CurvesEditor::~CurvesEditor()
{
}

void CurvesEditor::setInterpolation(int method)
{
    switch(method) {
    case 0: m_spline.setType(Spline::LINEAR); break;
    case 1: m_spline.setType(Spline::CUBIC); break;
    default: break;
    }

    repaint();
}

void CurvesEditor::setSnapRange(double range)
{
    m_snap_rng = range;
}

void CurvesEditor::setGridSize(int size)
{
    if (size < 1) return;

    m_grid_size = size;
    repaint();
}

void CurvesEditor::paintBackground(QPainter & painter)
{
    painter.save();
    QBrush brush(Qt::white);
    painter.fillRect(rect(), brush);

    painter.setPen(QColor(192, 192, 192));

    QVector<QPoint> pairs;

    int maxw = width() - 1;
    int maxh = height() - 1;
    for (int i = 0; i < m_grid_size; ++i) {
        pairs.push_back(QPoint(i * width() / m_grid_size, 0));
        pairs.push_back(QPoint(i * width() / m_grid_size, maxh));

        pairs.push_back(QPoint(0, i * height() / m_grid_size));
        pairs.push_back(QPoint(maxw, i * height() / m_grid_size));
    }

    pairs.push_back(QPoint(maxw, 0));
    pairs.push_back(QPoint(maxw, maxh));

    pairs.push_back(QPoint(0, maxh));
    pairs.push_back(QPoint(maxw, maxh));

    painter.drawLines(pairs);
    painter.restore();
}

void CurvesEditor::paintControlPoints(QPainter & painter)
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

void CurvesEditor::paintSpline(QPainter &painter)
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
        y = y < 0 ? 0 : y > 1 ? 1 : y;
        painter.drawLine(QPointF(width() * lx, height() * (1-ly)), QPointF(width() * x, height() * (1-y)));
        lx = x;
        ly = y;
    }
    painter.restore();
}

void CurvesEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    paintBackground(painter);
    paintControlPoints(painter);
    paintSpline(painter);
}

void CurvesEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_grab) {
        m_closest = findClosestPoint(event->pos(), m_closest_id);
        if (m_closest)
            setCursor(Qt::CrossCursor);
        else
            setCursor(Qt::ArrowCursor);
    } else {
        *m_closest = snap(project(event->pos()));
    }

    repaint();
}

QPointF * CurvesEditor::findClosestPoint(QPoint pt, int & id)
{
    QPointF * ret = NULL;
    int len, min_len = 30;
    for (int i = 0; i < m_points.size(); ++i) {
        QPointF p = m_points[i];
        len = (QPoint(width() * p.x(), height() * (1-p.y())) - pt).manhattanLength();
        if (len < min_len) {
            min_len = len;
            ret = &m_points[i];
            id = i;
        }
    }
    return ret;
}

void CurvesEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_closest) {
            m_points.push_back(QPointF(1.0 * event->pos().x() / width(), 1.0 - 1.0 * event->pos().y() / height()));
            m_closest = &m_points[m_points.size() - 1];
            m_closest_id = m_points.size() - 1;
        }

        m_grab = true;
    } else if (event->button() == Qt::RightButton) {
        if (m_closest && m_points.size() > 2) {
            m_points.remove(m_closest_id);
            m_closest = NULL;
        }
    }

    repaint();
}

QPointF CurvesEditor::project(QPoint pt)
{
    if (pt.x() < 0) pt.setX(0);
    if (pt.x() > width()) pt.setX(width());
    if (pt.y() < 0) pt.setY(0);
    if (pt.y() > height()) pt.setY(height());

    return QPointF(1.0 * pt.x() / width(), 1.0 - 1.0 * pt.y() / height());
}

float fsnap(float f, float rng, float acc) {
    float rem = fmod(f, rng);
    if (rem < acc * rng)
        return f - rem;
    if (rem > (1-acc) * rng)
        return f + rng - rem;
    return f;
}

QPointF CurvesEditor::snap(QPointF pt) {
    float rng = 1./m_grid_size;
    pt.setX(fsnap(pt.x(), rng, m_snap_rng));
    pt.setY(fsnap(pt.y(), rng, m_snap_rng));
    return pt;
}

QPoint CurvesEditor::unproject(QPointF pt)
{
    return QPoint(pt.x() * width(), (1.0 - pt.y()) * height());
}

void CurvesEditor::mouseReleaseEvent(QMouseEvent *event)
{
    m_grab = false;
}

void CurvesEditor::copyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString out = "unsigned char LUT[256] = {";
    QString sep = "";
    for (int i = 0; i < 256; ++i) {
        float x = 1.0 * i / 255;
        float y = m_spline(x);
        out += sep + QString("%1").arg((int)(y * 255));
        sep = ", ";
    }
    out += "};";
    clipboard->setText(out);
}
