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
#include <QtWidgets/QListWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QListWidget *listWidget;
    QListWidget *listWidget_2;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        listWidget = new QListWidget(Widget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(140, 70, 371, 311));
        listWidget->setStyleSheet(QString::fromUtf8("QListWidget::item:hover{\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FAFBFE, stop: 1 #DCDEF1);\n"
"}\n"
"\n"
"QListWidget::item:selected{\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6a6ea9, stop: 1 #888dd9);\n"
"}"));
        listWidget_2 = new QListWidget(Widget);
        new QListWidgetItem(listWidget_2);
        new QListWidgetItem(listWidget_2);
        new QListWidgetItem(listWidget_2);
        new QListWidgetItem(listWidget_2);
        new QListWidgetItem(listWidget_2);
        listWidget_2->setObjectName(QString::fromUtf8("listWidget_2"));
        listWidget_2->setGeometry(QRect(150, 390, 256, 192));
        listWidget_2->setStyleSheet(QString::fromUtf8("/* \345\210\227\350\241\250\351\241\271\351\273\230\350\256\244\346\240\267\345\274\217 */\n"
"QListView::item {\n"
"    height: 30px; /* \345\210\227\350\241\250\351\241\271\351\253\230\345\272\246 */\n"
"    padding-left: 10px; /* \346\226\207\345\255\227\345\267\246\351\227\264\350\267\235 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\345\201\234\357\274\232\345\236\202\347\233\264\346\270\220\345\217\230\357\274\210\346\265\205\350\223\235\342\206\222\346\265\205\347\264\253\357\274\211 */\n"
"QListView::item:hover {\n"
"    background: qlineargradient(\n"
"        x1: 0, y1: 0, x2: 0, y2: 1,\n"
"        stop: 0 #FAFBFE,  /* \351\241\266\351\203\250\351\242\234\350\211\262 */\n"
"        stop: 1 #DCDEF1   /* \345\272\225\351\203\250\351\242\234\350\211\262 */\n"
"    );\n"
"}\n"
"\n"
"/* \351\200\211\344\270\255\347\212\266\346\200\201\357\274\232\345\236\202\347\233\264\346\270\220\345\217\230\357\274\210\346\267\261\350\223\235\342\206\222\346\267\261\347\264\253\357\274\211+ \350\276\271\346"
                        "\241\206 */\n"
"QListView::item:selected {\n"
"    border: 1px solid #6a6ea9; /* \351\200\211\344\270\255\350\276\271\346\241\206 */\n"
"    background: qlineargradient(\n"
"        x1: 0, y1: 0, x2: 0, y2: 1,\n"
"        stop: 0 #6a6ea9,  /* \351\241\266\351\203\250\351\242\234\350\211\262 */\n"
"        stop: 1 #888dd9   /* \345\272\225\351\203\250\351\242\234\350\211\262 */\n"
"    );\n"
"    color: #fff; /* \351\200\211\344\270\255\346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"}\n"
"\n"
"/* \345\210\227\350\241\250\346\241\206\350\203\214\346\231\257\357\274\210\345\216\273\351\231\244\351\273\230\350\256\244\347\231\275\350\211\262\357\274\211 */\n"
"QListView {\n"
"    background-color: #f8f8f8;\n"
"    border: none; /* \345\217\226\346\266\210\351\273\230\350\256\244\350\276\271\346\241\206 */\n"
"}"));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("Widget", "111", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("Widget", "222", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = listWidget->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("Widget", "333", nullptr));
        listWidget->setSortingEnabled(__sortingEnabled);


        const bool __sortingEnabled1 = listWidget_2->isSortingEnabled();
        listWidget_2->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem3 = listWidget_2->item(0);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("Widget", "111", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = listWidget_2->item(1);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("Widget", "222", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = listWidget_2->item(2);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("Widget", "333", nullptr));
        QListWidgetItem *___qlistwidgetitem6 = listWidget_2->item(3);
        ___qlistwidgetitem6->setText(QCoreApplication::translate("Widget", "444", nullptr));
        QListWidgetItem *___qlistwidgetitem7 = listWidget_2->item(4);
        ___qlistwidgetitem7->setText(QCoreApplication::translate("Widget", "555", nullptr));
        listWidget_2->setSortingEnabled(__sortingEnabled1);

    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
