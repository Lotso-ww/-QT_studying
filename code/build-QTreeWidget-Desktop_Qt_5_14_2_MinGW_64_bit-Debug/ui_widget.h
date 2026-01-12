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
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QTreeWidget *treeWidget;
    QLineEdit *lineEdit;
    QPushButton *pushButton_insettTopLevelItem;
    QPushButton *pushButton_inertItem;
    QPushButton *pushButton_deleteItem;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        treeWidget = new QTreeWidget(Widget);
        treeWidget->headerItem()->setText(0, QString());
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(20, 10, 551, 571));
        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(580, 50, 211, 71));
        pushButton_insettTopLevelItem = new QPushButton(Widget);
        pushButton_insettTopLevelItem->setObjectName(QString::fromUtf8("pushButton_insettTopLevelItem"));
        pushButton_insettTopLevelItem->setGeometry(QRect(590, 130, 191, 51));
        pushButton_inertItem = new QPushButton(Widget);
        pushButton_inertItem->setObjectName(QString::fromUtf8("pushButton_inertItem"));
        pushButton_inertItem->setGeometry(QRect(590, 200, 191, 51));
        pushButton_deleteItem = new QPushButton(Widget);
        pushButton_deleteItem->setObjectName(QString::fromUtf8("pushButton_deleteItem"));
        pushButton_deleteItem->setGeometry(QRect(590, 270, 191, 51));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("Widget", "\350\257\267\350\276\223\345\205\245\350\246\201\346\217\222\345\205\245\347\232\204\350\212\202\347\202\271\345\220\215\345\255\227", nullptr));
        pushButton_insettTopLevelItem->setText(QCoreApplication::translate("Widget", "\346\267\273\345\212\240\345\210\260\351\241\266\345\261\202\345\205\203\347\264\240", nullptr));
        pushButton_inertItem->setText(QCoreApplication::translate("Widget", "\346\267\273\345\212\240\351\200\211\344\270\255\345\205\203\347\264\240", nullptr));
        pushButton_deleteItem->setText(QCoreApplication::translate("Widget", "\345\210\240\351\231\244\351\200\211\344\270\255\345\205\203\347\264\240", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
