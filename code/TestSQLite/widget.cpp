#include "widget.h"
#include "ui_widget.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

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


void Widget::on_connectDB_clicked()
{
    // 1. 导入数据库驱动
    sqlite = QSqlDatabase::addDatabase("QSQLITE");

    // 2. 设置数据库名称
    sqlite.setDatabaseName("studentDB.db");

    // 3. 打开数据库: 如果存在直接打开,不存在先创建再打开
    if(!sqlite.open())
    {
        qDebug() << "打开连接数据库失败[studentDB.db]: " << sqlite.lastError().text();
        return;
    }
    qDebug() << "打开连接数据库成功";

    // .....
}

void Widget::on_ctreateTable_clicked()
{
    QSqlQuery query;
    QString sql("CREATE TABLE IF NOT EXISTS student( \
                id INTEGER PRIMARY KEY AUTOINCREMENT,\
                name varchar(50) NOT NULL,\
                age INTEFER,\
                gpa REAL)\
                ");
   if(!query.exec(sql))
    {
        qDebug() << "创建学生表[student]失败: "<<query.lastError().text();
        return;
    }
    qDebug() << "创建学生表[student]成功";

}

void Widget::on_closeDB_clicked()
{
    sqlite.close();
}
