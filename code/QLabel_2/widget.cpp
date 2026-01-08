#include "widget.h"
#include "ui_widget.h"
#include <QResizeEvent>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QRect rect = this->geometry();
    ui->label->setGeometry(0, 0, rect.width(), rect.height());

    QPixmap pixmap(":/code.png");
    ui->label->setPixmap(pixmap);
    ui->label->setScaledContents(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::resizeEvent(QResizeEvent *event)
{
    qDebug() << event->size();
    ui->label->setGeometry(0, 0, event->size().width(), event->size().height());
}

