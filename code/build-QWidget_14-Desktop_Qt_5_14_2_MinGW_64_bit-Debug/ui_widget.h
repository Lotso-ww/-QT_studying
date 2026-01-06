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
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton_light;
    QPushButton *pushButton_dark;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        plainTextEdit = new QPlainTextEdit(Widget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 10, 781, 521));
        pushButton_light = new QPushButton(Widget);
        pushButton_light->setObjectName(QString::fromUtf8("pushButton_light"));
        pushButton_light->setGeometry(QRect(170, 550, 111, 41));
        pushButton_dark = new QPushButton(Widget);
        pushButton_dark->setObjectName(QString::fromUtf8("pushButton_dark"));
        pushButton_dark->setGeometry(QRect(460, 550, 111, 41));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton_light->setText(QCoreApplication::translate("Widget", "\346\227\245\351\227\264\346\250\241\345\274\217", nullptr));
        pushButton_dark->setText(QCoreApplication::translate("Widget", "\345\244\234\351\227\264\346\250\241\345\274\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
