#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 存在就获取, 不存在就创建
    QStatusBar* statusBar = this->statusBar();
    // 如果已经存在,这样设置没有啥意义,但是也不影响
    this->setStatusBar(statusBar);

    // 放置一个临时信息
    // statusBar->showMessage("这是一个临时信息", 3000);

    // 放置一些控件
    QLabel* label = new QLabel("这是一个Label");
    statusBar->addWidget(label); // 从左往右添加
    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(50);
    statusBar->addWidget(progressBar);
    QPushButton* pushButton = new QPushButton("按钮");
    statusBar->addPermanentWidget(pushButton); // 从右往左添加
}

MainWindow::~MainWindow()
{
    delete ui;
}

