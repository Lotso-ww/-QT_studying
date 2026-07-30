#ifndef CAEDEVICE_HF_H
#define CAEDEVICE_HF_H

#include "./c++_lib/inc/rfidlib_reader.h"
#include "./c++_lib/inc/rfidlib.h"
#include "./c++_lib/inc/rfidlib_aip_iso14443A.h"
#include "./c++_lib/inc/rfidlib_aip_iso15693.h"
#include <QObject>
#include "tag_hf.h"
#include <QEventLoop>

using namespace std;


//空中协议类型
#define AIR_ISO14443A			1   //空中协议参数类型
#define AIR_ISO15693			(1<<1)
#define AIR_ISO18000p3m3		(1<<2)
#define AIR_ISO180006C			(1<<3)
#define AIR_ISO14443B           (1<<4)
#define AIR_ST_ISO14443B        (1<<5)
#define AIR_SONY_FELICA         (1<<6)
#define AIR_NFC_FORUM_TYPE1     (1<<7)


class CAEDevice_HF : public QObject
{
    Q_OBJECT
public slots:
    void Inventory(void* hreader,BYTE antennas[], BYTE ant_cnt) ;
    void onUpdateCompleted();
signals:
    void workFinished();
    void sgnl_inventory_data_hf(int tag_count,vector<CTag_HF> tags,int use_time,int loop_count);
    void sgnl_inventory_end_loop(int iret);
    void updateConfirmed();
public:
    explicit CAEDevice_HF(QObject *parent = nullptr);
    ~CAEDevice_HF();

    err_t Start_Inventory();
    err_t End_Inventory();
    err_t func_Inventory();
    void AddNewISO15693Tag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 dsfid,UINT8 *uid,USHORT rssi);
    void AddNewISO14443ATag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 *uid,UINT8 uidlen);

public:
    vector<CTag_HF>  m_tags_hf;
    bool loop;
    BYTE antennas[64]={0};
    BYTE ant_count=64;
    RFID_READER_HANDLE hr=NULL;

private:
    QEventLoop loopEvt;
    bool waitSgl;

};


#endif // CAEDEVICE_HF_H
