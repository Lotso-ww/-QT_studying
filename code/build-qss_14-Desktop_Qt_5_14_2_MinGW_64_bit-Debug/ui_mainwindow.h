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
        MainWindow->setStyleSheet(QString::fromUtf8("/*QMenuBar{\n"
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
"}*/\n"
"\n"
"/* \350\217\234\345\215\225\346\240\217\350\203\214\346\231\257\357\274\232\345\236\202\347\233\264\346\270\220\345\217\230\357\274\210\346\265\205\347\201\260\342\206\222\346\267\261\347\201\260\357\274\211 */\n"
"QMenuBar {\n"
"    background-color: qlineargradient(\n"
"        x1: 0, y1: 0, x2: 0, y2: 1,\n"
"        stop: 0 lightgray,\n"
"        stop: 1 darkgray\n"
"    );\n"
"    spacing: 3px; /* \350\217\234\345\215\225\351\241\271\351\227\264\350\267\235 */\n"
"}\n"
""
                        "\n"
"/* \350\217\234\345\215\225\351\241\271\351\273\230\350\256\244\346\240\267\345\274\217 */\n"
"QMenuBar::item {\n"
"    padding: 1px 4px; /* \345\206\205\350\276\271\350\267\235 */\n"
"    background: transparent; /* \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border-radius: 4px; /* \345\234\206\350\247\222 */\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\351\241\271\351\200\211\344\270\255\357\274\210\351\274\240\346\240\207\346\202\254\346\265\256/\351\200\211\344\270\255\357\274\211 */\n"
"QMenuBar::item:selected {\n"
"    background-color: #a8a8a8;\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\351\241\271\346\214\211\345\216\213 */\n"
"QMenuBar::item:pressed {\n"
"    background-color: #888888;\n"
"}\n"
"\n"
"/* \344\270\213\346\213\211\350\217\234\345\215\225\350\203\214\346\231\257+\351\227\264\350\267\235 */\n"
"QMenu {\n"
"    background-color: white;\n"
"    margin: 0 2px; /* \350\217\234\345\215\225\350\276\271\347\274\230\351\227\264\350\267\235 */\n"
"    border: 1px solid #ddd; /* \350"
                        "\217\234\345\215\225\350\276\271\346\241\206 */\n"
"}\n"
"\n"
"/* \344\270\213\346\213\211\350\217\234\345\215\225\351\241\271\346\240\267\345\274\217 */\n"
"QMenu::item {\n"
"    padding: 2px 25px 2px 20px; /* \345\206\205\350\276\271\350\267\235\357\274\232\345\267\246\345\217\263\351\242\204\347\225\231\347\251\272\351\227\264 */\n"
"    border: 3px solid transparent; /* \351\200\211\344\270\255\350\276\271\346\241\206\351\242\204\347\225\231\347\251\272\351\227\264 */\n"
"}\n"
"\n"
"/* \344\270\213\346\213\211\350\217\234\345\215\225\351\241\271\351\200\211\344\270\255 */\n"
"QMenu::item:selected {\n"
"    border-color: darkblue; /* \351\200\211\344\270\255\350\276\271\346\241\206 */\n"
"    background-color: rgba(100, 100, 100, 0.1); /* \345\215\212\351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\345\210\206\351\232\224\347\272\277 */\n"
"QMenu::separator {\n"
"    height: 2px; /* \345\210\206\351\232\224\347\272\277\351\253\230\345\272\246 */\n"
"    backgrou"
                        "nd-color: lightblue; /* \345\210\206\351\232\224\347\272\277\351\242\234\350\211\262 */\n"
"    margin-left: 10px; /* \345\267\246\351\227\264\350\267\235 */\n"
"    margin-right: 5px; /* \345\217\263\351\227\264\350\267\235 */\n"
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
        menubar->setGeometry(QRect(0, 0, 800, 18));
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
