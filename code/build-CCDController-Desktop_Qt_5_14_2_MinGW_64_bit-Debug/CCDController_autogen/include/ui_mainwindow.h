/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *up;
    QPushButton *play;
    QPushButton *down;
    QComboBox *comboBox;
    QSlider *slider;
    QLabel *timer;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 499);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(150, 10, 641, 421));
        label->setStyleSheet(QString::fromUtf8("#label\n"
"{\n"
"	background-color:rgb(0, 0, 0);\n"
"}"));
        label->setScaledContents(true);
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(150, 430, 641, 31));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        up = new QPushButton(widget);
        up->setObjectName(QString::fromUtf8("up"));
        up->setMinimumSize(QSize(30, 0));
        up->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(up);

        play = new QPushButton(widget);
        play->setObjectName(QString::fromUtf8("play"));
        play->setMinimumSize(QSize(30, 0));
        play->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(play);

        down = new QPushButton(widget);
        down->setObjectName(QString::fromUtf8("down"));
        down->setMinimumSize(QSize(30, 0));
        down->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(down);

        comboBox = new QComboBox(widget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout->addWidget(comboBox);

        slider = new QSlider(widget);
        slider->setObjectName(QString::fromUtf8("slider"));
        slider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(slider);

        timer = new QLabel(widget);
        timer->setObjectName(QString::fromUtf8("timer"));
        timer->setMinimumSize(QSize(60, 0));
        timer->setMaximumSize(QSize(60, 16777215));

        horizontalLayout->addWidget(timer);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QString());
        up->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        play->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        down->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "1x", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MainWindow", "2x", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MainWindow", "3x", nullptr));

        timer->setText(QCoreApplication::translate("MainWindow", "    /", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
