#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
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
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("对话框");
    dialog->resize(400, 300);
    dialog->show(); // 展示对话框

    // 注意内存泄漏,Qt给我们提供了设置属性解决这个问题的方法
    dialog->setAttribute(Qt::WA_DeleteOnClose);
}
