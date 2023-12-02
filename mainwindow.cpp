#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->armWidget->setSegment1Length(10);
    ui->armWidget->setSegment2Length(10);
    ui->armWidget->setTargetCoord({15, 5});
}

MainWindow::~MainWindow()
{
    delete ui;
}
