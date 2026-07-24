#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cameraThread(nullptr) // 初始化为空
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
    // 窗口关闭时，如果线程还在跑，必须强行刹车并清理内存
    if(m_cameraThread && m_cameraThread->isRunning())
    {
        m_cameraThread->stop();
        m_cameraThread->wait();
        delete m_cameraThread;
        m_cameraThread = nullptr;
    }
    delete ui;
}

// 连接相机 -- 基础工作
void MainWindow::on_btn_connect_clicked()
{
    try
    {
        peak::Library::Initialize();
        auto& deviceManager = peak::DeviceManager::Instance();
        deviceManager.Update();

        if (deviceManager.Devices().empty())
        {
            QMessageBox::warning(this, "连接失败", "未找到可用相机！");
            return;
        }

        // 1. 获取第一台相机的描述信息
        auto deviceDescriptor = deviceManager.Devices().at(0);

        // 2. 真正打开设备，获取控制权，并交给智能指针管理
        m_device = deviceDescriptor->OpenDevice(peak::core::DeviceAccessType::Control);

        // 3. 获取相机的参数节点树 (用于后续修改曝光、增益)
        m_nodeMapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

        // 4. 打开相机的默认数据流通道 (相当于接上水管)
        m_dataStream = m_device->DataStreams().at(0)->OpenDataStream();

        QString qModelName = QString::fromStdString(deviceDescriptor->ModelName());
        QMessageBox::information(this, "连接成功", "成功打开相机并建立数据流: \n" + qModelName);

        // 防止重复打开，连接成功后禁用连接按钮
        ui->btn_connect->setEnabled(false);

    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "严重错误", QString("相机打开失败: %1").arg(e.what()));
    }
}

void MainWindow::on_btn_snap_clicked()
{
    // 0. 安全防御：检查是否真的连接了相机
    if (!m_device || !m_dataStream) {
        QMessageBox::warning(this, "警告", "请先连接相机！");
        return;
    }

    try {
        // ==========================================
        // 第一阶段：分配内存与启动曝光
        // ==========================================

        // 1. 查询相机当前图像大小 (Payload Size)
        auto payloadSize = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

        // 2. 在内存中申请一个 Buffer (缓冲池)

        // 2.1 强制清空之前报错残留的“坏水桶”，防止内存死锁
        m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);
        for (const auto& buf : m_dataStream->AnnouncedBuffers())
        {
            m_dataStream->RevokeBuffer(buf);
        }

        // 2.2 动态询问底层驱动：你最少需要几个水桶才肯开水龙头？
        auto minBufferCount = m_dataStream->NumBuffersAnnouncedMinRequired();
        // 如果它说不需要，我们也强制至少给 1 个；通常它会要 2 到 3 个
        minBufferCount = std::max<size_t>(minBufferCount, 1);

        // 2.3 按相机要求的数量，批量购买空水桶
        for (size_t i = 0; i < minBufferCount; ++i)
        {
            m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
        }

        // 2.4 把所有申请好的水桶，一口气全推到相机的接水队列里！
        for (const auto
                 & buf : m_dataStream->AnnouncedBuffers()) {
            m_dataStream->QueueBuffer(buf);
        }

        // 3. 开启数据流通道，并向相机发送“开始曝光”指令
        m_dataStream->StartAcquisition();
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();

        // ==========================================
        // 第二阶段：死等数据 (同步阻塞)
        // ==========================================

        // 4. 主线程停在这里死死等待，最多等 5000 毫秒。直到相机把图拍好塞进内存
        auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

        // ==========================================
        // 第三阶段：图像转换与 UI 渲染
        // ==========================================

        // 5. 将内存里的裸数据 (Raw) 包装成 IPL 图像对象
        // 彻底的类型强制对齐：精准打击 void* 指针和整数位宽不匹配的问题
        peak::ipl::Image iplRawImage(peak::ipl::PixelFormat(static_cast<peak::ipl::PixelFormatName>(buffer->PixelFormat())),
            static_cast<uint8_t*>(buffer->BasePtr()),
            static_cast<size_t>(buffer->Size()),
            static_cast<size_t>(buffer->Width()),
            static_cast<size_t>(buffer->Height())
        );

        // 6. 调用 IPL 算法库转码
        // 必须用 peak::ipl::PixelFormat(...) 把枚举包裹成对象
        auto
            iplRGBImage = iplRawImage.ConvertTo(peak::ipl::PixelFormat(peak::ipl::PixelFormatName::RGB8));

        // 7. 将底层的 RGB 数据拷贝给 Qt 的 QImage
        // 必须用 static_cast 严格对齐指针类型 (uchar*) 和宽高类型 (int)
        QImage qImg(static_cast<uchar*>(iplRGBImage.PixelPointer(0, 0)),
        static_cast<int>(iplRGBImage.Width()),static_cast<int>(iplRGBImage.Height()),QImage::Format_RGB888);

        // 8. 渲染到 Tab 1 的大黑框上！
        ui->label_live_display->setPixmap(QPixmap::fromImage(qImg));

        // ==========================================
        // 第四阶段：打扫战场 (极其重要，否则内存泄漏)
        // ==========================================

        // 9. 停止相机曝光，清空数据流，释放内存
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        m_dataStream->StopAcquisition();
        m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

        for (const auto& buf : m_dataStream->AnnouncedBuffers()) {
            m_dataStream->RevokeBuffer(buf);
        }

    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "抓图失败", QString("发生异常: %1").arg(e.what()));
    }
}


// ==========================================
// 专门负责贴图的槽函数（运行在主线程，绝对安全）
// ==========================================
void MainWindow::displayLiveImage(const QImage &img)
{
    // 将收到的图像等比例缩放，并贴到大黑框上
    ui->label_live_display->setPixmap(QPixmap::fromImage(img).scaled(
        ui->label_live_display->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
}

// ==========================================
// “实时视频流” 按钮的逻辑 (开启/停止 子线程)
// ==========================================
void MainWindow::on_btn_live_clicked()
{
    // 0. 防御：确保相机已经连接
    if (!m_device || !m_dataStream) {
        QMessageBox::warning(this, "警告", "请先连接相机！");
        return;
    }

    // 逻辑分支 A：如果当前正在播放，那么点击按钮就是“停止”
    if (m_cameraThread && m_cameraThread->isRunning()) {
        m_cameraThread->stop();
        m_cameraThread->wait(); // 等待子线程安全退出

        delete m_cameraThread;
        m_cameraThread = nullptr;

        ui->btn_live->setText("实时视频流"); // 按钮文字恢复
        ui->btn_snap->setEnabled(true);     // 恢复单帧抓图按钮
        return;
    }

    // 逻辑分支 B：如果当前是停止状态，那么点击按钮就是“启动”
    try {
        // 1. 创建子线程工厂，把水管和开关交接过去
        m_cameraThread = new CameraThread(m_dataStream, m_nodeMapRemoteDevice, this);

        // 2. 核心缝合：把子线程发射的 imageReady 信号，连接到主界面的 displayLiveImage 槽函数上！
        connect(m_cameraThread, &CameraThread::imageReady, this, &MainWindow::displayLiveImage);

        // 3. 启动流水线！
        m_cameraThread->start();

        // 4. 更新 UI 状态
        ui->btn_live->setText("停止实时流");
        ui->btn_snap->setEnabled(false); // 实时视频期间禁止单张抓图，防止抢夺数据流

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "严重错误", QString("无法启动视频流: %1").arg(e.what()));
    }
}


