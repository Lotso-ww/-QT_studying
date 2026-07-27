#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"

#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QFile>
#include <QAbstractItemView>
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

    ensureSaveDir();

    // 初始化数据库
    QString dbPath = ensureSaveDir() + "/capture.db";
    if (DbManager::init(dbPath)) {
        statusBar()->showMessage(QString("数据库就绪: %1").arg(QDir::toNativeSeparators(dbPath)), 8000);
        qDebug() << "数据库就绪:" << QDir::toNativeSeparators(dbPath);

        qint64 sid = DbManager::createSession(
            QString("启动于 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
        if (sid > 0)
            qDebug() << "本次会话 ID =" << sid;
    } else {
        QMessageBox::warning(this, "警告",
            "数据库初始化失败, 抓拍数据将仅保存为文件, 不写数据库。\n路径: " + dbPath);
    }

    refreshSessionCombo();
    connect(ui->combo_session, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_combo_session_currentIndexChanged);

    // 让下拉弹出列表(popup)的宽度自适应内容, 即使控件本身很窄, 展开时也能看到完整文字
    if (ui->combo_session->view()) {
        ui->combo_session->view()->setMinimumWidth(420);   // 弹出列表固定 420px 宽, 足够显示完整会话信息
    }
}


MainWindow::~MainWindow()
{
    // 1. 先停相机子线程 (它会 emit 信号让主线程写库, 必须在关库前停掉)
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
    if (m_playTimer)
        m_playTimer->stop();

    // 2. 关数据库 — 必须在 SDK 清理之前!
    //    如果 SDK 析构触发 debug assertion / abort(), 上面那些来不及跑,
    //    但数据库已经关好了, 下次启动就不会 "database is locked"
    DbManager::close();

    // 3. SDK 清理 — 用 catch(...) 兜底所有异常, 避免非 std::exception 类型导致 abort
    try { m_dataStream.reset(); }       catch (...) { qDebug() << "dataStream 释放异常"; }
    try { m_nodeMapRemoteDevice.reset(); } catch (...) { qDebug() << "nodeMap 释放异常"; }
    try { m_device.reset(); }          catch (...) { qDebug() << "device 释放异常"; }
    try { peak::Library::Close(); }     catch (...) { qDebug() << "Library::Close 异常"; }

    delete ui;
}

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

        if (deviceManager.Devices().empty()) {
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

// 曝光 (μs)
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

// 单张抓图
//  - live 渲染中: 截取当前预览帧
//  - 非 live:    同步抓一张
void MainWindow::on_btn_snap_clicked()
{
    if (!m_device || !m_dataStream) {
        QMessageBox::warning(this, "警告", "请先连接相机");
        return;
    }

    if (m_cameraThread && m_cameraThread->isRunning()) {
        if (m_lastImage.isNull()) {
            statusBar()->showMessage("预览尚未就绪, 请稍候", 3000);
            return;
        }
        saveSnapImage(m_lastImage);
        return;
    }

    try {
        auto payloadSize = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();
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

// 落盘 + 写库 (ModeSingle=0)
void MainWindow::saveSnapImage(const QImage &img)
{
    QString dir = ensureSaveDir();
    QString path = dir + "/ccd_snap_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz") + ".png";
    if (img.save(path))
    {
        qint64 logId = DbManager::insertCapture(img, path, ModeSingle);
        if (logId > 0) {
            qDebug() << "单张保存+写库成功, path=" << path << " log_id=" << logId;
            statusBar()->showMessage(QString("单张已保存并写库, ID=%1").arg(logId), 5000);
        } else {
            qWarning() << "单张保存成功但写库失败, path=" << path;
            statusBar()->showMessage("图像已落盘, 但写库失败", 5000);
        }
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

    try {
        m_cameraThread = new CameraThread(m_dataStream, m_nodeMapRemoteDevice, this);
        connect(m_cameraThread, &CameraThread::imageReady, this, &MainWindow::displayLiveImage);
        connect(m_cameraThread, &CameraThread::imageSaved, this, &MainWindow::onImageSavedToDb);
        connect(m_cameraThread, &CameraThread::saveLimitReached, this, &MainWindow::onSaveLimitReached);
        connect(m_cameraThread, &CameraThread::saveFailed, this, &MainWindow::onSaveFailed);

        m_cameraThread->start();

        ui->btn_live->setText("停止实时流");
        ui->btn_snap->setEnabled(true);
        ui->btn_save->setEnabled(true);
        ui->btn_save->setText("回调异步");

        statusBar()->showMessage("实时渲染已开启, 可点\"回调异步\"开始持续保存", 5000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "严重错误", QString("无法启动视频流: %1").arg(e.what()));
    }
}

// 保存达到上限
void MainWindow::onSaveLimitReached(int savedCount, const QString& saveDir)
{
    statusBar()->showMessage(QString("已保存 %1 张到 %2, 自动停止").arg(savedCount).arg(saveDir), 8000);
    ui->btn_save->setText("回调异步");
    QMessageBox::information(this, "保存完成",
        QString("已保存 %1 张图像到:\n%2\n点击\"回调异步\"可再次保存一轮。").arg(savedCount).arg(saveDir));
}

void MainWindow::onSaveFailed(const QString& reason)
{
    qWarning() << reason;
    statusBar()->showMessage(reason, 3000);
}

// 异步落盘后主线程写库
void MainWindow::onImageSavedToDb(const QImage &img, const QString &path)
{
    qint64 logId = DbManager::insertCapture(img, path, ModeAsync);
    if (logId > 0) {
        qDebug() << "[异步] 写库成功 log_id=" << logId << "path=" << path;
    } else {
        qWarning() << "[异步] 写库失败 log_id=" << logId << "path=" << path;
    }
}

// 回调异步保存开关
void MainWindow::on_btn_save_clicked()
{
    if (!m_cameraThread || !m_cameraThread->isRunning()) {
        ui->btn_save->setEnabled(false);
        return;
    }

    if (m_cameraThread->isSaving()) {
        m_cameraThread->stopSave();
        statusBar()->showMessage(QString("已手动停止, 共保存 %1 张").arg(m_cameraThread->savedCount()), 5000);
        ui->btn_save->setText("回调异步");
        return;
    }

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
    if (!QDir(dir).exists()) {
        QMessageBox::warning(this, "提示", "保存目录尚不存在:\n" + dir);
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

// ==================== Tab2: 浏览/播放 ====================

// 刷新会话下拉并加载当前所选
void MainWindow::on_btn_load_clicked()
{
    refreshSessionCombo();
}

// 刷新会话下拉框: 第一项=全部图片, 后面=历史会话(倒序), 默认选中本次会话
void MainWindow::refreshSessionCombo()
{
    if (!ui->combo_session) return;

    ui->combo_session->blockSignals(true);
    ui->combo_session->clear();

    const auto sessions = DbManager::listSessions();
    const qint64 curSid = DbManager::currentSessionId();

    ui->combo_session->addItem("【全部图片】", qint64(-1));

    int selectRow = 0;
    for (int i = 0; i < sessions.size(); ++i)
    {
        const SessionInfo &s = sessions[i];
        // 紧凑显示: 会话#5  2026-07-27 09:42  (单:2 异:26)
        QString label = QString("会话#%1  %2  (单:%3 异:%4)")
                            .arg(s.id)
                            .arg(s.startTime.left(16))
                            .arg(s.singleCount)
                            .arg(s.asyncCount);
        ui->combo_session->addItem(label, s.id);

        if (s.id == curSid) selectRow = i + 1;
    }

    // 找不到当前会话时默认选第一个历史会话 (而不是"全部")
    if (selectRow == 0 && sessions.size() > 0) selectRow = 1;

    ui->combo_session->setCurrentIndex(selectRow);
    ui->combo_session->blockSignals(false);

    // 给每项设置 tooltip, 下拉框再宽也容纳不下长文本时悬停可看全
    for (int i = 0; i < ui->combo_session->count(); ++i) {
        ui->combo_session->setItemData(i, ui->combo_session->itemText(i), Qt::ToolTipRole);
    }

    loadImagesFromCurrentSelection();
}

// 按下拉所选 session id 加载图像 (方案B: 只取 id, 展示时按 id 读 BLOB)
void MainWindow::loadImagesFromCurrentSelection()
{
    qint64 sid = ui->combo_session->currentData().toLongLong();
    QList<ImageRow> rows = DbManager::listImagesBySession(sid);

    m_imageIds.clear();
    m_imagePaths.clear();
    for (const ImageRow &r : rows) {
        m_imageIds.append(r.id);
        m_imagePaths.append(r.path);
    }

    if (m_imageIds.isEmpty())
    {
        ui->label->setText("该会话暂无图片");
        ui->slider_progress->setEnabled(false);
        ui->btn_prev->setEnabled(false);
        ui->btn_next->setEnabled(false);
        ui->btn_play->setEnabled(false);
        ui->timer->setText("0 / 0");
        return;
    }

    m_currentIndex = 0;
    ui->slider_progress->setEnabled(true);
    ui->slider_progress->setMinimum(0);
    ui->slider_progress->setMaximum(m_imageIds.size() - 1);
    ui->slider_progress->setValue(0);
    ui->btn_prev->setEnabled(m_imageIds.size() > 1);
    ui->btn_next->setEnabled(m_imageIds.size() > 1);
    ui->btn_play->setEnabled(m_imageIds.size() > 1);

    showImageAt(0);
}

// 切换会话下拉
void MainWindow::on_combo_session_currentIndexChanged(int)
{
    loadImagesFromCurrentSelection();
}

// 展示指定下标的图 (方案B: 直接从数据库 BLOB 读)
void MainWindow::showImageAt(int index)
{
    if (index < 0 || index >= m_imageIds.size()) return;

    QImage img = DbManager::loadImageById(m_imageIds.at(index));
    if (img.isNull())
    {
        qWarning() << "无法从数据库解码图像, image_data.id=" << m_imageIds.at(index);
        ui->label->setText("图像解码失败 (id=" + QString::number(m_imageIds.at(index)) + ")");
        return;
    }

    ui->label->setPixmap(QPixmap::fromImage(img).scaled(
        ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_currentIndex = index;
    refreshSliderAndTimer();

    if (index < m_imagePaths.size()) {
        statusBar()->showMessage(QString("当前: %1").arg(m_imagePaths.at(index)), 3000);
    }
}

void MainWindow::refreshSliderAndTimer()
{
    if (m_imageIds.isEmpty()) return;

    ui->slider_progress->blockSignals(true);
    ui->slider_progress->setValue(m_currentIndex);
    ui->slider_progress->blockSignals(false);

    ui->timer->setText(QString("%1 / %2")
        .arg(m_currentIndex + 1, 3, 10, QChar('0'))
        .arg(m_imageIds.size(), 3, 10, QChar('0')));
}

void MainWindow::on_btn_prev_clicked()
{
    if (m_imageIds.isEmpty()) return;
    int idx = m_currentIndex - 1;
    if (idx < 0) idx = m_imageIds.size() - 1;
    showImageAt(idx);
}

void MainWindow::on_btn_next_clicked()
{
    if (m_imageIds.isEmpty()) return;
    int idx = m_currentIndex + 1;
    if (idx >= m_imageIds.size()) idx = 0;
    showImageAt(idx);
}

void MainWindow::on_btn_play_clicked()
{
    if (m_imageIds.isEmpty()) return;
    m_isPlaying ? stopPlayback() : startPlayback();
}

void MainWindow::startPlayback()
{
    if (m_imageIds.size() <= 1) return;
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
    if (m_imageIds.isEmpty()) return;
    int idx = m_currentIndex + m_playStep;
    while (idx >= m_imageIds.size())
        idx -= m_imageIds.size();
    showImageAt(idx);
}

// 倍速: 1x 125ms/1帧 | 2x 125ms/2帧 | 3x 90ms/2帧 | 4x 71ms/2帧
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