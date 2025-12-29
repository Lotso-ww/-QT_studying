#include "widget.h"
#include "ui_widget.h"

#include "QLabel"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 推荐申请在堆上
    QLabel* label = new QLabel(this);
    // 栈上
    // QLabel label;

    label->setText("hello world");

}

Widget::~Widget()
{
    delete ui;
}

