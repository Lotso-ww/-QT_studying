#include "widget.h"
#include "ui_widget.h"
#include <QIcon>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // QIcon icon("D:/Gitee.code/qt-learning/code/QWidget_6/Lotso.jpg");
    // 相对路径会更好点,但是也无法解决一些问题
    // qrc来解决
    QIcon icon(":/Lotso.jpg");
    this->setWindowIcon(icon);
}

Widget::~Widget()
{
    delete ui;
}

