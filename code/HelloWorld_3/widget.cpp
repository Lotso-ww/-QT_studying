#include "widget.h"
#include "ui_widget.h"
#include <QLineEdit>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QLineEdit* edit = new QLineEdit(this);
    edit->setText("Hello world！");
}

Widget::~Widget()
{
    delete ui;
}

