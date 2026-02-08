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
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        pushButton = new QPushButton(Widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(270, 160, 141, 71));
        pushButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"	font-size: 20px;\n"
"	border: 2px solid rgb(106, 255, 188);\n"
"	border-radius: 10px;\n"
"	background-color: rgb(0, 255, 255)\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"	background: rgb(157, 111, 255)\n"
"}"));
        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(280, 260, 121, 61));
        pushButton_2->setStyleSheet(QString::fromUtf8("/* \346\214\211\351\222\256\345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\345\234\206\350\247\222\343\200\201\347\201\260\350\211\262\350\203\214\346\231\257\343\200\201\350\276\271\346\241\206 */\n"
"QPushButton {\n"
"    font-size: 20px;          /* \346\226\207\345\255\227\345\244\247\345\260\217 */\n"
"    border: 2px solid #8f8f91;/* \350\276\271\346\241\206\357\274\232\345\256\275\345\272\246+\351\242\234\350\211\262 */\n"
"    border-radius: 15px;     /* \345\234\206\350\247\222\357\274\210\346\225\260\345\200\274\350\266\212\345\244\247\350\266\212\345\234\206\357\274\211 */\n"
"    background-color: #dadbde;/* \350\203\214\346\231\257\350\211\262 */\n"
"    padding: 8px 20px;      /* \345\206\205\350\276\271\350\267\235\357\274\232\344\270\212\344\270\2138px\357\274\214\345\267\246\345\217\26320px\357\274\210\346\211\251\345\244\247\347\202\271\345\207\273\345\214\272\345\237\237\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\345\201\234\347\212\266\346\200\201\357"
                        "\274\232\350\203\214\346\231\257\350\211\262\345\217\230\346\265\205 */\n"
"QPushButton:hover {\n"
"    background-color: #e8e9ed;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\345\216\213\347\212\266\346\200\201\357\274\232\350\203\214\346\231\257\350\211\262\345\217\230\347\231\275\357\274\214\350\276\271\346\241\206\345\206\205\351\231\267 */\n"
"QPushButton:pressed {\n"
"    background-color: #f6f7fa;\n"
"    border-style: inset;     /* \350\276\271\346\241\206\345\206\205\351\231\267\357\274\214\346\250\241\346\213\237\346\214\211\345\216\213\346\225\210\346\236\234 */\n"
"}\n"
"\n"
"/* \347\246\201\347\224\250\347\212\266\346\200\201\357\274\232\347\201\260\350\211\262\350\203\214\346\231\257+\347\201\260\350\211\262\346\226\207\345\255\227 */\n"
"QPushButton:disabled {\n"
"    background-color: #f0f0f0;\n"
"    color: #999;\n"
"    border-color: #ddd;\n"
"}"));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "\346\214\211\351\222\256", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Widget", "\346\214\211\351\222\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
