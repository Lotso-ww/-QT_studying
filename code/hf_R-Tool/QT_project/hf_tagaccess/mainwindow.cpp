#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QListWidgetItem"
#include "qmessagebox.h"
#include "gfunction.h"
#include "./c++_lib/inc/rfidlib.h"

#include <QThread>

using namespace Qt;
QString TCHARToQString(const TCHAR* str) {
#ifdef UNICODE
    return QString::fromWCharArray(str);
#else
    return QString::fromLocal8Bit(str);
#endif
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iret = -1;
    int drvCnt = 0;


    QString connStr = "\\Drivers";
    const wchar_t* connStrPtr = reinterpret_cast<const wchar_t*>(connStr.utf16());
    iret = RDR_LoadReaderDrivers(connStrPtr);
    drvCnt = RDR_GetLoadedReaderDriverCount();

    DWORD COMCount ;
    COMCount = COMPort_Enum() ;
    for(int i =0;i<COMCount ;i++)
    {
        TCHAR comname[64] ;
        memset(comname,0,sizeof(comname)) ;
        iret =  COMPort_GetEnumItem(i,comname,sizeof(comname)/ sizeof(TCHAR)) ;
        if(iret == 0)
        {

            ui->cmb_com_name->addItem(QString::fromWCharArray(comname));

        }
    }


    for(int i=0;i<drvCnt ;i++)
    {
        DWORD nSize ;
        CRdrInfo rdrinfo ;
        TCHAR tmp[64] ;
        memset(tmp,0,sizeof(tmp)) ;
        nSize = sizeof(tmp) / sizeof(TCHAR);
        QString catalogStr = LOADED_RDRDVR_OPT_CATALOG;
        QString readerStr = RDRDVR_TYPE_READER;
        QString drvNameStr = LOADED_RDRDVR_OPT_NAME;
        QString drvIDStr = LOADED_RDRDVR_OPT_ID;
        QString commTypeStr = LOADED_RDRDVR_OPT_COMMTYPESUPPORTED;
        std::wstring wstr = catalogStr.toStdWString();
        const wchar_t* catalog = reinterpret_cast<const wchar_t*>(catalogStr.utf16());
        LPCTSTR pdrvName = reinterpret_cast<LPCTSTR>(drvNameStr.utf16());
        LPCTSTR pdrvID =reinterpret_cast<LPCTSTR>(drvIDStr.utf16());
        LPCTSTR pcommType =reinterpret_cast<LPCTSTR>(commTypeStr.utf16());


        RDR_GetLoadedReaderDriverOpt(i,catalog ,tmp, &nSize) ;
        rdrinfo.m_Catalog= TCHARToQString(tmp) ;
        if(rdrinfo.m_Catalog ==(RDRDVR_TYPE_READER)){
            memset(tmp,0,sizeof(tmp));
            nSize = sizeof(tmp) / sizeof(TCHAR);
            RDR_GetLoadedReaderDriverOpt(i,pdrvName , tmp,&nSize) ;
            rdrinfo.m_name = TCHARToQString(tmp) ;

            memset(tmp,0,sizeof(tmp)) ;
            nSize = sizeof(tmp) / sizeof(TCHAR);
            RDR_GetLoadedReaderDriverOpt(i,pdrvID ,tmp,&nSize) ;
            rdrinfo.m_productType =  TCHARToQString(tmp) ;
            memset(tmp,0,sizeof(tmp)) ;
            nSize = sizeof(tmp) / sizeof(TCHAR);
            RDR_GetLoadedReaderDriverOpt(i,pcommType ,tmp,&nSize) ;
            rdrinfo.m_CommTypeSupported =_wtoi(tmp) ;

            ui->cmb_rdtype->addItem(rdrinfo.m_name);
        }
    }

    if(ui->cmb_com_name->count() > 0)
    {
        ui->cmb_com_name->setCurrentIndex(0) ;
    }


}

