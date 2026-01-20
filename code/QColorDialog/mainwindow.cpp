#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>

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
    QColor color = QColorDialog::getColor(QColor(0, 255, 0), this, "选择颜色");
    // 可以通过用户选择的颜色来修改窗口背景色
    // QString style = "background-color: rgb(" + QString::number(color.red()) + ", " + QString::number(color.green())
    //        + ", " + QString::number(color.blue()) + ");";


    char style[1024] = {0};
    sprintf(style, "background-color: rgb(%d, %d, %d)", color.red(), color.green(), color.blue());
    this->setStyleSheet(style);
}
