#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenuBar* menuBar = new QMenuBar();
    this->setMenuBar(menuBar);

    QMenu* parentMenu = new QMenu("父菜单");
    QMenu* childMenu1 = new QMenu("子菜单1");
    QMenu* childMenu2 = new QMenu("子菜单2");
    menuBar->addMenu(parentMenu);
    parentMenu->addMenu(childMenu1);
    parentMenu->addMenu(childMenu2);

    QAction* action1 = new QAction("菜单项1");
    QAction* action2 = new QAction("菜单项2");
    childMenu1->addAction(action1);
    childMenu1->addAction(action2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

