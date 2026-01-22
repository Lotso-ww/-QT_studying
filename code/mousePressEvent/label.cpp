#include "label.h"
#include <QDebug>
#include <QMouseEvent>

Label::Label(QWidget* parent)
    :QLabel(parent)
{

}

void Label::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton){
        qDebug() << "按下左键";
    }else if(ev->button() == Qt::RightButton){
        qDebug() << "按下右键";
    }

    // 当前 ev 对象就包含了鼠标点击位置的坐标
    qDebug() << ev->x() << ',' << ev->y(); // 相对于Label
    qDebug() << ev->globalX() << ',' << ev->globalY(); // 相对于屏幕左上角
}

void Label::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton){
        qDebug() << "释放左键";
    }else if(ev->button() == Qt::RightButton){
        qDebug() << "释放右键";
    }
}

void Label::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton){
        qDebug() << "双击左键";
    }else if(ev->button() == Qt::RightButton){
        qDebug() << "双击右键";
    }
}
