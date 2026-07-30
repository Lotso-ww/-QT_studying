#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./c++_lib/inc/rfidlib.h"
#include "./c++_lib/inc/rfidlib_reader.h"
#include <vector>
#include "tag_hf.h"
#include "CAEDevice_HF.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
using namespace std ;

class CRdrInfo
{

public:
    CRdrInfo(){}
    virtual ~CRdrInfo() {}
public:
    QString m_name ;
    QString m_productType;
    QString m_Catalog;
    DWORD m_CommTypeSupported;
public:
    CRdrInfo &operator=(const CRdrInfo &Src)
    {
        m_name = Src.m_name ;
        m_productType =Src.m_productType ;
        m_Catalog = Src.m_Catalog ;
        m_CommTypeSupported = Src.m_CommTypeSupported ;
        return *this;
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    RFID_READER_HANDLE hr = NULL;
    MainWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *e);
    ~MainWindow();
signals:
    void signals_Inventory(void* hreader,BYTE antennasSrc[], BYTE ant_cnt);
    void signals_updateComplited();

private slots:
    void on_pushButton_clicked();
    


    void on_pushButton_2_clicked();

    void on_btn_inventory_start_clicked();
    void on_cmb_usb_opentype_currentIndexChanged(int index);
    void slot_inventory_end_loop(int iret);



    void on_btn_inventory_stop_clicked();

    void on_btn_inventory_clear_clicked();

    void on_btn_access_read_block_clicked();


    void on_btn_connect_clicked();

    void on_btn_Disconnect_clicked();

    void on_btn_access_write_block_clicked();

    void on_btn_access_open_eas_clicked();

    void on_btn_access_close_eas_clicked();
    void set_info(QString str,bool success=true);

    void on_btn_access_get_eas_clicked();

    void on_btn_access_write_afi_clicked();

    void on_btn_access_get_status_clicked();

private:
    bool loop=false;
    QString RDType = QString("%1").arg("");
    vector<CTag_HF> m_tags_hf;
    QThread *thread;
    CAEDevice_HF *device;
    bool loaded=false;
    QSize formSize;
    bool running;
    RFID_TAG_HANDLE ht;

    //BYTE air_type=AIR_ISO15693;

private:
    Ui::MainWindow *ui;
    void bind_antennas();
    void create_inventory_view();
    void setWidgetEnable(QWidget *m_wid,bool enable);
    void get_selected_antennas(BYTE ants[], int &len);
    void slot_inventory_data_hf(int tag_count,vector<CTag_HF> tags,int use_time,int loop_count);
    void getConnectString(char *&connStr);
    void bind_access_tags();
    void HF_TagConnect();
    void HF_TagDisconnect();
    void SetAccessAntenna();
};
#endif // MAINWINDOW_H
