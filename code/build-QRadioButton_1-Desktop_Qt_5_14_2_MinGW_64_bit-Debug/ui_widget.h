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
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QRadioButton *radioButton_male;
    QRadioButton *radioButton_female;
    QRadioButton *radioButton_other;
    QLabel *label;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        radioButton_male = new QRadioButton(Widget);
        radioButton_male->setObjectName(QString::fromUtf8("radioButton_male"));
        radioButton_male->setGeometry(QRect(280, 230, 115, 19));
        radioButton_female = new QRadioButton(Widget);
        radioButton_female->setObjectName(QString::fromUtf8("radioButton_female"));
        radioButton_female->setGeometry(QRect(280, 270, 115, 19));
        radioButton_other = new QRadioButton(Widget);
        radioButton_other->setObjectName(QString::fromUtf8("radioButton_other"));
        radioButton_other->setGeometry(QRect(280, 310, 115, 19));
        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(240, 180, 141, 31));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        radioButton_male->setText(QCoreApplication::translate("Widget", "\347\224\267", nullptr));
        radioButton_female->setText(QCoreApplication::translate("Widget", "\345\245\263", nullptr));
        radioButton_other->setText(QCoreApplication::translate("Widget", "\345\205\266\344\273\226", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\346\202\250\347\232\204\346\200\247\345\210\253\346\230\257:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
