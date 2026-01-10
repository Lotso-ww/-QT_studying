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


void Widget::on_pushButton_clicked()
{
    // 1.先获取到这两个输入框的时间
    QDateTime timeOld = ui->dateTimeEdit->dateTime();
    QDateTime timeNew = ui->dateTimeEdit_2->dateTime();
    qDebug() << timeOld << timeNew;

    // 2.计算日期的差值
    //int days = timeOld.daysTo(timeNew);
    int seconds = timeOld.secsTo(timeNew);

    // 3.要把秒数换算成小时和天数
    int hours = (seconds / 3600) % 24;
    int days = (seconds / 3600) / 24;

    // 4.把计算结果放到 label 中
    ui->label->setText(QString("距离生日已经过去了 ") + QString::number(days)
                       + QString(" 天零 ") + QString::number(hours) + QString(" 小时"));
}
