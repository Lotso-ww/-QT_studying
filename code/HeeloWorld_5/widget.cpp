#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->myButton, &QPushButton::clicked, this, &Widget::handleClick);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::handleClick()
{
    if(ui->myButton->text() == QString("Hello World")){
        ui->myButton->setText("Hello Qt");
    }
    else{
        ui->myButton->setText("Heelo World");
    }
}
