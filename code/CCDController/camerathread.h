//#ifndef CAMERATHREAD_H
//#define CAMERATHREAD_H

//#include <QThread>
//#include <QImage>
//#include <QString>
//#include <memory>
//#include <mutex>
//#include <peak/peak.hpp>

//class CameraThread : public
// QThread
//{
//    Q_OBJECT
//public:
//    explicit CameraThread(QObject *parent = nullptr);
//    ~CameraThread();

//    // 控制线程停止
//    void stop();

//signals:
//    // 告诉 UI 界面有新图片来了
//    void imageReady(const QImage &image);
//    //告诉 UI 界面相机出错了
//    void errorOccurred(const QString &errorMsg);

//protected:
//    // 线程的主循环，获取图像的逻辑
//    void run() override;

//private:
//    bool m_isRunning;

//};

//#endif
