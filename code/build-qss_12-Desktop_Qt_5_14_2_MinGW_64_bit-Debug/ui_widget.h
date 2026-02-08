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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(190, 160, 261, 61));
        lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
"	border: 2px solid rgb(170, 170, 255);\n"
"	border-radius: 20px;\n"
"	padding-left: 10px;\n"
"	color: rgb(255, 85, 127);\n"
"	font-size: 25px;\n"
"	background-color: rgb(255, 248, 240);\n"
"	selection-color: rgb(0, 180, 0);\n"
"	selection-background-color: rgb(180, 0, 0);\n"
"}"));
        lineEdit_2 = new QLineEdit(Widget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(200, 250, 251, 61));
        lineEdit_2->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border-width: 1px;    /* \350\276\271\346\241\206\345\256\275\345\272\246 */\n"
"    border-radius: 10px;  /* \345\234\206\350\247\222 */\n"
"    border-color: #585858;/* \350\276\271\346\241\206\351\242\234\350\211\262 */\n"
"    border-style: inset;  /* \350\276\271\346\241\206\351\243\216\346\240\274 */\n"
"    padding: 0 8px;      /* \345\206\205\350\276\271\350\267\235\357\274\232\345\267\246\345\217\2638px\357\274\210\346\226\207\345\255\227\344\270\215\350\264\264\350\276\271\357\274\211 */\n"
"    color: #fff;          /* \346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"    background-color: #405361; /* \350\203\214\346\231\257\350\211\262\357\274\210\346\267\261\350\211\262\357\274\211 */\n"
"    font-size: 20px;\n"
"}\n"
"\n"
"/* \350\276\223\345\205\245\346\241\206\350\216\267\345\217\226\347\204\246\347\202\271\346\227\266\357\274\232\350\276\271\346\241\206\345\217\230\350\211\262 */\n"
"QLineEdit:focus {\n"
"    border-color: #1ab394; /* \344\270\273\351\242\230\350\211"
                        "\262\350\276\271\346\241\206 */\n"
"    outline: none;         /* \345\217\226\346\266\210\351\273\230\350\256\244\347\204\246\347\202\271\350\276\271\346\241\206 */\n"
"}\n"
"\n"
"/* \351\200\211\344\270\255\346\226\207\345\255\227\346\240\267\345\274\217 */\n"
"QLineEdit::selection {\n"
"    background-color: #1ab394; /* \351\200\211\344\270\255\350\203\214\346\231\257\350\211\262 */\n"
"    color: #fff;              /* \351\200\211\344\270\255\346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"}\n"
"\n"
"/* \345\217\252\350\257\273\347\212\266\346\200\201 */\n"
"QLineEdit:read-only {\n"
"    background-color: #333;\n"
"    color: #bbb;\n"
"}"));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
