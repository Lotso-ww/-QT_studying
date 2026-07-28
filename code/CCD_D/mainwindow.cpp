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
    , m_saveWorker(nullptr)
    , m_saveThread(nullptr)
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

    ui->doubleSpinBox->setRange(50.0, 30000.0);
    ui->doubleSpinBox->setSingleStep(100.0);
    ui->doubleSpinBox->setSuffix(" μs");
    ui->doubleSpinBox->setValue(5000.0);

    ui->doubleSpinBox_2->setRange(0.0, 36.0);
    ui->doubleSpinBox_2->setSingleStep(0.5);
    ui->doubleSpinBox_2->setSuffix(" dB");
    ui->doubleSpinBox_2->setValue(0.0);

    m_playTimer = new QTimer(this);
    m_playTimer->setInterval(m_playIntervalMs);
    connect(m_playTimer, &QTimer::timeout, this, &MainWindow::onPlaybackTimeout);

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

    m_saveWorker = new SaveWorker;
    m_saveThread = new QThread(this);
    m_saveWorker->moveToThread(m_saveThread);
    connect(m_saveWorker, &SaveWorker::imageSaved,        this, &MainWindow::onImageSavedToDb,  Qt::QueuedConnection);
    connect(m_saveWorker, &SaveWorker::saveFailed,         this, &MainWindow::onSaveFailed,      Qt::QueuedConnection);
    connect(m_saveWorker, &SaveWorker::saveLimitReached,   this, &MainWindow::onSaveLimitReached, Qt::QueuedConnection);
    m_saveThread->start();
    QMetaObject::invokeMethod(m_saveWorker, "initDb",
        Qt::QueuedConnection,
        Q_ARG(QString, dbPath),
        Q_ARG(qint64, DbManager::currentSessionId()));

    refreshSessionCombo();
    connect(ui->combo_session, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_combo_session_currentIndexChanged);

    if (ui->combo_session->view()) {
        ui->combo_session->view()->setMinimumWidth(420);
    }
}


MainWindow::~MainWindow()
{
    // 1. 停相机子线程
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

    // 2. 停 SaveWorker 线程
    if (m_saveThread) {
        m_saveWorker->cancelSave();
        QMetaObject::invokeMethod(m_saveWorker, "clearQueue", Qt::BlockingQueuedConnection);
        QMetaObject::invokeMethod(m_saveWorker, "closeDb",   Qt::BlockingQueuedConnection);
        m_saveThread->quit();
        m_saveThread->wait(3000);
    }
    delete m_saveWorker;  m_saveWorker = nullptr;
    delete m_saveThread;  m_saveThread = nullptr;

    if (m_playTimer)
        m_playTimer->stop();

    // 3. 关数据库 (必须在 SDK 清理之前)
    DbManager::close();

    // 4. SDK 清理
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

void MainWindow::saveSnapImage(const QImage &img)
{
    if (!m_saveWorker) {
        qWarning() << "SaveWorker 未初始化, 无法保存";
        return;
    }
    QString dir = ensureSaveDir();
    QString path = dir + "/ccd_snap_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz") + ".png";
    QMetaObject::invokeMethod(m_saveWorker, "doSaveSingle",
        Qt::QueuedConnection,
        Q_ARG(QImage, img),
        Q_ARG(QString, path));
}

