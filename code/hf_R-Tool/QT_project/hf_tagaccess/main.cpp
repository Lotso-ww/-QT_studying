#include "mainwindow.h"

#include <QApplication>

// 程序入口函数
// argc: 命令行参数个数, argv: 命令行参数数组
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);   // 创建Qt应用程序对象，管理控制流和主设置
    MainWindow w;                 // 创建主窗口对象
    w.show();                      // 显示主窗口
    return a.exec();               // 进入Qt主事件循环，等待用户操作(关闭窗口时退出)
}