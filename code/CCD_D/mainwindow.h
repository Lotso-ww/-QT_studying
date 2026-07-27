#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QStringList>
#include <QList>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>
#include "camerathread.h"
#include "dbmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SaveWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btn_connect_clicked();
    void on_btn_snap_clicked();
    void on_btn_live_clicked();
    void on_doubleSpinBox_editingFinished();   // 曝光
    void on_doubleSpinBox_2_editingFinished(); // 增益
    void on_btn_save_clicked();                // 停止保存
    void on_btn_openDir_clicked();             // 打开保存目录

    // Tab2
    void on_btn_load_clicked();
    void on_btn_prev_clicked();
    void on_btn_next_clicked();
    void on_btn_play_clicked();
    void on_combo_session_currentIndexChanged(int index);  // 会话切换
    void on_comboBox_currentIndexChanged(int index);  // 0=1x 1=2x 2=3x 3=4x
    void on_slider_progress_sliderMoved(int position);
    void onPlaybackTimeout();

    void displayLiveImage(const QImage &img);
    void onImageSavedToDb(qint64 logId, const QString &path);  // SaveWorker 落盘+写库完成后通知
    void onSaveLimitReached(int savedCount, const QString& saveDir);
    void onSaveFailed(const QString& reason);

private:
    Ui::MainWindow *ui;
    std::shared_ptr<peak::core::Device>     m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap>    m_nodeMapRemoteDevice;
    CameraThread* m_cameraThread;

    // 落盘独立线程: 抓帧线程把图丢过来, SaveWorker 在自己线程做 PNG 编码
    SaveWorker* m_saveWorker;
    QThread*    m_saveThread;

    QImage m_lastImage;

    // Tab2
    QList<qint64> m_imageIds;      // 每张图对应的 image_data.id (方案B: 从 BLOB 解出图)
    QStringList  m_imagePaths;    // 路径(仅显示, 不再用于读图)
    int          m_currentIndex;
    QTimer      *m_playTimer;
    int          m_playIntervalMs;
    int          m_playStep;
    bool         m_isPlaying;
    bool         m_manualStop = false;   // true=用户手动点了"停止保存", 抑制 saveLimitReached 弹窗
    bool         m_saveActive = false;  // true=保存正在进行(从开始到worker完成), 主线程自己的状态

    QString ensureSaveDir();
    void saveSnapImage(const QImage &img);
    void showImageAt(int index);
    void refreshSessionCombo();               // 刷新会话下拉
    void loadImagesFromCurrentSelection();    // 按下拉所选会话从库读路径
    void refreshSliderAndTimer();
    void stopPlayback();
    void startPlayback();
};

#endif // MAINWINDOW_H