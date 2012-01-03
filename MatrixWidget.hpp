#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QWidget>

class QTableWidget;

class MatrixWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MatrixWidget(QWidget *parent = 0);


protected:
    virtual void resizeEvent(QResizeEvent* event);

signals:

public slots:

    void setZero();
    void setEye();
    void setOnes();

    void flipV();
    void flipH();
    void transpose();
    void antiTranspose();

private:
    QTableWidget * m_table;

    void recalculateSize();
};

#endif // MATRIXWIDGET_H
