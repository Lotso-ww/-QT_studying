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
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QFrame *frame;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QCheckBox *checkBox;
    QPushButton *pushButton;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->setWindowModality(Qt::NonModal);
        Widget->resize(800, 600);
        Widget->setStyleSheet(QString::fromUtf8("QFrame{\n"
"	border-image: url(:/Snipaste.png)\n"
"}\n"
"\n"
"QLineEdit{\n"
"	color: white;\n"
"	background-color: #405361;\n"
"	padding: 0 5px;\n"
"	border: none;\n"
"	border-radius: 10px;\n"
"}\n"
"\n"
"QCheckBox{\n"
"	color: white;\n"
"	font-size: 18px;\n"
"}\n"
"QPushButton{\n"
" 	color: white;\n"
"	font-size: 20px;\n"
"	background-color: #555;\n"
"	border-radius: 10px;\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"	color: black;\n"
"	background-color: red;\n"
"}"));
        frame = new QFrame(Widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(-10, -10, 811, 621));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        widget = new QWidget(frame);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(280, 160, 231, 211));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMinimumSize(QSize(0, 50));
        lineEdit->setMaximumSize(QSize(16777215, 50));
        lineEdit->setEchoMode(QLineEdit::Normal);

        verticalLayout->addWidget(lineEdit);

        lineEdit_2 = new QLineEdit(widget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setMinimumSize(QSize(0, 50));
        lineEdit_2->setMaximumSize(QSize(16777215, 50));
        lineEdit_2->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(lineEdit_2);

        checkBox = new QCheckBox(widget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        verticalLayout->addWidget(checkBox);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(0, 50));
        pushButton->setMaximumSize(QSize(16777215, 50));

        verticalLayout->addWidget(pushButton);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "\347\231\273\345\275\225\347\225\214\351\235\242", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("Widget", "\350\257\267\350\276\223\345\205\245\347\224\250\346\210\267\345\220\215", nullptr));
        lineEdit_2->setPlaceholderText(QCoreApplication::translate("Widget", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        checkBox->setText(QCoreApplication::translate("Widget", "\350\256\260\344\275\217\345\257\206\347\240\201", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
