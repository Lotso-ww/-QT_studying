#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QGridLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 创建 6 个按钮，按照 3 行 2 列的方式进行排列
    QPushButton* btn1 = new QPushButton("按钮1");
    QPushButton* btn2 = new QPushButton("按钮2");
    QPushButton* btn3 = new QPushButton("按钮3");
    QPushButton* btn4 = new QPushButton("按钮4");
    QPushButton* btn5 = new QPushButton("按钮5");
    QPushButton* btn6 = new QPushButton("按钮6");

    // 可以垂直拉伸的关键步骤
    btn1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn5->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn6->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建 layout 并把按钮添加进去
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(btn1, 0, 0);
    layout->addWidget(btn2, 0, 1);
    layout->addWidget(btn3, 1, 0);
    layout->addWidget(btn4, 1, 1);
    layout->addWidget(btn5, 2, 0);
    layout->addWidget(btn6, 2, 1);

    // 把 layout 设置到窗口中
    this->setLayout(layout);

    // 设置拉伸系数
    // 直接设置是无效的,得需要上面那个关键步骤
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setRowStretch(2, 2);
}

Widget::~Widget()
{
    delete ui;
}

