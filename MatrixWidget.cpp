#include "MatrixWidget.hpp"

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>
#include <QtGui>
#include <QTableWidget>

class SpinBoxDelegate : public QItemDelegate
{
public:
    SpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(-(INT_MAX-1));
    editor->setMaximum(INT_MAX);

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}



MatrixWidget::MatrixWidget(QWidget *parent) :
    QWidget(parent)
{
    m_table = new QTableWidget(4, 4);

    SpinBoxDelegate * delegate = new SpinBoxDelegate;
    m_table->setItemDelegate(delegate);

    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setStretchLastSection(true);

    m_table->setSelectionMode(QAbstractItemView::NoSelection);

    // Hide headers
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->hide();

    // Disable scrollBars
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    // Disable the frame by default
    //m_table->setFrameStyle(QFrame::NoFrame);



    // Define prototype item
    QTableWidgetItem* _item = new QTableWidgetItem;
    _item->setData(Qt::DisplayRole, QVariant(0.0));
    _item->setTextAlignment(Qt::AlignCenter);

    // The table takes ownership of the prototype.
    m_table->setItemPrototype(_item);
    m_table->setEditTriggers(QTableWidget::DoubleClicked);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_table);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);

    for (int r = 0; r < m_table->rowCount(); ++r)
        for (int c = 0; c < m_table->columnCount(); ++c)
        {
            QTableWidgetItem* item = m_table->item(r, c);
            if (!item)
            {
                m_table->setItem(r, c , m_table->itemPrototype()->clone());
            }
        }

    setEye();
}

void MatrixWidget::setZero()
{
    for (int r = 0; r < m_table->rowCount(); ++r)
        for (int c = 0; c < m_table->columnCount(); ++c)
            m_table->item(r, c)->setData(Qt::DisplayRole, QVariant(0));

}

void MatrixWidget::setOnes()
{
    for (int r = 0; r < m_table->rowCount(); ++r)
        for (int c = 0; c < m_table->columnCount(); ++c)
            m_table->item(r, c)->setData(Qt::DisplayRole, QVariant(1));

}

void MatrixWidget::setEye()
{
    int newValue;
    for (int r = 0; r < m_table->rowCount(); ++r)
        for (int c = 0; c < m_table->columnCount(); ++c) {
            newValue = r == c ? 1 : 0;
            m_table->item(r, c)->setData(Qt::DisplayRole, QVariant(newValue));
        }
}

void MatrixWidget::flipV()
{
    QVariant val;
    int rc = m_table->rowCount() - 1;
    for (int r = 0; r <= rc / 2; ++r)
        for (int c = 0; c < m_table->columnCount(); ++c) {
            val = m_table->item(r, c)->data(Qt::DisplayRole);
            m_table->item(r, c)->setData(Qt::DisplayRole, m_table->item(rc - r, c)->data(Qt::DisplayRole));
            m_table->item(rc - r, c)->setData(Qt::DisplayRole, val);
        }
}

void MatrixWidget::flipH()
{
    QVariant val;
    int cc = m_table->columnCount() - 1;
    for (int r = 0; r < m_table->rowCount(); ++r)
        for (int c = 0; c <= cc / 2; ++c) {
            val = m_table->item(r, c)->data(Qt::DisplayRole);
            m_table->item(r, c)->setData(Qt::DisplayRole, m_table->item(r, cc - c)->data(Qt::DisplayRole));
            m_table->item(r, cc - c)->setData(Qt::DisplayRole, val);
        }
}

void MatrixWidget::transpose()
{
    int cc = m_table->columnCount();
    int rr = m_table->rowCount();

    // transpose only works for square matrices
    if (cc != rr)
        return;

    QVariant val;
    for (int r = 0; r < rr; ++r)
        for (int c = r+1; c < cc ; ++c) {
            val = m_table->item(r, c)->data(Qt::DisplayRole);
            m_table->item(r, c)->setData(Qt::DisplayRole, m_table->item(c, r)->data(Qt::DisplayRole));
            m_table->item(c, r)->setData(Qt::DisplayRole, val);
        }
}

void MatrixWidget::antiTranspose()
{
    int cc = m_table->columnCount();
    int rr = m_table->rowCount();

    // transpose only works for square matrices
    if (cc != rr)
        return;

    QVariant val;
    for (int r = 0; r < rr; ++r)
        for (int c = 0; c < cc-r-1 ; ++c) {
            val = m_table->item(r, c)->data(Qt::DisplayRole);
            m_table->item(r, c)->setData(Qt::DisplayRole, m_table->item(cc-c-1, rr-r-1)->data(Qt::DisplayRole));
            m_table->item(cc-c-1, rr-r-1)->setData(Qt::DisplayRole, val);
        }
}

void MatrixWidget::recalculateSize() {
    QSize viewportSize = this->size();

    // columns
    const int ccount = m_table->columnCount();
    int colWidth = viewportSize.width() / ccount;
    for (int j=0; j < ccount - 1; j++)
    {
        m_table->setColumnWidth(j, colWidth);
    }

    // rows
    const int rcount = m_table->rowCount();
    int rowHeight = viewportSize.height() / rcount;
    for (int i=0; i < rcount; i++)
    {
        m_table->setRowHeight(i, rowHeight);
    }
    m_table->updateGeometry();
}

void MatrixWidget::resizeEvent(QResizeEvent *event)
{
    recalculateSize();
}
