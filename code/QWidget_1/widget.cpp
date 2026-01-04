#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QPushButton* button = new QPushButton(this);
    button->setText("按钮");
    button->isEnabled(); // 获取控件可用状态
    button->setEnabled(false); // 禁用按钮
}

Widget::~Widget()
{
    delete ui;
}

