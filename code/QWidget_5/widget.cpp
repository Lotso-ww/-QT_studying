#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("窗口标题");

    QPushButton* button = new QPushButton(this);
    button->setText("按钮");
    // 按钮设置窗口标题肯定是不能这样写的，但是这里没报错
    button->setWindowTitle("按钮设置窗口标题");
}

Widget::~Widget()
{
    delete ui;
}

