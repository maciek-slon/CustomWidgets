#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MatrixWidget.hpp"

#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MatrixWidget * mat = new MatrixWidget;
    QPushButton * btnH = new QPushButton("Flip H");
    QPushButton * btnV = new QPushButton("Flip V");
    QPushButton * btnT = new QPushButton("Transpose");
    QPushButton * btnA = new QPushButton("Anti Transpose");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(mat);
    layout->addWidget(btnH);
    layout->addWidget(btnV);
    layout->addWidget(btnT);
    layout->addWidget(btnA);


    connect(btnH, SIGNAL(clicked()), mat, SLOT(flipH()));
    connect(btnV, SIGNAL(clicked()), mat, SLOT(flipV()));
    connect(btnT, SIGNAL(clicked()), mat, SLOT(transpose()));
    connect(btnA, SIGNAL(clicked()), mat, SLOT(antiTranspose()));

    ui->centralWidget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
