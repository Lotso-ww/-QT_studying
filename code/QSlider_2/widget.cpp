#include "widget.h"
#include "ui_widget.h"
#include <QShortcut>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 使用快捷键
    QShortcut* shortcut1 = new QShortcut(this);
    shortcut1->setKey(QKeySequence("-"));
    QShortcut* shortcut2 = new QShortcut(this);
    shortcut2->setKey(QKeySequence("="));

    // 使用信号槽,感知到快捷键被按下
    connect(shortcut1, &QShortcut::activated, this, &Widget::subValue);
    connect(shortcut2, &QShortcut::activated, this, &Widget::addValue);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::subValue()
{
    int value = ui->horizontalSlider->value();
    if(value <= ui->horizontalSlider->minimum()) return;
    ui->horizontalSlider->setValue(value - 5);
}

void Widget::addValue()
{
    int value = ui->horizontalSlider->value();
    if(value >= ui->horizontalSlider->maximum()) return;
    ui->horizontalSlider->setValue(value + 5);
}


void Widget::on_horizontalSlider_valueChanged(int value)
{
    ui->label->setText("当前的值为 " + QString::number(value));
}
