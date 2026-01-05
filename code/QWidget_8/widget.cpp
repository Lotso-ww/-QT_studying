#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QCursor cursor(Qt::OpenHandCursor);
    ui->pushButton->setCursor(cursor);
}

Widget::~Widget()
{
    delete ui;
}

