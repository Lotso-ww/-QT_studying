#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cameraThread(nullptr)
    , m_currentIndex(-1)
    , m_playTimer(nullptr)
    , m_playIntervalMs(125)
    , m_playStep(1)
    , m_isPlaying(false)
{
    ui->setupUi(this);

    try
    {
        peak::Library::Initialize();
        qDebug() << "IDS Peak SDK 初始化成功";
    }
    catch (const std::exception& e)
    {
        qDebug() << "SDK 初始化失败:" << e.what();
        QMessageBox::critical(this, "错误", "无法初始化相机 SDK");
    }

    // 曝光/增益输入框
    ui->doubleSpinBox->setRange(50.0, 30000.0);
    ui->doubleSpinBox->setSingleStep(100.0);
    ui->doubleSpinBox->setSuffix(" μs");
    ui->doubleSpinBox->setValue(5000.0);

    ui->doubleSpinBox_2->setRange(0.0, 36.0);
    ui->doubleSpinBox_2->setSingleStep(0.5);
    ui->doubleSpinBox_2->setSuffix(" dB");
    ui->doubleSpinBox_2->setValue(0.0);

    // 播放定时器
    m_playTimer = new QTimer(this);
    m_playTimer->setInterval(m_playIntervalMs);
    connect(m_playTimer, &QTimer::timeout, this, &MainWindow::onPlaybackTimeout);

    // Tab2 按钮
    ui->btn_prev->setText("<");
    ui->btn_play->setText("▶");
    ui->btn_next->setText(">");
    ui->btn_prev->setEnabled(false);
    ui->btn_next->setEnabled(false);
    ui->btn_play->setEnabled(false);
    ui->slider_progress->setEnabled(false);
    ui->comboBox->setCurrentIndex(0);

    ui->btn_save->setEnabled(false);

    // 提前创建保存目录
    ensureSaveDir();
}


MainWindow::~MainWindow()
{
    // 1. 先停子线程 (必须在释放 SDK 资源之前完成, 否则子线程还在用 dataStream 就崩了)
    if (m_cameraThread)
    {
        if (m_cameraThread->isRunning())
        {
            m_cameraThread->stop();
            m_cameraThread->wait();
        }
        delete m_cameraThread;
        m_cameraThread = nullptr;
    }

    // 2. 停定时器
    if (m_playTimer)
        m_playTimer->stop();

    // 3. 释放 SDK 资源 (顺序: dataStream -> nodeMap -> device -> Library)
    try { m_dataStream.reset(); } catch (const std::exception& e) { qDebug() << "dataStream 释放异常:" << e.what(); }
    try { m_nodeMapRemoteDevice.reset(); } catch (const std::exception& e) { qDebug() << "nodeMap 释放异常:" << e.what(); }
    try { m_device.reset(); } catch (const std::exception& e) { qDebug() << "device 释放异常:" << e.what(); }

    try
    {
        peak::Library::Close();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Library::Close 异常:" << e.what();
    }

    delete ui;
}

// 保存目录: exe 所在目录下的 captures/
QString MainWindow::ensureSaveDir()
{
    QString dir = QCoreApplication::applicationDirPath() + "/captures";
    QDir().mkpath(dir);

    if (ui->label_savePath)
        ui->label_savePath->setText("保存目录: " + QDir::toNativeSeparators(dir));
    return dir;
}

// 连接相机
void MainWindow::on_btn_connect_clicked()
{
    try
    {
        auto& deviceManager = peak::DeviceManager::Instance();
        deviceManager.Update();

        if (deviceManager.Devices().empty())
        {
            QMessageBox::warning(this, "连接失败", "未找到可用相机");
            return;
        }

        auto deviceDescriptor = deviceManager.Devices().at(0);
        m_device = deviceDescriptor->OpenDevice(peak::core::DeviceAccessType::Control);
        m_nodeMapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);
        m_dataStream = m_device->DataStreams().at(0)->OpenDataStream();

        QString qModelName = QString::fromStdString(deviceDescriptor->ModelName());
        QMessageBox::information(this, "连接成功", "已打开相机: " + qModelName);

        on_doubleSpinBox_editingFinished();
        on_doubleSpinBox_2_editingFinished();

        ui->btn_connect->setEnabled(false);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "严重错误", QString("相机打开失败: %1").arg(e.what()));
    }
}

