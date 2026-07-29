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
    void on_doubleSpinBox_editingFinished();
    void on_doubleSpinBox_2_editingFinished();
    void on_btn_save_clicked();
    void on_btn_openDir_clicked();

    void on_btn_load_clicked();
    void on_btn_prev_clicked();
    void on_btn_next_clicked();
    void on_btn_play_clicked();
    void on_combo_session_currentIndexChanged(int index);
    void on_comboBox_currentIndexChanged(int index);  // 0=1x 1=2x 2=3x 3=4x
    void on_slider_progress_sliderMoved(int position);
    void onPlaybackTimeout();

    void displayLiveImage(const QImage &img);
    void onImageSavedToDb(qint64 logId, const QString &path);
    void onSaveLimitReached(int savedCount, const QString& saveDir);
    void onSaveFailed(const QString& reason);

private:
    Ui::MainWindow *ui;
    std::shared_ptr<peak::core::Device>     m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap>    m_nodeMapRemoteDevice;
    CameraThread* m_cameraThread;

    SaveWorker* m_saveWorker;
    QThread*    m_saveThread;

    QImage m_lastImage;

    QList<qint64> m_imageIds;
    QStringList  m_imagePaths;
    int          m_currentIndex;
    QTimer      *m_playTimer;
    int          m_playIntervalMs;
    int          m_playStep;
    bool         m_isPlaying;
    bool         m_manualStop = false;   // 抑制 saveLimitReached 弹窗
    bool         m_saveActive = false;  // 主线程自己的保存状态标志

    QString ensureSaveDir();
    void saveSnapImage(const QImage &img);
    void showImageAt(int index);
    void refreshSessionCombo();
    void loadImagesFromCurrentSelection();
    void refreshSliderAndTimer();
    void stopPlayback();
    void startPlayback();
};

#endif // MAINWINDOW_H