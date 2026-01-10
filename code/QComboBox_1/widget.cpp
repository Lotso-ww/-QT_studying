#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->comboBox->addItem("麦辣鸡腿堡");
    ui->comboBox->addItem("培根芝士双牛堡");
    ui->comboBox->addItem("板烧鸡腿堡");

    ui->comboBox_2->addItem("中薯");
    ui->comboBox_2->addItem("麦辣鸡翅");
    ui->comboBox_2->addItem("麦乐鸡块");

    ui->comboBox_3->addItem("可乐");
    ui->comboBox_3->addItem("雪碧");
    ui->comboBox_3->addItem("怡泉");
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    qDebug() << "汉堡: " << ui->comboBox->currentText()
             << "小食: " << ui->comboBox_2->currentText()
             << "饮料: " << ui->comboBox_3->currentText();
}
