#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 对QWidget和它的子类都生效
    // a.setStyleSheet("QWidget { color: red; }");
    // 只对QWidget生效
    // a.setStyleSheet(".QWidget { color: red; }");

//    QString style = "QPushButton { color: red; }";
//    // 利用ID选择器给这两个按钮单独设置
//    style += "#pushButton_2 { color: blue; }";
//    style += "#pushButton_3 { color: green; }";

    // 并集选择器, 可以选择组合使用
//    QString style = "QPushButton, QLabel, QLineEdit { color: red; }";
    QString style = "#pushButton_2, QLabel, QLineEdit { color: red; }";

    a.setStyleSheet(style);
    Widget w;
    w.show();
    return a.exec();
}
