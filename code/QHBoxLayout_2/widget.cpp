#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 创建垂直的布局管理器
    QVBoxLayout* vlayout = new QVBoxLayout();
    this->setLayout(vlayout);

    // 添加两个按钮进去
    QPushButton* btn1 = new QPushButton("按钮1");
    QPushButton* btn2 = new QPushButton("按钮2");
    vlayout->addWidget(btn1);
    vlayout->addWidget(btn2);

    // 创建水平的布局管理器
    QHBoxLayout* hlayout = new QHBoxLayout();

    // 添加两个按钮进去
    QPushButton* btn3 = new QPushButton("按钮3");
    QPushButton* btn4 = new QPushButton("按钮4");
    hlayout->addWidget(btn3);
    hlayout->addWidget(btn4);

    // 把水平的添加到垂直的
    vlayout->addLayout(hlayout);
}

Widget::~Widget()
{
    delete ui;
}

