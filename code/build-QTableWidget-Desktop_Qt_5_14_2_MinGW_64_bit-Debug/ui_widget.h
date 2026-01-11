/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QTableWidget *tableWidget;
    QPushButton *pushButton_insertRow;
    QPushButton *pushButton_deleteRow;
    QPushButton *pushButton_insertColumn;
    QPushButton *pushButton_deleteColumn;
    QLineEdit *lineEdit;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        tableWidget = new QTableWidget(Widget);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(30, 10, 731, 461));
        pushButton_insertRow = new QPushButton(Widget);
        pushButton_insertRow->setObjectName(QString::fromUtf8("pushButton_insertRow"));
        pushButton_insertRow->setGeometry(QRect(60, 500, 101, 31));
        pushButton_deleteRow = new QPushButton(Widget);
        pushButton_deleteRow->setObjectName(QString::fromUtf8("pushButton_deleteRow"));
        pushButton_deleteRow->setGeometry(QRect(60, 540, 101, 31));
        pushButton_insertColumn = new QPushButton(Widget);
        pushButton_insertColumn->setObjectName(QString::fromUtf8("pushButton_insertColumn"));
        pushButton_insertColumn->setGeometry(QRect(190, 500, 101, 31));
        pushButton_deleteColumn = new QPushButton(Widget);
        pushButton_deleteColumn->setObjectName(QString::fromUtf8("pushButton_deleteColumn"));
        pushButton_deleteColumn->setGeometry(QRect(190, 540, 101, 31));
        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(350, 510, 361, 51));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton_insertRow->setText(QCoreApplication::translate("Widget", "\346\226\260\345\242\236\344\270\200\350\241\214", nullptr));
        pushButton_deleteRow->setText(QCoreApplication::translate("Widget", "\345\210\240\351\231\244\351\200\211\344\270\255\350\241\214", nullptr));
        pushButton_insertColumn->setText(QCoreApplication::translate("Widget", "\346\226\260\345\242\236\344\270\200\345\210\227", nullptr));
        pushButton_deleteColumn->setText(QCoreApplication::translate("Widget", "\345\210\240\351\231\244\351\200\211\344\270\255\345\210\227", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("Widget", "\350\257\267\350\276\223\345\205\245\346\226\260\345\242\236\347\232\204\345\210\227\345\220\215", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
