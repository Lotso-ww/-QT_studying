#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

#include <fstream>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 需要读取文件内容,把文件中的每一行读取出来，作为一个 ComboBox 的选项
    std::ifstream file("D:/Gitee.code/qt-learning/code/QComboBox_2/config.txt");
    if(!file.is_open())
    {
        qDebug() << "文件打开失败";
        return;
    }

    // 按行来读取文本
    std::string line;
    while(std::getline(file, line))
    {
        // 取到的每一行内容设置到下拉框中
        ui->comboBox->addItem(QString::fromStdString(line));
    }

    file.close();
}

Widget::~Widget()
{
    delete ui;
}


// 同步下Label的显示
void Widget::on_comboBox_currentTextChanged(const QString &arg1)
{
    (void) arg1;
    ui->label->setText("请选择一个角色:" + ui->comboBox->currentText());
}
