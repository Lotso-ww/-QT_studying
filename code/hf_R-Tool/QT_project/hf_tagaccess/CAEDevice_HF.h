#ifndef CAEDEVICE_HF_H
#define CAEDEVICE_HF_H

#include "./c++_lib/inc/rfidlib_reader.h"
#include "./c++_lib/inc/rfidlib.h"
#include "./c++_lib/inc/rfidlib_aip_iso14443A.h"
#include "./c++_lib/inc/rfidlib_aip_iso15693.h"
#include <QObject>
#include <QByteArray>
#include "tag_hf.h"
#include "rfidtagtypes.h"

using namespace std;


// 空中接口协议类型(用位掩码表示，每个协议占一位)
#define AIR_ISO14443A			1       // ISO14443A 协议
#define AIR_ISO15693			(1<<1)  // ISO15693 协议
#define AIR_ISO18000p3m3		(1<<2)  // ISO18000-3 Mode3 协议
#define AIR_ISO180006C			(1<<3)  // ISO18000-6C (UHF EPC Gen2) 协议
#define AIR_ISO14443B           (1<<4)  // ISO14443B 协议
#define AIR_ST_ISO14443B        (1<<5)  // ST 特殊的 ISO14443B 协议
#define AIR_SONY_FELICA         (1<<6)  // Sony Felica 协议
#define AIR_NFC_FORUM_TYPE1     (1<<7)  // NFC Forum Type1 标签

enum StableScanResult {
    StableScanSuccess = NO_ERR,
    StableScanNoTag = -10001,
    StableScanNotUnique = -10002,
    StableScanInconsistent = -10003,
    StableScanNotEnoughObservations = -10004,
};

// 高频(HF)读写器设备控制类
// 运行在子线程中，负责执行标签盘点的循环逻辑
// 通过信号和槽与主界面(MainWindow)通信，避免阻塞界面
class CAEDevice_HF : public QObject
{
    Q_OBJECT
public slots:
    // 盘点槽函数：在子线程中被调用，执行循环盘点
    // hreader: 读写器句柄, antennas: 选中的天线数组, ant_cnt: 天线个数
    void Inventory(void* hreader, QByteArray antennasSrc, int ant_cnt) ;
    // 单次扫描：只执行一轮盘点，用于箱单或单标签扫描。
    void ScanOnce(void* hreader, QByteArray antennasSrc, int ant_cnt);
    // 业务扫描：在固定窗口内重复盘点，确认唯一且稳定的 ISO15693 标签。
    void ScanStableBusinessTag(void* hreader, QByteArray antennasSrc, int ant_cnt);
    // 盘点完成后的更新槽函数(由主界面更新完表格后触发，用于唤醒子线程继续下一轮)
    void onUpdateCompleted();

signals:
    void workFinished();                                                          // 工作完成信号(预留)
    void sgnl_inventory_data_hf(int tag_count,vector<CTag_HF> tags,int use_time,int loop_count); // 一次盘点完成信号：标签数、标签列表、耗时、轮数
    void sgnl_inventory_end_loop(int iret);                                       // 整个盘点循环结束信号(带错误码)
    void sgnl_scan_data_hf(int tag_count, vector<CTag_HF> tags, int use_time);   // 单次扫描结果
    void sgnl_scan_finished(int iret);                                             // 单次扫描结束信号
    void updateConfirmed();    // 内部信号，用于唤醒等待主界面更新的局部事件循环

public:
    explicit CAEDevice_HF(QObject *parent = nullptr);
    ~CAEDevice_HF();

    err_t Start_Inventory();      // 启动盘点(预留)
    err_t End_Inventory();        // 停止盘点：把 loop 置为 false，退出盘点循环
    err_t func_Inventory();       // 执行一次盘点操作(核心盘点函数)
    // 添加一张新发现的 ISO15693 标签到列表
    void AddNewISO15693Tag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 dsfid,UINT8 *uid,USHORT rssi);
    // 添加一张新发现的 ISO14443A 标签到列表
    void AddNewISO14443ATag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 *uid,UINT8 uidlen);

    // ISO15693 business adapter methods. They keep SDK calls out of RfidTagService and MainWindow.
    static RfidDeviceResult setAccessAntenna(RFID_READER_HANDLE reader, quint32 antenna);
    static RfidDeviceResult connectIso15693(RFID_READER_HANDLE reader, quint32 tagType,
                                             const QString &uid, RFID_TAG_HANDLE *tagHandle);
    static RfidDeviceResult disconnectTag(RFID_READER_HANDLE reader, RFID_TAG_HANDLE *tagHandle);
    static RfidDeviceResult getSystemInfo(RFID_READER_HANDLE reader, RFID_TAG_HANDLE tagHandle,
                                          TagSystemInfo *systemInfo);
    static RfidDeviceResult readBlocks(RFID_READER_HANDLE reader, RFID_TAG_HANDLE tagHandle,
                                       int basicIndex, int blockCount, int blockSize, QByteArray *data);
    static RfidDeviceResult writeBlocks(RFID_READER_HANDLE reader, RFID_TAG_HANDLE tagHandle,
                                        int basicIndex, int blockCount, int blockSize,
                                        const QByteArray &data);

public:
    vector<CTag_HF>  m_tags_hf;            // 本次盘点到的标签集合
    bool loop;                             // 盘点循环控制标志：true=继续盘点, false=停止
    BYTE antennas[64]={0};                  // 参与盘点的天线编号数组
    BYTE ant_count=64;                      // 参与盘点的天线个数
    RFID_READER_HANDLE hr=NULL;             // 读写器句柄(由主界面传入)

private:
    bool waitSgl;          // 是否正在等待主界面更新完成的标志

};


#endif // CAEDEVICE_HF_H
