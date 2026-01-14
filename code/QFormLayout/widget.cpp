#include "widget.h"
#include "ui_widget.h"
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 设置成 3 行 2 列
    QFormLayout* layout = new QFormLayout();
    this->setLayout(layout);

    QLabel* label1 = new QLabel("姓名");
    QLabel* label2 = new QLabel("电话");
    QLabel* label3 = new QLabel("年龄");

    QLineEdit* edit1 = new QLineEdit();
    QLineEdit* edit2 = new QLineEdit();
    QLineEdit* edit3 = new QLineEdit();

    layout->addRow(label1, edit1);
    layout->addRow(label2, edit2);
    layout->addRow(label3, edit3);

    QPushButton* btn1 = new QPushButton("提交");
    layout->addRow(nullptr, btn1);
}

Widget::~Widget()
{
    delete ui;
}

