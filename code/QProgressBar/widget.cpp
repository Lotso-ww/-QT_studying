#include "widget.h"
#include "ui_widget.h"
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::handle);
    // 启动定时器
    timer->start(100);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::handle()
{
    // 获取当前进度条的数值
    int value = ui->progressBar->value();
    // 到了100就停止
    if(value >= 100)
    {
        timer->stop();
        return;
    }

    // 进度条数值的改变
    ui->progressBar->setValue(value + 1);
}

