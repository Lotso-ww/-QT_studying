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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QCheckBox *checkBox;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        checkBox = new QCheckBox(Widget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(230, 150, 171, 91));
        checkBox->setStyleSheet(QString::fromUtf8("QCheckBox{\n"
"	font-size: 20px;\n"
"}\n"
"\n"
"QCheckBox::indicator{\n"
"	width: 20px;\n"
"	height: 20px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked{\n"
"	image: url(:/checkbox-unchecked.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:hover{\n"
"	image: url(:/checkbox-unchecked_hover.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:pressed{\n"
"	image: url(:/checkbox-unchecked_pressed.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked{\n"
"	image: url(:/checkbox-checked.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:hover{\n"
"	image: url(:/checkbox-checked_hover.png)\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:pressed{\n"
"	image: url(:/checkbox-checked_pressed.png)\n"
"}"));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        checkBox->setText(QCoreApplication::translate("Widget", "\345\244\215\351\200\211\346\241\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
