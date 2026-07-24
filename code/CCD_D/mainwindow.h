#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>
#include "camerathread.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

    // void onPlaybackTimeout(); // 定时器触发时，就负责换下一张图
    // void on_btn_play_clicked(); // Tab 2 里的“播放/暂停”按钮

    // 新增一个自定义槽函数，专门用来接收子线程发来的 QImage
    void displayLiveImage(const QImage &img);

    // void on_btn_save_clicked();

private:
    Ui::MainWindow *ui;
    // 几个比较关键的成员变量
    std::shared_ptr<peak::core::Device> m_device;               // 相机设备对象
    std::shared_ptr<peak::core::DataStream> m_dataStream;       // 图像数据流通道
    std::shared_ptr<peak::core::NodeMap> m_nodeMapRemoteDevice; // 相机的参数节点图(控制曝光等)

    CameraThread* m_cameraThread;
};
#endif // MAINWINDOW_H
