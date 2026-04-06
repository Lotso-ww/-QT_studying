#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_connectDB_clicked();

    void on_ctreateTable_clicked();

    void on_closeDB_clicked();

private:
    Ui::Widget *ui;
    QSqlDatabase sqlite;
};
#endif // WIDGET_H
