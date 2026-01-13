#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QGridLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QPushButton* btn1 = new QPushButton("按钮1");
    QPushButton* btn2 = new QPushButton("按钮2");
    QPushButton* btn3 = new QPushButton("按钮3");
    QPushButton* btn4 = new QPushButton("按钮4");


    QGridLayout* layout = new QGridLayout();
//    layout->addWidget(btn1, 0, 0);
//    layout->addWidget(btn2, 0, 1);
//    layout->addWidget(btn3, 1, 0);
//    layout->addWidget(btn4, 1, 1);

    // 这个写法就相当于水平布局了
    // 此时大家的行数一样就可以，不一定非得是0
//    layout->addWidget(btn1, 0, 0);
//    layout->addWidget(btn2, 0, 1);
//    layout->addWidget(btn3, 0, 2);
//    layout->addWidget(btn4, 0, 3);

    // 这个写法就相当于是垂直布局了
//    layout->addWidget(btn1, 0, 0);
//    layout->addWidget(btn2, 1, 0);
//    layout->addWidget(btn3, 2, 0);
//    layout->addWidget(btn4, 3, 0);

    // 这个写法是每个按钮独占一行和一列
    // 即使这是成 100,100，也不搞出很大的空白空间
    // 此时设置的行数和列数, 只是用来决定控件之前的相对位置
    layout->addWidget(btn1, 0, 0);
    layout->addWidget(btn2, 1, 1);
    layout->addWidget(btn3, 2, 2);
    layout->addWidget(btn4, 3, 3);
//    layout->addWidget(btn4, 100, 100);

    this->setLayout(layout);
}

Widget::~Widget()
{
    delete ui;
}

