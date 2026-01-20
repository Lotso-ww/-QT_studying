#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 打开或者保存的功能还需要额外实现，这里先不讲
void MainWindow::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this);
    qDebug() << filePath;

}

void MainWindow::on_pushButton_2_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this);
    qDebug() << filePath;
}