// 曝光时间 (μs)
void MainWindow::on_doubleSpinBox_editingFinished()
{
    if (!m_nodeMapRemoteDevice) return;

    try
    {
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ExposureAuto")->SetCurrentEntry("Off");
        double minExp = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")->Minimum();
        double maxExp = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")->Maximum();
        double v = std::clamp(ui->doubleSpinBox->value(), minExp, maxExp);
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")->SetValue(v);
        qDebug() << "曝光时间 =" << v << "μs";
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "参数写入失败", QString("曝光设置失败: %1").arg(e.what()));
    }
}

// 增益 (dB)
void MainWindow::on_doubleSpinBox_2_editingFinished()
{
    if (!m_nodeMapRemoteDevice) return;

    try
    {
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("GainAuto")->SetCurrentEntry("Off");
        double minG = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("Gain")->Minimum();
        double maxG = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("Gain")->Maximum();
        double v = std::clamp(ui->doubleSpinBox_2->value(), minG, maxG);
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("Gain")->SetValue(v);
        qDebug() << "增益 =" << v << "dB";
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "参数写入失败", QString("增益设置失败: %1").arg(e.what()));
    }
}

// 单张抓图: 抓一张并立即保存
//  - live 渲染中: 直接截取当前预览帧 (所见即所得, 不抢 dataStream)
//  - 非 live:    同步抓一张 (走完整流程)
void MainWindow::on_btn_snap_clicked()
{
    if (!m_device || !m_dataStream) {
        QMessageBox::warning(this, "警告", "请先连接相机");
        return;
    }

    // 分支 A: live 渲染中, 直接截取当前预览帧
    if (m_cameraThread && m_cameraThread->isRunning()) {
        if (m_lastImage.isNull()) {
            statusBar()->showMessage("预览尚未就绪, 请稍候", 3000);
            return;
        }
        saveSnapImage(m_lastImage);
        return;
    }

    // 分支 B: 非 live, 同步抓一张
    try {
        auto payloadSize = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

        // 清理残留状态 (防 live 停止后干扰)
        try { m_dataStream->StopAcquisition(); } catch (...) {}
        m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);
        for (const auto& buf : m_dataStream->AnnouncedBuffers())
            m_dataStream->RevokeBuffer(buf);

        size_t minBufferCount = std::max<size_t>(m_dataStream->NumBuffersAnnouncedMinRequired(), 1);
        for (size_t i = 0; i < minBufferCount; ++i)
            m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
        for (const auto& buf : m_dataStream->AnnouncedBuffers())
            m_dataStream->QueueBuffer(buf);

        m_dataStream->StartAcquisition();
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();

        // 丢弃前 2 帧让传感器稳定
        for (int i = 0; i < 2; ++i) {
            auto tmp = m_dataStream->WaitForFinishedBuffer(2000);
            m_dataStream->QueueBuffer(tmp);
        }

        auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

        peak::ipl::Image iplRawImage(
            peak::ipl::PixelFormat(static_cast<peak::ipl::PixelFormatName>(buffer->PixelFormat())),
            static_cast<uint8_t*>(buffer->BasePtr()),
            static_cast<size_t>(buffer->Size()),
            static_cast<size_t>(buffer->Width()),
            static_cast<size_t>(buffer->Height()));

        auto iplRGB = iplRawImage.ConvertTo(peak::ipl::PixelFormat(peak::ipl::PixelFormatName::RGB8));

        QImage qImg(static_cast<uchar*>(iplRGB.PixelPointer(0, 0)),
                    static_cast<int>(iplRGB.Width()),
                    static_cast<int>(iplRGB.Height()),
                    static_cast<int>(iplRGB.Width()) * 3,
                    QImage::Format_RGB888);
        m_lastImage = qImg.copy();

        ui->label_live_display->setPixmap(QPixmap::fromImage(qImg).scaled(
            ui->label_live_display->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        saveSnapImage(m_lastImage);

        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        m_dataStream->StopAcquisition();
        m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);
        for (const auto& buf : m_dataStream->AnnouncedBuffers())
            m_dataStream->RevokeBuffer(buf);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "抓图失败", QString("发生异常: %1").arg(e.what()));
    }
}

