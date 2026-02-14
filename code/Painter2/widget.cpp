#include "widget.h"
#include "ui_widget.h"
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter painter(this);

    QPixmap pixmap(":/Snipaste.png");

    // 基础绘制
//    painter.drawPixmap(0,0,pixmap);
//    painter.drawPixmap(100,100,pixmap);

    // 图片缩放
//    painter.drawPixmap(100,100,400,300,pixmap);

    // 图片的旋转, 本质上是把 Qpainter 对象进行了需旋转, 绘制出来的内容也就旋转了
    painter.rotate(180); // 默认绕原点(0,0)旋转,所以会到屏幕外去
    painter.translate(-800,-600); // 我们可以平移一下,但是由于之前旋转了所以坐标轴也变了,现在是用负的
    painter.drawPixmap(100,100,400,300,pixmap);

}

