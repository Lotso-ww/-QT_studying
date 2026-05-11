#include "widget.h"
#include "ui_widget.h"
#include "thread.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 关联信号槽
    connect(&thread, &Thread::notify, this, &Widget::handle);
    // 启动线程
    thread.start();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::handle()
{
    // 我们在这个里面进行处理
    int value = ui->lcdNumber->intValue();
    value--;
    ui->lcdNumber->display(value);
}

