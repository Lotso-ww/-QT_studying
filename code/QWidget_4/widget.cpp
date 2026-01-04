#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QRect rect1 = this->geometry();
    QRect rect2 = this->frameGeometry();

    qDebug() << rect1;
    qDebug() << rect2;
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    QRect rect1 = this->geometry();
    QRect rect2 = this->frameGeometry();

    qDebug() << rect1;
    qDebug() << rect2;
}
