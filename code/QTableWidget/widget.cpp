#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 创建 3 行
    ui->tableWidget->insertRow(0);
    ui->tableWidget->insertRow(1);
    ui->tableWidget->insertRow(2);

    // 创建 3 列
    ui->tableWidget->insertColumn(0);
    ui->tableWidget->insertColumn(1);
    ui->tableWidget->insertColumn(2);

    // 给 3 个列设定列名(设置水平方向的表头)
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("学号"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("姓名"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("年龄"));

    // 给表格添加数据
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("1001"));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem("张三"));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem("20"));

    ui->tableWidget->setItem(1, 0, new QTableWidgetItem("1002"));
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem("李四"));
    ui->tableWidget->setItem(1, 2, new QTableWidgetItem("19"));

    ui->tableWidget->setItem(2, 0, new QTableWidgetItem("1003"));
    ui->tableWidget->setItem(2, 1, new QTableWidgetItem("王五"));
    ui->tableWidget->setItem(2, 2, new QTableWidgetItem("20"));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_insertRow_clicked()
{
    // 需要知道当前一共有多少行了，然后在最后一行加上新行
    int rowCount = ui->tableWidget->rowCount();
    // 注意此处的参数是下标,表示你新增之后的这一行是第几行
    ui->tableWidget->insertRow(rowCount);
}

void Widget::on_pushButton_deleteRow_clicked()
{
    // 获取到选中的行号
    int currentRow = ui->tableWidget->currentRow();
    // 删除这一行
    ui->tableWidget->removeRow(currentRow);
}

void Widget::on_pushButton_insertColumn_clicked()
{
    // 先获取到一共有几列
    int columnCount = ui->tableWidget->columnCount();
    // 在对应的位置新增这一列
    ui->tableWidget->insertColumn(columnCount);
    // 设置列名(从输入框中获取到)
    const QString& text = ui->lineEdit->text();
    ui->tableWidget->setHorizontalHeaderItem(columnCount, new QTableWidgetItem(text));
}


void Widget::on_pushButton_deleteColumn_clicked()
{
    // 获取到选中的列
    int curColumn = ui->tableWidget->currentColumn();
    // 删除这一列
    ui->tableWidget->removeColumn(curColumn);
}
