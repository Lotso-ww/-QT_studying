#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->pushButton_Yes->setToolTip("这是一个Yes按钮!");
    ui->pushButton_Yes->setToolTipDuration(3000);
    ui->pushButton_No->setToolTip("这是一个NO按钮");
    ui->pushButton_No->setToolTipDuration(7000);
}

Widget::~Widget()
{
    delete ui;
}

