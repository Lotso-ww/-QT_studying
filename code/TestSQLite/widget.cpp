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

void Widget::on_insert_clicked()
{
    // QString sql("INSERT INTO student(id,name,age,gpa) values(1, '张三', 18, 3.0)");
    // QString sql("INSERT INTO student(name,age,gpa) values('张三', 18, 3.0)");
    // QString sql("INSERT INTO student values(2, '李四', 19, 3.5)");
//    QString sql("INSERT INTO student(id,name,age,gpa) values(%1, '%2', %3, %4)");
//    sql = sql.arg(4, 1, 10)
//             .arg("王五")
//             .arg(20)
//             .arg(3.8);

//    QSqlQuery query;
//    if(!query.exec(sql))
//    {
//        qDebug() << "往学生表里插入数据失败: "<< query.lastError().text();
//        return;
//    }
//    qDebug() << "插入成功";

    // 按照名称绑定
//    QSqlQuery query;
//    query.prepare("INSERT INTO student(name,age,gpa) values(:name, :age, :gpa)");
//    query.bindValue(":name", "赵六");
//    query.bindValue(":age", 21);
//    query.bindValue(":gpa", 4.0);
//    if(!query.exec())
//    {
//        qDebug() << "往学生表里插入数据失败: "<< query.lastError().text();
//        return;
//    }
    // qDebug() << "插入成功";

    // 按照位置绑定
//    QSqlQuery query;
//    query.prepare("INSERT INTO student(name,age,gpa) values(?,?,?)");
//    query.bindValue(0, "田七");
//    query.bindValue(1, 22);
//    query.bindValue(2, 4.2);
//    if(!query.exec())
//    {
//        qDebug() << "往学生表里插入数据失败: "<< query.lastError().text();
//        return;
//    }
//    qDebug() << "插入成功";

    // 按照固定次序来, 不能改变 -- 虽然不会报错但是数据顺序是错的
    QSqlQuery query;
    query.prepare("INSERT INTO student(name,age,gpa) values(?,?,?)");
    query.addBindValue("刘八");
    query.addBindValue(23);
    query.addBindValue(5.2);
    if(!query.exec())
    {
        qDebug() << "往学生表里插入数据失败: "<< query.lastError().text();
        return;
    }
    qDebug() << "插入成功";
}

void Widget::on_select_clicked()
{
    QString sql("SELECT * FROM student");
    QSqlQuery query;
    if(!query.exec(sql))
    {
        qDebug() << "在学生表里查询数据失败: "<< query.lastError().text();
        return;
    }

    // 按照位置查询
    while(query.next())
    {
        qDebug() << query.value(0).toInt()
                 << query.value(1).toString()
                 << query.value(2).toInt()
                 << query.value(3).toDouble();
    }

//    // 按照列名查询
//    while(query.next())
//    {
//        qDebug() << query.value("id").toInt()
//                 << query.value("name").toString()
//                 << query.value("age").toInt()
//                 << query.value("gpa").toDouble();
//    }
}

void Widget::on_update_clicked()
{
    QSqlQuery query;
    query.prepare("UPDATE student SET age=?, gpa=? WHERE name=?");
    query.bindValue(0, 22);
    query.bindValue(1, 5);
    query.bindValue(2, "王五");

    if(!query.exec())
    {
        qDebug() << "往学生表里更新数据失败: "<< query.lastError().text();
        return;
    }

    query.prepare("SELECT * FROM student WHERE name = :name");
    query.bindValue(":name", "王五");
    if(!query.exec())
    {
        qDebug() << "在学生表里查询数据失败: "<< query.lastError().text();
        return;
    }

    // 按照列名查询
    while(query.next())
    {
        qDebug() << query.value("id").toInt()
                 << query.value("name").toString()
                 << query.value("age").toInt()
                 << query.value("gpa").toDouble();
    }

}

void Widget::on_delete_2_clicked()
{
    // 删除id为3的数据
    QSqlQuery query;
    query.prepare("DELETE FROM student WHERE id=?");
    query.addBindValue(3);
    if(!query.exec())
    {
        qDebug() << "在学生表里删除数据失败: "<< query.lastError().text();
        return;
    }

    // 通过查询验证是否正确
    QString sql("SELECT * FROM student");
    if(!query.exec(sql))
    {
        qDebug() << "在学生表里查询数据失败: "<< query.lastError().text();
        return;
    }

    // 按照位置查询
    while(query.next())
    {
        qDebug() << query.value(0).toInt()
                 << query.value(1).toString()
                 << query.value(2).toInt()
                 << query.value(3).toDouble();
    }
}
