#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QGridLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 创建 6 个按钮，使用网格布局按照 2 * 3 的方式来排列
    QPushButton* btn1 = new QPushButton("按钮1");
    QPushButton* btn2 = new QPushButton("按钮2");
    QPushButton* btn3 = new QPushButton("按钮3");
    QPushButton* btn4 = new QPushButton("按钮4");
    QPushButton* btn5 = new QPushButton("按钮5");
    QPushButton* btn6 = new QPushButton("按钮6");

    // 创建网格布局管理器，把这些控件添加进去
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(btn1, 0, 0);
    layout->addWidget(btn2, 0, 1);
    layout->addWidget(btn3, 0, 2);
    layout->addWidget(btn4, 1, 0);
    layout->addWidget(btn5, 1, 1);
    layout->addWidget(btn6, 1, 2);

    this->setLayout(layout);

    // 设置水平拉伸系数
    // 此处代码的含义就是这三个列按 1:1:2 来设置宽度
    // 如果拉伸系数设为0，则证明不参与拉伸
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 2);
}

Widget::~Widget()
{
    delete ui;
}

