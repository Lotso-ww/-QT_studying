#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPushButton>
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


void MainWindow::on_pushButton_clicked()
{
    QMessageBox* message = new QMessageBox(this);
    message->setWindowTitle("对话框标题");
    message->setText("这是对话框文本");
    message->setIcon(QMessageBox::Warning);
    message->setStandardButtons(QMessageBox::Ok | QMessageBox::Save | QMessageBox::Cancel);

    // 添加自定义按钮,可以关联信号槽完成一些操作
    // QPushButton* button = new QPushButton("按钮", message);
    // message->addButton(button, QMessageBox::AcceptRole);

    // 非模态对话框，在窗口关闭之前会阻塞在这里
    // 我们点击按钮窗口关闭之后就可以通过exec的返回值，来知道用户点击的是那个按钮，然后去执行一些对应逻辑了
    int result = message->exec();
    if(result == QMessageBox::Ok){
        qDebug() << "OK";
    }else if(result == QMessageBox::Save){
        qDebug() << "Save";
    }else if(result == QMessageBox::Cancel){
        qDebug() << "Cancel";
    }

    message->setAttribute(Qt::WA_DeleteOnClose);
    // delete message; // 所以这样也可以
}
