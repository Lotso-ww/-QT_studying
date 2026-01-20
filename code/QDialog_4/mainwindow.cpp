#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <dialog.h>

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
    Dialog* dialog = new Dialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    // dialog->show(); // 非模态对话框
    dialog->exec(); // 模态对话框
}
