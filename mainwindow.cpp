#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MatrixWidget.hpp"
#include "CurvesEditor.hpp"
#include "ctkDoubleSlider.hpp"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget * tab;

    // MatrixWidget
    tab = new QWidget;

    MatrixWidget * mat = new MatrixWidget;

    QPushButton * btnH = new QPushButton("Flip H");
    connect(btnH, SIGNAL(clicked()), mat, SLOT(flipH()));

    QPushButton * btnV = new QPushButton("Flip V");
    connect(btnV, SIGNAL(clicked()), mat, SLOT(flipV()));

    QPushButton * btnT = new QPushButton("Transpose");
    connect(btnT, SIGNAL(clicked()), mat, SLOT(transpose()));

    QPushButton * btnA = new QPushButton("Anti Transpose");
    connect(btnA, SIGNAL(clicked()), mat, SLOT(antiTranspose()));

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(mat);
    layout->addWidget(btnH);
    layout->addWidget(btnV);
    layout->addWidget(btnT);
    layout->addWidget(btnA);


    tab->setLayout(layout);

    ui->tabWidget->addTab(tab, "MatrixWidget");

    // CurvesEditor
    tab = new QWidget;

    CurvesEditor * cw = new CurvesEditor;

    QComboBox * cb = new QComboBox();
    cb->addItem("Linear");
    cb->addItem("Cubic");
    connect(cb, SIGNAL(currentIndexChanged(int)), cw, SLOT(setInterpolation(int)));

    ctkDoubleSlider * ds = new ctkDoubleSlider;
    ds->setMinimum(0);
    ds->setMaximum(0.5);
    ds->setSingleStep(0.01);
    ds->setOrientation(Qt::Horizontal);
    connect(ds, SIGNAL(valueChanged(double)), cw, SLOT(setSnapRange(double)));

    QSpinBox * sb = new QSpinBox;
    sb->setMinimum(1);
    sb->setMaximum(16);
    connect(sb, SIGNAL(valueChanged(int)), cw, SLOT(setGridSize(int)));

    QPushButton * btnCopy = new QPushButton("Copy to clipboard");
    connect(btnCopy, SIGNAL(clicked()), cw, SLOT(copyToClipboard()));

    QGridLayout* layout2 = new QGridLayout;
    layout2->addWidget(cw, 0, 0, 1, 2);
    layout2->addWidget(new QLabel("Interpolation"), 1, 0);
    layout2->addWidget(cb, 1, 1);
    layout2->addWidget(new QLabel("Snap range"), 2, 0);
    layout2->addWidget(ds, 2, 1);
    layout2->addWidget(new QLabel("Grid size"), 3, 0);
    layout2->addWidget(sb, 3, 1);
    layout2->addWidget(btnCopy, 4, 0, 1, 2);

    tab->setLayout(layout2);

    ui->tabWidget->addTab(tab, "CurvesWidget");
}

MainWindow::~MainWindow()
{
    delete ui;
}
