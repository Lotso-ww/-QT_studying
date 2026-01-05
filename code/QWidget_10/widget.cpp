#include "widget.h"
#include "ui_widget.h"
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{//
    ui->setupUi(this);

    QLabel* label = new QLabel(this);
    label->setText("这是一个文本!");

    QFont front;
    front.setFamily("华文中宋");
    front.setPixelSize(20);
    front.setBold(true);
    front.setItalic(true);
    front.setWeight(true);
    front.setUnderline(true);
    front.setStrikeOut(true);

    label->setFont(front);
}

Widget::~Widget()
{
    delete ui;
}

