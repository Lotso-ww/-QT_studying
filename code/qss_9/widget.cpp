#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QPushButton* button = new QPushButton("按钮", this);
    button->setGeometry(0, 0, 100, 100);
    QString style = "QPushButton { border: 5px solid red; margin: 20px; }";
    button->setStyleSheet(style);
    qDebug() << button->geometry();
}

Widget::~Widget()
{
    delete ui;
}