//获取天线数量，填充天线列表框
void MainWindow::bind_antennas()
{
    ui->lw_ants->clear();
    DWORD ant_cnt=RDR_GetAntennaInterfaceCount(hr);
    QString str;
    for (int i = 1; i <= ant_cnt; ++i) {
        str.sprintf("#%d",i);
        QListWidgetItem* chk=new QListWidgetItem();
        chk->setData(Qt::DisplayRole,"text");
        chk->setData(Qt::CheckStateRole, Checked);
        chk->setData(Qt::UserRole, i);//set user data(start from 1)
        chk->setText(str);
        if(i==1)
            chk->setCheckState(Qt::CheckState::Checked);
        else
            chk->setCheckState(Qt::CheckState::Unchecked);
        ui->lw_ants->addItem(chk);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString rdTypeStr = ui->cmb_rdtype->currentText();
    QString comStr = ui->cmb_com_name->currentText();
    QString baudStr = ui->cmb_com_baud->currentText();
    QString frameStr = ui->cmb_com_frame->currentText();
    QString commTypeStr = "COM";

    QString connStr = QString("%1=%2;%3=%4;%5=%6;%7=%8;%9=%10;%11=%12")
    .arg(CONNSTR_NAME_RDTYPE).arg(rdTypeStr).arg(CONNSTR_NAME_COMMTYPE).arg(commTypeStr)
    .arg(CONNSTR_NAME_COMNAME)
    .arg(comStr)
    .arg(CONNSTR_NAME_COMBARUD)
    .arg(baudStr)
    .arg(CONNSTR_NAME_COMFRAME)
    .arg(frameStr)
    .arg(CONNSTR_NAME_BUSADDR)
    .arg("255");

    char connChar[255];
    char *pConn=connChar;
    getConnectString(pConn);
    wchar_t* conn = ANSIToUnicode(pConn);
     LPCTSTR pconnStr =reinterpret_cast<LPCTSTR>(connStr.utf16());
    int iret = RDR_Open(conn,&hr) ;
    free(conn);
    if(iret == 0 ){
        ui->pushButton_2->setEnabled(true);
        ui->pushButton->setEnabled(false);
        bind_antennas();
        thread = new QThread();
        device = new CAEDevice_HF(thread);
        device->moveToThread(thread);
        connect(this,&MainWindow::signals_Inventory,device,&CAEDevice_HF::Inventory);
        connect(device,&CAEDevice_HF::sgnl_inventory_data_hf,this,&MainWindow::slot_inventory_data_hf);
        connect(device,&CAEDevice_HF::sgnl_inventory_end_loop,this,&MainWindow::slot_inventory_end_loop);
        connect(this,&MainWindow::signals_updateComplited,device,&CAEDevice_HF::onUpdateCompleted);
        thread->start();
    }

}



void MainWindow::on_pushButton_2_clicked()
{

    if(running)
    {
        return;
    }
    int iret = RDR_Close(hr);
    if(iret == NO_ERR){
        hr = nullptr;
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(false);
        disconnect(this,&MainWindow::signals_Inventory,device,&CAEDevice_HF::Inventory);
        disconnect(device,&CAEDevice_HF::sgnl_inventory_data_hf,this,&MainWindow::slot_inventory_data_hf);
        disconnect(device,&CAEDevice_HF::sgnl_inventory_end_loop,this,&MainWindow::slot_inventory_end_loop);
        disconnect(this,&MainWindow::signals_updateComplited,device,&CAEDevice_HF::onUpdateCompleted);
    }

}

void MainWindow::setWidgetEnable(QWidget *m_wid,bool enable)
{
    m_wid->setEnabled(enable);
    qApp->processEvents();
    m_wid->repaint();
}

void MainWindow::get_selected_antennas(BYTE ants[],int &len)
{
    DWORD ant_cnt=ui->lw_ants->count();
    if(len < ant_cnt)
    {
        len=0;
        return;
    }
    len=0;
    for (int i = 0; i < ant_cnt; ++i) {
        if(ui->lw_ants->item(i)->checkState()==CheckState::Checked)
        {
            ants[len]=ui->lw_ants->item(i)->data(UserRole).toInt();//get userrole data
            len++;
        }
    }
}

void MainWindow::getConnectString(char *&connStr)
{

    RDType=ui->cmb_rdtype->currentText();
    QString CommType=ui->cmb_commtype->currentText();
    int CommIndex=ui->cmb_commtype->currentIndex();
    if(CommIndex==0)//COM
    {
        QString COMName=ui->cmb_com_name->currentText();
        int BaudRate=ui->cmb_com_baud->currentData().toInt();
        QString Frame=ui->cmb_com_frame->currentText();

        sprintf(connStr,"RDType=%s;CommType=%s;COMName=%s;BaudRate=%d;Frame=%s;BusAddr=255",
                RDType.toStdString().c_str(),
                CommType.toStdString().c_str(),
                COMName.toStdString().c_str(),
                BaudRate,
                Frame.toStdString().c_str());
    }
    else if(CommIndex==1)//USB
    {
        int AddrMode=ui->cmb_usb_opentype->currentIndex();
        QString SerNum="\0";
        if(AddrMode > 0)
        {
            SerNum=ui->cmb_usb_sn_path->currentText();
            if(SerNum.isEmpty()|| SerNum.isNull())
            {
                *connStr=NULL;
                QMessageBox::warning(this,"Connect String","No USB Device Selected!",QMessageBox::Ok);
                return;
            }
        }
        sprintf(connStr,"RDType=%s;CommType=%s;AddrMode=%d;SerNum=%s",
                RDType.toStdString().c_str(),
                CommType.toStdString().c_str(),
                AddrMode,
                SerNum.toStdString().c_str());
    }
    else if(CommIndex==2)//TCP
    {
        QString RemoteIP=ui->txt_tcp_ip->text().trimmed();
        int RemotePort=ui->cmb_tcp_port->currentData().toInt();
        QString LocalIP=ui->cmb_local_tcp_ip->currentText().trimmed();
        sprintf(connStr,"RDType=%s;CommType=%s;RemoteIP=%s;RemotePort=%d;LocalIP=%s",
                RDType.toStdString().c_str(),
                CommType.toStdString().c_str(),
                RemoteIP.toStdString().c_str(),
                RemotePort,LocalIP.toStdString().c_str());
    }
    printf("connStr :%s\n",connStr);
}


void MainWindow::create_inventory_view()
{
    ui->tbw_inventory_tags->clear();
    QStringList strList;
    BYTE* szRate;
    int columns=6;

    strList<<tr("AntID")<<tr("Air Protocol")<<tr("Tag Type")<<tr("UID")<<tr("RSSI")<<tr("Read Count");
    columns=strList.length();
    szRate=new BYTE[columns]{1,1,1,2,1,1};

    ui->tbw_inventory_tags->setColumnCount(columns);
    ui->tbw_inventory_tags->setHorizontalHeaderLabels(strList);
    int wid=ui->tbw_inventory_tags->size().width()-38;
    ui->tbw_inventory_tags->setRowCount(0);
    int szSum=0;
    for(int i=0;i<columns;i++)
    {
        szSum +=szRate[i];
    }
    for(int i=0;i<columns;i++)
    {
        ui->tbw_inventory_tags->setColumnWidth(i,szRate[i]*wid/szSum);
    }

    delete[] szRate;

    ui->tbw_inventory_tags->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbw_inventory_tags->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbw_inventory_tags->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QSize size = ui->tbw_inventory_tags->size();

    if(!loaded || size!=formSize ){
        loaded =true;
        formSize = ui->tbw_inventory_tags->size();
        create_inventory_view();
    }

}

void MainWindow::bind_access_tags()
{
    ui->cmb_access_tags->clear();
    for(int i=0;i<m_tags_hf.size();i++)
    {
        ui->cmb_access_tags->addItem(m_tags_hf[i].m_uid);
    }

    for(int i=0;i<32;i++)
    {
        ui->cmb_access_block_start->addItem(QString("%1").arg(i),i);
        ui->cmb_access_block_count->addItem(QString("%1").arg(i+1),i+1);
    }


    if(ui->cmb_access_tags->count()>0){
        ui->cmb_access_tags->setCurrentIndex(0);
    }
    ui->cmb_access_block_count->setCurrentIndex(0);
    ui->cmb_access_block_start->setCurrentIndex(0);

}
void MainWindow::slot_inventory_data_hf(int tag_count,vector<CTag_HF> tags,int use_time,int loop_count)
{
    if(tag_count<0)//inventory terminaled
    {
        return;
    }
    bool exist = false;
    int sum_count = m_tags_hf.size();
    for (int i = 0; i < tag_count; ++i) {
        exist = false;
        for (int j=0; j < sum_count;++j) {
            if(tags.at(i).equal(m_tags_hf.at(j)))
            {
                m_tags_hf.at(j).m_counter ++;
                exist = true;
                break;
            }
        }
        if(!exist)
        {
            m_tags_hf.push_back(tags.at(i));
        }
    }


    sum_count = m_tags_hf.size();

    ui->tbw_inventory_tags->setRowCount(sum_count);

    ui->tbw_inventory_tags->setColumnCount(6);
    //strList<<tr("AntID")<<tr("Air Protocol")<<tr("Tag Type")<<tr("UID")<<tr("RSSI")<<tr("Read Count");
    for(int i=0;i < sum_count;i++)
    {
        ui->tbw_inventory_tags->setItem(i,0,new QTableWidgetItem(QString::number(m_tags_hf[i].m_antNo)));
        ui->tbw_inventory_tags->setItem(i,1,new QTableWidgetItem(QString::number(m_tags_hf[i].m_AIP)));
        ui->tbw_inventory_tags->setItem(i,2,new QTableWidgetItem(QString::number(m_tags_hf[i].m_type)));
        ui->tbw_inventory_tags->setItem(i,3,new QTableWidgetItem(QString(m_tags_hf[i].m_uid)));
        ui->tbw_inventory_tags->setItem(i,4,new QTableWidgetItem(QString::number(m_tags_hf[i].m_rssi)));
        ui->tbw_inventory_tags->setItem(i,5,new QTableWidgetItem(QString::number(m_tags_hf[i].m_counter)));

        ui->tbw_inventory_tags->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        //ui->tbw_inventory_tags;
    }


    ui->tbw_inventory_tags->show();
    ui->tbw_inventory_tags->setVisible(true);
    ui->tbw_inventory_tags->update();


    ui->lbl_inventory->setText(QString("Tags: %1  \nTime: %2 ms\nLoop: %3").arg(sum_count).arg(use_time).arg(loop_count));

    QMetaObject::invokeMethod(this, [this]() {
            emit signals_updateComplited();
        }, Qt::QueuedConnection);



}

void MainWindow::slot_inventory_end_loop(int iret)
{
    if(iret!= ERR_OK)
    {
        QString errMsg = QString("err_t = %1 !").arg(iret);
        QMessageBox::information(this,"Inventory",errMsg,QMessageBox::Ok);

    }
    running =false;
    setWidgetEnable(ui->btn_inventory_start,true);
    bind_access_tags();
}
void MainWindow::on_btn_inventory_start_clicked()
{

    setWidgetEnable(ui->btn_inventory_start,false);

    int ant_count=ui->lw_ants->count();
    BYTE ants[32];
    memset(ants,0,sizeof(ants));
    get_selected_antennas(ants,ant_count);
    if(ant_count<=0)
    {
        setWidgetEnable(ui->btn_inventory_start,true);
        QMessageBox::warning(this,"Inventory","No Antenna Selected!",QMessageBox::Ok);
        return;
    }
    if(hr == nullptr)
    {
        setWidgetEnable(ui->btn_inventory_start,true);
        QMessageBox::warning(this,"Inventory","Please open reader first!",QMessageBox::Ok);
        return;
    }

    ui->tbw_inventory_tags->clearContents();
    ui->tbw_inventory_tags->setRowCount(0);


    running =true;
    emit MainWindow::signals_Inventory(hr,ants,ant_count);

}


void MainWindow::on_cmb_usb_opentype_currentIndexChanged(int index)
{
    if(index==0)//no addr
    {
        ui->cmb_usb_sn_path->clear();
    }
    else if(index==1)//SN
    {
        ui->cmb_usb_sn_path->clear();
        QString rdTypeQStr = ui->cmb_rdtype->currentText();
         const wchar_t* rdTypeWStr = reinterpret_cast<const wchar_t *>(rdTypeQStr.utf16());
        int cnt=HID_Enum(rdTypeWStr);
        int iret=0;
        DWORD len=13;
        if(cnt>0)
        {
            LPTSTR str=new TCHAR[len]{'\0'};
            for (int i = 0; i < cnt; ++i) {
                iret=HID_GetEnumItem(i,HID_ENUM_INF_TYPE_SERIALNUM,str,&len);
                if(iret==NO_ERR)
                {
                    QString strs = QString("%1").arg(str);
                    ui->cmb_usb_sn_path->addItem(strs);
                }
            }
            delete[] str;
        }
    }

}

void MainWindow::on_btn_inventory_stop_clicked()
{
    if(device){
        device->End_Inventory();
    }
}

void MainWindow::on_btn_inventory_clear_clicked()
{
    ui->tbw_inventory_tags->clearContents();
    ui->tbw_inventory_tags->setRowCount(0);
}

void MainWindow::on_btn_access_read_block_clicked()
{
    bool read_secsta=false;


    DWORD byts_read_out=0;

    DWORD addr,num;
    addr=ui->cmb_access_block_start->currentIndex();
    num=ui->cmb_access_block_count->currentIndex()+1;
    DWORD nSize=(read_secsta?num*5:num*4);
    DWORD numOfBlks_Readed=0;
    BYTE buffer[128*5] ;
    memset(buffer,0,sizeof(buffer));
    int iret=ISO15693_ReadMultiBlocks(hr,ht,read_secsta,addr,num,&numOfBlks_Readed,buffer,nSize,&byts_read_out);
    char* strs = new char[byts_read_out*2+1];
    memset(strs,0,byts_read_out*2+1);
    BytesToHexStr(buffer,byts_read_out,strs);//字节数组转十六进制字符串
    ui->txt_access_block_data->setText(QString(strs));//char*转QString
    delete[] strs;

}

void MainWindow::HF_TagConnect()
{

    SetAccessAntenna();
    int index = ui->cmb_access_tags->currentIndex();
    CHAR addr_mode = 1;

    int ulen=m_tags_hf[index].m_uid.length()>2?m_tags_hf[index].m_uid.length()/2:0;
    BYTE byts[255];
    memset(byts,0,ulen);
    int blen=0;
    int iret = 0;

    BYTE* byt = new BYTE[ulen];
    memset(byt,0,ulen);
     HexStrToBytes(m_tags_hf[index].m_uid.toUtf8().data(),byt,blen);

     ht=NULL;
    iret = ISO15693_Connect(hr,m_tags_hf[index].m_type,addr_mode,byt,&ht);
    if(iret ==ERR_OK)
    {
        ui->btn_connect->setEnabled(false);
    }
}
void MainWindow::HF_TagDisconnect()
{

    if(ht == NULL) return;
    int iret=RDR_TagDisconnect(hr,ht);
    if(iret!= NO_ERR)
        return;
    ui->btn_connect->setEnabled(true);
}
void MainWindow::SetAccessAntenna()
{
    int ant_count=ui->lw_ants->count();
    BYTE ants[32];
    memset(ants,0,sizeof(ants));
    get_selected_antennas(ants,ant_count);
    int iret;
    if(ant_count>0){
        iret = RDR_SetAcessAntenna(hr,ants[0]);
    }
    else
    {
        QMessageBox::warning(this,"Tag Connect","Please select antenna!",QMessageBox::Ok);
    }

}

void MainWindow::on_btn_connect_clicked()
{
    HF_TagConnect();
}

void MainWindow::on_btn_Disconnect_clicked()
{
    HF_TagDisconnect();
}

void MainWindow::on_btn_access_write_block_clicked()
{
    int block_addr = ui->cmb_access_block_start->currentIndex();
    int block_num = ui->cmb_access_block_count->currentIndex()+1;
    QString qstrs=ui->txt_access_block_data->text().trimmed();

    int blen=qstrs.size()/2;
    BYTE *data =new BYTE[blen];

    HexStrToBytes(qstrs.toUtf8().data(),data,blen);
    int iret = ISO15693_WriteMultipleBlocks(hr,ht,block_addr,block_num,data,blen);
    delete[] data;

    if(iret!=NO_ERR)
        set_info(QString("Write Block Failed! err=%1").arg(iret),false);
    else
        set_info(QString("Write Block Success"));

}

void MainWindow::on_btn_access_open_eas_clicked()
{
    int iret=NXPICODESLI_EableEAS(hr,ht);
    if(iret!=NO_ERR)
        set_info(QString("Open EAS Failed! err=%1").arg(iret),false);
    else
        set_info(QString("Open EAS Success"));
}

void MainWindow::on_btn_access_close_eas_clicked()
{
    int iret=NXPICODESLI_DisableEAS(hr,ht);
    if(iret!=NO_ERR)
        set_info(QString("Close EAS Failed! err=%1").arg(iret),false);
    else
        set_info(QString("Close EAS Success"));
}

void MainWindow::set_info(QString str,bool success)
{

    ui->lbl_info->setText(str);
}

void MainWindow::on_btn_access_get_eas_clicked()
{
    BYTE eas_sta;
    int iret=NXPICODESLI_EASCheck(hr,ht,&eas_sta);
    if(iret!=NO_ERR)
    {
        set_info(QString("Get EAS Failed! err=%1").arg(iret),false);
    }
    else
    {
        set_info(QString("Get EAS Success!"));
        QMessageBox::information(this,"Get EAS",QString("Eas is %1").arg(eas_sta==1?"Opened":"Closed"),QMessageBox::Ok);
    }

}

void MainWindow::on_btn_access_write_afi_clicked()
{
    BYTE afi=(BYTE)ui->txt_access_afi->text().trimmed().toInt(NULL,16);
    int iret = ISO15693_WriteAFI(hr,ht,(BYTE)afi);
    if(iret!=NO_ERR)
    {
        set_info(QString("Write AFI Failed! err=%1").arg(iret),false);
    }
    else{
        set_info(QString("Write AFI Success!"));
    }

}

void MainWindow::on_btn_access_get_status_clicked()
{
    BYTE dsfid;
    BYTE afi;
    DWORD block_size;
    DWORD block_num;
    BYTE ic_ref;
    int iret = 0;

    int index = ui->cmb_access_tags->currentIndex();

    int ulen=m_tags_hf[index].m_uid.length()>2?m_tags_hf[index].m_uid.length()/2:0;
    BYTE *uid =new BYTE[ulen];

    HexStrToBytes(m_tags_hf[index].m_uid.toUtf8().data(),uid,ulen);



    iret=ISO15693_GetSystemInfo(hr,ht,uid,&dsfid,&afi,&block_size,&block_num,&ic_ref);
    if(iret == NO_ERR)
    {
        ui->txt_access_afi->setText(QString("%1").arg((int)afi,2,16,QChar('0')).toUpper());
        set_info(QString("Get Status Success"));
    }


}
