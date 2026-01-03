#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(this, &Widget::mySignal, this, &Widget::handleMySignal);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::handleMySignal(const QString& text)
{
    this->setWindowTitle(text);
}



void Widget::on_pushButton_1_clicked()
{
    // 发送出自己定义的信号
    // 发送信号的操作,也可以在任意合适的代码中,不一定非要在构造函数里

    // 此时就是点击按钮的时候，发送自定义信号
    emit mySignal("1");
}

void Widget::on_pushButton_2_clicked()
{
    emit mySignal("2");
}
