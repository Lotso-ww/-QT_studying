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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action111;
    QAction *action222;
    QAction *action333;
    QAction *action444;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        MainWindow->setStyleSheet(QString::fromUtf8("QMenuBar{\n"
"	background-color: rgb(170, 170, 255);\n"
"	spacing: 5px;\n"
"}\n"
"\n"
"QMenuBar::item{\n"
"	border-radius: 10px;\n"
"	padding: 3px 10px;\n"
"	background-color: rgb(255, 250, 210);\n"
"}\n"
"\n"
"QMenuBar::item:selected{\n"
"	background-color: rgb(255, 85, 0);\n"
"}\n"
"\n"
"QMenu::item{\n"
"	border: 2px solid transparent;\n"
"	padding: 2px 10px;\n"
"}\n"
"\n"
"QMenu::item:selected{\n"
"	border: 2px solid red;\n"
"}\n"
"\n"
"QMenu::separator{\n"
"	heighht: 2px;\n"
"	background-color: green;\n"
"	margin: 0 5px;\n"
"}"));
        action111 = new QAction(MainWindow);
        action111->setObjectName(QString::fromUtf8("action111"));
        action222 = new QAction(MainWindow);
        action222->setObjectName(QString::fromUtf8("action222"));
        action333 = new QAction(MainWindow);
        action333->setObjectName(QString::fromUtf8("action333"));
        action444 = new QAction(MainWindow);
        action444->setObjectName(QString::fromUtf8("action444"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menu_3 = new QMenu(menubar);
        menu_3->setObjectName(QString::fromUtf8("menu_3"));
        menu_4 = new QMenu(menubar);
        menu_4->setObjectName(QString::fromUtf8("menu_4"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu_2->menuAction());
        menubar->addAction(menu_3->menuAction());
        menubar->addAction(menu_4->menuAction());
        menu_2->addAction(action111);
        menu_2->addAction(action222);
        menu_2->addAction(action333);
        menu_2->addSeparator();
        menu_2->addAction(action444);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        action111->setText(QCoreApplication::translate("MainWindow", "111", nullptr));
        action222->setText(QCoreApplication::translate("MainWindow", "222", nullptr));
        action333->setText(QCoreApplication::translate("MainWindow", "333", nullptr));
        action444->setText(QCoreApplication::translate("MainWindow", "444", nullptr));
        menu_2->setTitle(QCoreApplication::translate("MainWindow", "\350\217\234\345\215\2251", nullptr));
        menu_3->setTitle(QCoreApplication::translate("MainWindow", "\350\217\234\345\215\2252", nullptr));
        menu_4->setTitle(QCoreApplication::translate("MainWindow", "\350\217\234\345\215\2253", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
