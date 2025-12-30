#include "mylabel.h"
#include <iostream>
#include <QDebug>

mylabel::mylabel(QWidget* parent)
    :QLabel(parent)
{

}

mylabel::~mylabel()
{
    // std::cout<< "mylabel 被销毁!" << std::endl;
    qDebug()<< "mylabel 被销毁!";
}


