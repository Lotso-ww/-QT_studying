#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

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
    void on_pushButton_insertRow_clicked();

    void on_pushButton_deleteRow_clicked();

    void on_pushButton_insertColumn_clicked();

    void on_pushButton_deleteColumn_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
