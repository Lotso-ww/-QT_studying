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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *connectDB;
    QPushButton *closeDB;
    QPushButton *ctreateTable;
    QPushButton *insert;
    QPushButton *delete_2;
    QPushButton *select;
    QPushButton *update;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        connectDB = new QPushButton(Widget);
        connectDB->setObjectName(QString::fromUtf8("connectDB"));
        connectDB->setGeometry(QRect(60, 210, 99, 28));
        closeDB = new QPushButton(Widget);
        closeDB->setObjectName(QString::fromUtf8("closeDB"));
        closeDB->setGeometry(QRect(190, 210, 99, 28));
        ctreateTable = new QPushButton(Widget);
        ctreateTable->setObjectName(QString::fromUtf8("ctreateTable"));
        ctreateTable->setGeometry(QRect(60, 290, 99, 28));
        insert = new QPushButton(Widget);
        insert->setObjectName(QString::fromUtf8("insert"));
        insert->setGeometry(QRect(170, 290, 99, 28));
        delete_2 = new QPushButton(Widget);
        delete_2->setObjectName(QString::fromUtf8("delete_2"));
        delete_2->setGeometry(QRect(290, 290, 99, 28));
        select = new QPushButton(Widget);
        select->setObjectName(QString::fromUtf8("select"));
        select->setGeometry(QRect(410, 290, 99, 28));
        update = new QPushButton(Widget);
        update->setObjectName(QString::fromUtf8("update"));
        update->setGeometry(QRect(530, 290, 99, 28));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        connectDB->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245\346\225\260\346\215\256\345\272\223", nullptr));
        closeDB->setText(QCoreApplication::translate("Widget", "\346\226\255\345\274\200\346\225\260\346\215\256\345\272\223", nullptr));
        ctreateTable->setText(QCoreApplication::translate("Widget", "\345\210\233\345\273\272\350\241\250", nullptr));
        insert->setText(QCoreApplication::translate("Widget", "\345\242\236\345\212\240", nullptr));
        delete_2->setText(QCoreApplication::translate("Widget", "\345\210\240\351\231\244", nullptr));
        select->setText(QCoreApplication::translate("Widget", "\346\237\245\346\211\276", nullptr));
        update->setText(QCoreApplication::translate("Widget", "\346\233\264\346\226\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