void MainWindow::displayLiveImage(const QImage &img)
{
    // FastTransformation: 邻近采样比 SmoothTransformation 快 5~10 倍, 实时画面不掉帧
    m_lastImage = img.copy();
    ui->label_live_display->setPixmap(QPixmap::fromImage(img).scaled(
        ui->label_live_display->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

void MainWindow::on_btn_live_clicked()
{
    if (!m_device || !m_dataStream) {
        QMessageBox::warning(this, "警告", "请先连接相机");
        return;
    }

    if (m_cameraThread && m_cameraThread->isRunning()) {
        m_cameraThread->stop();
        m_cameraThread->wait();

        m_saveWorker->cancelSave();
        QMetaObject::invokeMethod(m_saveWorker, "clearQueue", Qt::QueuedConnection);
        m_manualStop = true;
        m_saveActive = false;

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
        connect(m_cameraThread, &CameraThread::imageReady,     this, &MainWindow::displayLiveImage,     Qt::QueuedConnection);
        connect(m_cameraThread, &CameraThread::imageToSave,    m_saveWorker, &SaveWorker::doSave,        Qt::QueuedConnection);

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

void MainWindow::onSaveLimitReached(int savedCount, const QString& saveDir)
{
    ui->btn_save->setText("回调异步");
    m_saveActive = false;
    if (m_manualStop) {
        statusBar()->showMessage(QString("已手动停止, 共保存 %1 张").arg(savedCount), 5000);
        return;
    }
    statusBar()->showMessage(QString("已保存 %1 张到 %2, 自动停止").arg(savedCount).arg(saveDir), 8000);
    QMessageBox::information(this, "保存完成",
        QString("已保存 %1 张图像到:\n%2\n点击\"回调异步\"可再次保存一轮。").arg(savedCount).arg(saveDir));
}

void MainWindow::onSaveFailed(const QString& reason)
{
    qWarning() << reason;
    statusBar()->showMessage(reason, 3000);
}

void MainWindow::onImageSavedToDb(qint64 logId, const QString &path)
{
    if (logId > 0) {
        statusBar()->showMessage(QString("已保存并写库, ID=%1").arg(logId), 5000);
    } else {
        qWarning() << "写库失败 path=" << path;
        statusBar()->showMessage("图像已落盘, 但写库失败", 5000);
    }
}

void MainWindow::on_btn_save_clicked()
{
    if (!m_cameraThread || !m_cameraThread->isRunning()) {
        ui->btn_save->setEnabled(false);
        return;
    }

    // 用 m_saveActive 判断状态, 不能用 m_cameraThread->isSaving()
    // 因为相机线程到达上限后 m_saveEnabled=false, 但 worker 可能还在处理队列
    if (m_saveActive) {
        m_cameraThread->stopSave();
        m_saveWorker->cancelSave();
        QMetaObject::invokeMethod(m_saveWorker, "clearQueue", Qt::QueuedConnection);
        m_manualStop = true;
        m_saveActive = false;

        statusBar()->showMessage(QString("已手动停止, 共保存 %1 张").arg(m_cameraThread->savedCount()), 5000);
        ui->btn_save->setText("回调异步");
        return;
    }

    int limit = ui->spinBox_saveLimit->value();
    QString dir = ensureSaveDir();
    m_saveWorker->setupSave(limit, dir);
    m_cameraThread->enableSave(dir, limit);
    m_manualStop = false;
    m_saveActive = true;

    ui->btn_save->setText("停止保存");
    statusBar()->showMessage(QString("回调异步已开启, 上限 %1 张, 保存到 %2").arg(limit).arg(dir), 5000);
}

void MainWindow::on_btn_openDir_clicked()
{
    QString dir = ensureSaveDir();
    if (!QDir(dir).exists()) {
        QMessageBox::warning(this, "提示", "保存目录尚不存在:\n" + dir);
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}


void MainWindow::on_btn_load_clicked()
{
    refreshSessionCombo();
}

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
        // 会话#5  2026-07-27 09:42  (单:2 异:26)
        QString label = QString("会话#%1  %2  (单:%3 异:%4)")
                            .arg(s.id)
                            .arg(s.startTime.left(16))
                            .arg(s.singleCount)
                            .arg(s.asyncCount);
        ui->combo_session->addItem(label, s.id);

        if (s.id == curSid) selectRow = i + 1;
    }

    if (selectRow == 0 && sessions.size() > 0) selectRow = 1;

    ui->combo_session->setCurrentIndex(selectRow);
    ui->combo_session->blockSignals(false);

    for (int i = 0; i < ui->combo_session->count(); ++i) {
        ui->combo_session->setItemData(i, ui->combo_session->itemText(i), Qt::ToolTipRole);
    }

    loadImagesFromCurrentSelection();
}

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

void MainWindow::on_combo_session_currentIndexChanged(int)
{
    loadImagesFromCurrentSelection();
}

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