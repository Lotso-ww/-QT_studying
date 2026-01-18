#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 给主窗口添加一个子窗口
    QDockWidget* dockWidget = new QDockWidget();
    this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    // 浮动窗口也是可以设置标题的
    dockWidget->setWindowTitle("这是浮动窗口");

    // 给浮动窗口内部添加一些其他的控件
    // 不能直接给这个浮动窗口添加子控件，而是需要创建出一个单独的QWidget
    // 把要添加的控件放入其中，然后再设置到dockWidget
    QWidget* container = new QWidget();
    dockWidget->setWidget(container);

    // 创建垂直布局管理器,把布局管理器设置到QWidget中
    QVBoxLayout* layout = new QVBoxLayout();
    container->setLayout(layout);

    // 创建其他控件放入布局管理器中
    QLabel* label = new QLabel("这是一个QLabel");
    QPushButton* button = new QPushButton("按钮");
    layout->addWidget(label);
    layout->addWidget(button);

    // 设置浮动窗口允许停靠的位置
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);

}

MainWindow::~MainWindow()
{
    delete ui;
}

