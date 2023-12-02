#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->armWidget->setSegment1Length(350);
    ui->armWidget->setSegment2Length(350);
    ui->armWidget->setSegment3Length(100);
    ui->armWidget->setLink3Yaw(0);
    ui->armWidget->setTargetCoord({0, 0});
}

MainWindow::~MainWindow()
{
    delete ui;
}
