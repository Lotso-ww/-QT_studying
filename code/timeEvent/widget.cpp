#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 开启定时器事件
    // 此处的 timeId 是一个定时器的身份标识
    timeId = this->startTimer(1000);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::timerEvent(QTimerEvent *event)
{
    // 如果一个程序中存在多个定时器 (startTimer 创建的定时器), 此时每个定时器都会触发 timeEvent 函数
    // 先判断一下这次触发的是否是想要的定时器触发的
    if(event->timerId() != this->timeId)
    {
        // 如果不是我们的定时器触发的,就直接忽略
        return;
    }
    // 是我们自己搞的定时器
    int value = ui->lcdNumber->intValue();
    if(value <= 0)
    {
        // 停止定时器
        this->killTimer(this->timeId);
        return;
    }
    value -= 1;
    ui->lcdNumber->display(value);
}

