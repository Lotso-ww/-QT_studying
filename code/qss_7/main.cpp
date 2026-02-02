#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString style = "QPushButton { color: red; }";
    style += "QPushButton:hover { color: blue; }";
    style += "QPushButton:pressed { color: green; }";
    a.setStyleSheet(style);
    Widget w;
    w.show();
    return a.exec();
}
