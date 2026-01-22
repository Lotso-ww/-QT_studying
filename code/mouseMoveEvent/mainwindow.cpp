#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMouseTracking(true); // 设置为 true 才能追踪鼠标移动
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug() << ev->x() << ',' << ev->y();
}

void MainWindow::wheelEvent(QWheelEvent *ev)
{

}
