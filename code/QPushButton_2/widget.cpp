#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 设置这些按钮的图标
    ui->pushButton_target->setIcon(QIcon(":/image/code.png"));
    ui->pushButton_target->setIconSize(QSize(120, 120));

    ui->pushButton_up->setIcon(QIcon(":/image/up.png"));
    ui->pushButton_up->setIconSize(QSize(50, 50));

    ui->pushButton_down->setIcon(QIcon(":/image/down.png"));
    ui->pushButton_down->setIconSize(QSize(50, 50));

    ui->pushButton_left->setIcon(QIcon(":/image/left.png"));
    ui->pushButton_left->setIconSize(QSize(50, 50));

    ui->pushButton_right->setIcon(QIcon(":/image/right.png"));
    ui->pushButton_right->setIconSize(QSize(50, 50));

    // 设置快捷键
//    ui->pushButton_up->setShortcut(QKeySequence("w"));
//    ui->pushButton_down->setShortcut(QKeySequence("s"));
//    ui->pushButton_left->setShortcut(QKeySequence("a"));
//    ui->pushButton_right->setShortcut(QKeySequence("d"));

    ui->pushButton_up->setShortcut(QKeySequence(Qt::Key_W));
    ui->pushButton_down->setShortcut(QKeySequence(Qt::Key_S));
    ui->pushButton_left->setShortcut(QKeySequence(Qt::Key_A));
    ui->pushButton_right->setShortcut(QKeySequence(Qt::Key_D));

    // 开启鼠标点击的连发功能
    ui->pushButton_up->setAutoRepeat(true);
    ui->pushButton_down->setAutoRepeat(true);
    ui->pushButton_left->setAutoRepeat(true);
    ui->pushButton_right->setAutoRepeat(true);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_up_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    ui->pushButton_target->setGeometry(rect.x(), rect.y() - 5, rect.width(), rect.height());
}

void Widget::on_pushButton_down_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    ui->pushButton_target->setGeometry(rect.x(), rect.y() + 5, rect.width(), rect.height());
}

void Widget::on_pushButton_left_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    ui->pushButton_target->setGeometry(rect.x() - 5, rect.y(), rect.width(), rect.height());
}

void Widget::on_pushButton_right_clicked()
{
    QRect rect = ui->pushButton_target->geometry();
    ui->pushButton_target->setGeometry(rect.x() + 5, rect.y(), rect.width(), rect.height());
}

