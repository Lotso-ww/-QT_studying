#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

//    QCursor cursor(Qt::OpenHandCursor);
//    ui->pushButton->setCursor(cursor);
    // 访问一个图片
    QPixmap pixmap(":/code.png");
    // pixmap = pixmap.scaled(100, 100); // 缩放图片
    QCursor cursor(pixmap, 10, 10); // 默认热点位置会是图片左上角
    this->setCursor(cursor);
}

Widget::~Widget()
{
    delete ui;
}

