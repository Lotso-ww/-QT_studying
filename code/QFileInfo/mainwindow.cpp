#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>

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


void MainWindow::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this);
    QFileInfo fileInfo(path);
    qDebug() << fileInfo.fileName();
    qDebug() << fileInfo.suffix();
    qDebug() << fileInfo.path();
    qDebug() << fileInfo.size();
    qDebug() << fileInfo.isFile();
    qDebug() << fileInfo.isDir();

    // …………
}
