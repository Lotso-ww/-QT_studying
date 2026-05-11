#include "widget.h"
#include "ui_widget.h"
#include "thread.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 创建两个线程并且启动 -> 去执行run函数
    Thread thread1;
    Thread thread2;
    thread1.start();
    thread2.start();

    // 主线程需要去等待这两个子线程
    thread1.wait();
    thread2.wait();

    qDebug() << Thread::num;
}

Widget::~Widget()
{
    delete ui;
}

