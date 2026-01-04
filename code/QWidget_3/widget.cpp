#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_up_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    qDebug() << rect;
    // 1. 这样的其实是在移动左上角的点，宽度和高度也会改变
//    rect.setY(rect.y() - 5);
//    ui->pushButton_target->setGeometry(rect);
    // 2. 只改变位置,不改变大小
    ui->pushButton_target->setGeometry(rect.x(), rect.y() - 5, rect.width(), rect.height());
}

void Widget::on_pushButton_down_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    qDebug() << rect;
//    rect.setY(rect.y() + 5);
//    ui->pushButton_target->setGeometry(rect);
    ui->pushButton_target->setGeometry(rect.x(), rect.y() + 5, rect.width(), rect.height());
}

void Widget::on_pushButton_left_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    qDebug() << rect;
//    rect.setX(rect.x() - 5);
//    ui->pushButton_target->setGeometry(rect);
    ui->pushButton_target->setGeometry(rect.x() - 5, rect.y(), rect.width(), rect.height());
}

void Widget::on_pushButton_right_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    qDebug() << rect;
//    rect.setX(rect.x() + 5);
//    ui->pushButton_target->setGeometry(rect);
    ui->pushButton_target->setGeometry(rect.x() + 5, rect.y(), rect.width(), rect.height());
}
