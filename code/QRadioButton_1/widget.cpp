#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 初始一个默认选择的按钮
    ui->radioButton_male->setChecked(true);
    ui->label->setText("您的性别是: 男");

    // 禁用一个按钮
    // 但这样文本还是会显示，至只是不能被选中了而已
    // ui->radioButton_other->setCheckable(false);
    ui->radioButton_other->setEnabled(false);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_radioButton_male_clicked()
{
    ui->label->setText("您的性别是: 男");
}

void Widget::on_radioButton_female_clicked()
{
    ui->label->setText("您的性别是: 女");
}

void Widget::on_radioButton_other_clicked()
{
    ui->label->setText("您的性别是: 其他");
}
