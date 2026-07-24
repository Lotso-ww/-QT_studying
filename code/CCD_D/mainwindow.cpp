#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. 初始化 IDS Peak 库
    try
    {
        peak::Library::Initialize();
        qDebug() << "IDS Peak SDK 初始化成功！";
    }
    catch (const std::exception& e)
    {
        qDebug() << "SDK 初始化失败:" << e.what();
        QMessageBox::critical(this, "错误", "无法初始化相机 SDK！");
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_connect_clicked()
{
    try
    {
        // 1. 初始化 IDS peak 底层库 (极其重要，必须第一步调用)
        peak::Library::Initialize();

        // 2. 获取设备管理器并刷新物理接口
        auto& deviceManager = peak::DeviceManager::Instance();
        deviceManager.Update();

        // 3. 检查是否探测到任何相机
        if (deviceManager.Devices().empty())
        {
            QMessageBox::warning(this, "连接失败", "未找到任何可用相机，请检查电源和数据线！");
            return;
        }

        // 4. 提取第一台相机的型号名称，并弹窗提示成功
        std::string modelName = deviceManager.Devices().at(0)->ModelName();
        QString qModelName = QString::fromStdString(modelName);

        QMessageBox::information(this, "连接成功", "成功探测到相机: \n" + qModelName);

        // 按钮状态反转 (防止重复点击)
        // ui->btn_connect->setEnabled(false);

    }
    catch (const std::exception& e)
    {
        // 捕获任何底层驱动抛出的异常
        QMessageBox::critical(this, "严重错误", QString("相机初始化引发异常: %1").arg(e.what()));
    }
}

