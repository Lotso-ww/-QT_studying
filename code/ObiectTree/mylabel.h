#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>

class mylabel : public QLabel
{
public:
    // 构造函数使用带 QWidget* 的版本
    // 这样才能确保我们自己的对象可以加到对象树上
    mylabel(QWidget* parent);
    ~mylabel();
};

#endif // MYLABEL_H