// 把图像存到 captures/ 目录
void MainWindow::saveSnapImage(const QImage &img)
{
    QString dir = ensureSaveDir();
    QString path = dir + "/ccd_snap_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz") + ".png";
    if (img.save(path))
    {
        qDebug() << "单张已保存:" << path;
        statusBar()->showMessage(QString("单张已保存: %1").arg(path), 5000);
    }
    else
    {
        QMessageBox::warning(this, "保存失败", "图像保存失败:\n" + path);
    }
}

// 实时渲染
void MainWindow::displayLiveImage(const QImage &img)
{
    m_lastImage = img.copy();
    ui->label_live_display->setPixmap(QPixmap::fromImage(img).scaled(
        ui->label_live_display->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// live 流: 启动/停止
void MainWindow::on_btn_live_clicked()
{
    if (!m_device || !m_dataStream) {
        QMessageBox::warning(this, "警告", "请先连接相机");
        return;
    }

    // 停止
    if (m_cameraThread && m_cameraThread->isRunning()) {
        m_cameraThread->stop();
        m_cameraThread->wait();
        delete m_cameraThread;
        m_cameraThread = nullptr;

        ui->btn_live->setText("实时视频流");
        ui->btn_snap->setEnabled(true);
        ui->btn_save->setEnabled(false);
        ui->btn_save->setText("回调异步");
        return;
    }

    // 启动
    try {
        m_cameraThread = new CameraThread(m_dataStream, m_nodeMapRemoteDevice, this);
        connect(m_cameraThread, &CameraThread::imageReady, this, &MainWindow::displayLiveImage);
        connect(m_cameraThread, &CameraThread::saveLimitReached, this, &MainWindow::onSaveLimitReached);
        connect(m_cameraThread, &CameraThread::saveFailed, this, &MainWindow::onSaveFailed);

        m_cameraThread->start();

        // live 仅做渲染预览; snap 走截预览帧分支, 不抢 dataStream, 仍可用
        ui->btn_live->setText("停止实时流");
        ui->btn_snap->setEnabled(true);
        ui->btn_save->setEnabled(true);
        ui->btn_save->setText("回调异步");

        statusBar()->showMessage("实时渲染已开启, 可点\"回调异步\"开始持续保存", 5000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "严重错误", QString("无法启动视频流: %1").arg(e.what()));
    }
}

// 保存达到上限 (子线程信号触发)
void MainWindow::onSaveLimitReached(int savedCount, const QString& saveDir)
{
    statusBar()->showMessage(QString("已保存 %1 张到 %2, 自动停止").arg(savedCount).arg(saveDir), 8000);
    ui->btn_save->setText("回调异步");   // 允许再来一轮
    QMessageBox::information(this, "保存完成",
        QString("已保存 %1 张图像到:\n%2\n点击\"回调异步\"可再次保存一轮。").arg(savedCount).arg(saveDir));
}

void MainWindow::onSaveFailed(const QString& reason)
{
    qWarning() << reason;
    statusBar()->showMessage(reason, 3000);
}

// 回调异步保存: 开/关 (仅在 live 流运行期间可用)
void MainWindow::on_btn_save_clicked()
{
    if (!m_cameraThread || !m_cameraThread->isRunning()) {
        ui->btn_save->setEnabled(false);
        return;
    }

    // 正在保存 -> 提前停止
    if (m_cameraThread->isSaving()) {
        m_cameraThread->stopSave();
        statusBar()->showMessage(QString("已手动停止, 共保存 %1 张").arg(m_cameraThread->savedCount()), 5000);
        ui->btn_save->setText("回调异步");
        return;
    }

    // 未保存 -> 开启, 持续拍到上限自动停
    int limit = ui->spinBox_saveLimit->value();
    QString dir = ensureSaveDir();
    m_cameraThread->enableSave(dir, limit);

    ui->btn_save->setText("停止保存");
    statusBar()->showMessage(QString("回调异步已开启, 上限 %1 张, 保存到 %2").arg(limit).arg(dir), 5000);
}

// 打开保存目录
void MainWindow::on_btn_openDir_clicked()
{
    QString dir = ensureSaveDir();
    if (!QDir(dir).exists())
    {
        QMessageBox::warning(this, "提示", "保存目录尚不存在:\n" + dir);
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

// ==================== Tab2: 浏览/播放 ====================

void MainWindow::on_btn_load_clicked()
{
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QString dir = QFileDialog::getExistingDirectory(this, "选择图像所在文件夹", defaultDir);
    if (dir.isEmpty()) return;

    QStringList filter = {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff", "*.tif"};
    QDir d(dir);
    d.setNameFilters(filter);
    d.setSorting(QDir::Name);
    QStringList files = d.entryList(QDir::Files);
    if (files.isEmpty())
    {
        QMessageBox::warning(this, "提示", "所选文件夹内没有可识别的图像文件");
        return;
    }

    m_imagePaths.clear();
    for (const QString& f : qAsConst(files))
        m_imagePaths.append(d.absoluteFilePath(f));

    m_currentIndex = 0;
    ui->slider_progress->setEnabled(true);
    ui->slider_progress->setMinimum(0);
    ui->slider_progress->setMaximum(m_imagePaths.size() - 1);
    ui->slider_progress->setValue(0);

    ui->btn_prev->setEnabled(m_imagePaths.size() > 1);
    ui->btn_next->setEnabled(m_imagePaths.size() > 1);
    ui->btn_play->setEnabled(m_imagePaths.size() > 1);

    showImageAt(0);
}

void MainWindow::showImageAt(int index)
{
    if (index < 0 || index >= m_imagePaths.size()) return;

    QImage img(m_imagePaths.at(index));
    if (img.isNull())
    {
        qWarning() << "无法解码图像:" << m_imagePaths.at(index);
        return;
    }

    ui->label->setPixmap(QPixmap::fromImage(img).scaled(
        ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_currentIndex = index;
    refreshSliderAndTimer();
}

void MainWindow::refreshSliderAndTimer()
{
    if (m_imagePaths.isEmpty()) return;

    ui->slider_progress->blockSignals(true);
    ui->slider_progress->setValue(m_currentIndex);
    ui->slider_progress->blockSignals(false);

    ui->timer->setText(QString("%1 / %2")
        .arg(m_currentIndex + 1, 3, 10, QChar('0'))
        .arg(m_imagePaths.size(), 3, 10, QChar('0')));
}

void MainWindow::on_btn_prev_clicked()
{
    if (m_imagePaths.isEmpty()) return;
    int idx = m_currentIndex - 1;
    if (idx < 0) idx = m_imagePaths.size() - 1;
    showImageAt(idx);
}

void MainWindow::on_btn_next_clicked()
{
    if (m_imagePaths.isEmpty()) return;
    int idx = m_currentIndex + 1;
    if (idx >= m_imagePaths.size()) idx = 0;
    showImageAt(idx);
}

void MainWindow::on_btn_play_clicked()
{
    if (m_imagePaths.isEmpty()) return;
    m_isPlaying ? stopPlayback() : startPlayback();
}

void MainWindow::startPlayback()
{
    if (m_imagePaths.size() <= 1) return;
    m_isPlaying = true;
    ui->btn_play->setText("⏸");
    m_playTimer->start(m_playIntervalMs);
}

void MainWindow::stopPlayback()
{
    m_isPlaying = false;
    ui->btn_play->setText("▶");
    m_playTimer->stop();
}

void MainWindow::onPlaybackTimeout()
{
    if (m_imagePaths.isEmpty()) return;
    int idx = m_currentIndex + m_playStep;
    while (idx >= m_imagePaths.size())
        idx -= m_imagePaths.size();
    showImageAt(idx);
}

// 倍速: 1x 125ms/1帧  | 2x 125ms/2帧 | 3x 90ms/2帧 | 4x 71ms/2帧
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    struct Cfg { int ms; int step; };
    static const Cfg cfg[] = { {125,1}, {125,2}, {90,2}, {71,2} };
    int i = (index >= 0 && index < 4) ? index : 0;
    m_playIntervalMs = cfg[i].ms;
    m_playStep       = cfg[i].step;

    if (m_playTimer->isActive())
        m_playTimer->setInterval(m_playIntervalMs);
}

void MainWindow::on_slider_progress_sliderMoved(int position)
{
    showImageAt(position);
}