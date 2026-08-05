#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include "./c++_lib/inc/rfidlib.h"
#include "./c++_lib/inc/rfidlib_reader.h"
#include <vector>
#include "tag_hf.h"
#include "CAEDevice_HF.h"
#include "rfidbusinessworker.h"
#include "rfidlogdispatcher.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
using namespace std ;

// 读写器信息类：保存一个已加载的读写器驱动的基本信息
class CRdrInfo
{

public:
    CRdrInfo(){}
    virtual ~CRdrInfo() {}
public:
    QString m_name ;               // 驱动名称(显示用)
    QString m_productType;         // 产品类型ID
    QString m_Catalog;             // 设备分类(如 "Reader")
    DWORD m_CommTypeSupported;     // 支持的通信类型(USB/COM/TCP 的位掩码)
public:
    // 赋值运算符重载：复制另一个 CRdrInfo 的内容
    CRdrInfo &operator=(const CRdrInfo &Src)
    {
        m_name = Src.m_name ;
        m_productType =Src.m_productType ;
        m_Catalog = Src.m_Catalog ;
        m_CommTypeSupported = Src.m_CommTypeSupported ;
        return *this;
    }
};

// 主窗口类：整个程序的界面与控制中心
// 负责: 连接读写器、启动/停止盘点、显示标签、对标签进行读写操作
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    RFID_READER_HANDLE hr = NULL;   // 读写器句柄(连接成功后非空)
    MainWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *e);   // 重绘事件：窗口大小变化时重建表格
    ~MainWindow();
signals:
    // 盘点信号：发送给子线程的 CAEDevice_HF，触发盘点
    void signals_Inventory(void* hreader, QByteArray antennasSrc, int ant_cnt);
    void signals_ScanOnce(void* hreader, QByteArray antennasSrc, int ant_cnt);
    void signals_ScanStableBusinessTag(void* hreader, QByteArray antennasSrc, int ant_cnt);
    void signals_updateComplited();    // 表格更新完成信号：通知子线程可以继续下一轮盘点

private slots:
    void on_pushButton_clicked();      // "打开读写器"按钮槽(连接读写器)


    void on_pushButton_2_clicked();   // "关闭读写器"按钮槽(断开连接)

    void on_btn_inventory_start_clicked();    // "开始盘点"按钮槽
    void on_cmb_usb_opentype_currentIndexChanged(int index);  // USB打开方式下拉框变化槽
    void slot_inventory_end_loop(int iret);    // 盘点循环结束槽(子线程发来的)



    void on_btn_inventory_stop_clicked();     // "停止盘点"按钮槽

    void on_btn_inventory_clear_clicked();    // "清空列表"按钮槽

    void on_btn_scan_mode_start_clicked();
    void on_btn_scan_mode_clear_clicked();

    void on_btn_access_read_block_clicked();  // "读块"按钮槽



    void on_btn_connect_clicked();             // "连接标签"按钮槽

    void on_btn_Disconnect_clicked();          // "断开标签"按钮槽

    void on_btn_access_write_block_clicked();  // "写块"按钮槽

    void on_btn_access_open_eas_clicked();     // "打开EAS"按钮槽

    void on_btn_access_close_eas_clicked();    // "关闭EAS"按钮槽
    void set_info(QString str,bool success=true);  // 在界面底部显示状态信息

    void on_btn_access_get_eas_clicked();      // "获取EAS状态"按钮槽

    void on_btn_access_write_afi_clicked();   // "写AFI"按钮槽

    void on_btn_access_get_status_clicked();  // "获取标签状态/系统信息"按钮槽

private:
    enum class AccessTagSource {
        None,
        Inventory,
        ScanMode,
    };

    void on_business_read_clicked();
    void on_business_write_clicked();
    void on_business_cancel_clicked();
    void on_business_completed(const RfidOperationResult &result);
    void on_business_attempt_started(int attempt);
    void on_business_retry_scheduled(int failedAttempt, int delayMs);
    void on_business_device_stage(const QString &stage, const QString &message);
    void on_stable_scan_uid_changed(const QString &previousUids, const QString &currentUids, int elapsedMs);

private:
    bool loop=false;                // 盘点循环标志(主界面侧，未实际使用)
    QString RDType = QString("%1").arg(""); // 当前选中的读写器类型名称
    vector<CTag_HF> m_tags_hf;     // 盘点到的所有标签集合(累计，不重复)
    QThread *thread = nullptr;      // 子线程对象(运行盘点逻辑)
    CAEDevice_HF *device = nullptr; // 盘点设备对象(运行在子线程)
    RfidBusinessWorker *businessWorker = nullptr;
    RfidLogDispatcher *businessLogger = nullptr;
    bool businessOperationRunning = false;
    bool businessReadOperation = false;
    bool businessScanStable = false;
    AccessTagSource accessTagSource = AccessTagSource::None;
    bool loaded=false;              // 表格是否已初始化(防止重复创建)
    QSize formSize;                 // 上次记录的表格尺寸(用于判断是否需要重建)
    bool running = false;           // 是否正在盘点中
    bool scanRunning = false;       // 是否正在执行单次扫描
    RFID_TAG_HANDLE ht = nullptr;   // 当前已连接的标签句柄(用于读写操作)

    //BYTE air_type=AIR_ISO15693;

private:
    Ui::MainWindow *ui;
    void bind_antennas();           // 获取天线数量并填充天线列表框
    void create_inventory_view();   // 创建盘点结果表格(设置列标题、宽度等)
    void setWidgetEnable(QWidget *m_wid,bool enable);  // 设置控件可用状态并立即刷新
    void get_selected_antennas(BYTE ants[], int &len);  // 获取用户选中的天线编号
    void slot_inventory_data_hf(int tag_count,vector<CTag_HF> tags,int use_time,int loop_count);  // 接收盘点数据并更新表格
    void slot_scan_data_hf(int tag_count, vector<CTag_HF> tags, int use_time);
    void slot_scan_finished(int iret);
    void getConnectString(char *&connStr);  // 根据界面选择生成连接字符串
    void bind_access_tags();        // 把盘点到的标签填充到"标签操作"下拉框
    void HF_TagConnect();            // 连接选中的HF标签(建立标签句柄)
    void HF_TagDisconnect();        // 断开当前连接的HF标签
    bool SetAccessAntenna();         // 设置标签访问使用的天线
    bool ensureAccessReady(const QString &operation);
    bool parseHexInput(const QString &text, int expectedBytes, QByteArray &data, QString &error) const;
    void resetTagConnectionState();
    void create_scan_mode_view();
    void create_business_view();
    void update_business_tag_state();
    bool selected_business_tag(InventoryObservation *tag) const;
    void log_business(RfidLogLevel level, const QString &stage, int attempt,
                      const QString &message, const QMap<QString, QString> &fields = {});
    DWORD m_accessBlockSize = 4;     // ISO15693 常见块大小；读取系统信息后会更新
};
#endif // MAINWINDOW_H
