#include "dialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

Dialog::Dialog()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    this->setLayout(layout);

    QLabel* label = new QLabel("这是一个弹窗");
    QPushButton* button = new QPushButton("关闭");
    layout->addWidget(label);
    layout->addWidget(button);

    connect(button, &QPushButton::clicked, this, [=]()
    {
       this->close();
    });
}
