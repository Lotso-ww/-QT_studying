#include "CAEDevice_HF.h"
#include "gfunction.h"
#include <QElapsedTimer>


CAEDevice_HF::CAEDevice_HF(QObject *parent) : QObject(parent)
{
    connect(this, &CAEDevice_HF::updateConfirmed, &loopEvt, &QEventLoop::quit);
}

CAEDevice_HF::~CAEDevice_HF()
{

}


err_t CAEDevice_HF::Start_Inventory()
{
    return 0;
}

err_t CAEDevice_HF::End_Inventory()
{
    loop=false;
    return ERR_OK;
}


void CAEDevice_HF::Inventory(void* hreader,BYTE antennasSrc[], BYTE ant_cnt)
{
    err_t iret;
    DWORD tagCount ;

    memset(antennas,0,ant_count);
    memcpy(antennas,antennasSrc,ant_cnt);
    ant_count=ant_cnt;
    hr = hreader;

    int loop_count=0;
    m_tags_hf.clear();//标签集合
    loop = true;
    QElapsedTimer timer;
    waitSgl =false;


    while(loop)//wait any key to exit
    {
        if(waitSgl){

            loopEvt.exec();
            waitSgl = false;
        }
        timer.start();
        m_tags_hf.clear();

        iret = func_Inventory();
        qint64 sec = timer.elapsed();
        if(iret == NO_ERR)
        {
            loop_count++;
            tagCount =m_tags_hf.size();
            emit sgnl_inventory_data_hf(tagCount,m_tags_hf,sec,loop_count);
            waitSgl = true;
        }
        else
        {
            break;
        }

    }

    if(hr != NULL)
        RDR_ResetCommuImmeTimeout(hr);//reset timout status
    loop=false;
    //is_inventory=false;
    emit sgnl_inventory_end_loop(iret);

}


void CAEDevice_HF::onUpdateCompleted() {
    // 只是简单地转发一个内部信号，用于唤醒事件循环
    emit updateConfirmed();
    waitSgl = false;
}

err_t CAEDevice_HF::func_Inventory()
{
    err_t iret;
    BYTE newAI= AI_TYPE_NEW;

    //RDR_DisableAsyncTagReportOutput(hRdr);// disable async tag report output

    RFID_DN_HANDLE dnInvenParamList= RFID_HANDLE_NULL;
    dnInvenParamList = RDR_CreateInvenParamSpecList();
    if(dnInvenParamList)
    {
        ISO15693_CreateInvenParam(dnInvenParamList,0,false,0,0x00);
        ISO14443A_CreateInvenParam(dnInvenParamList,0);
    }
    else
        return -ERR_MEM;

    iret = RDR_TagInventory(hr,newAI,ant_count,antennas,dnInvenParamList);
    if(iret == NO_ERR || iret == -ERR_STOPTRRIGOCUR)
    {
        RFID_DN_HANDLE dnhReport = RDR_GetTagDataReport(hr,RFID_SEEK_FIRST);
        while(dnhReport != RFID_HANDLE_NULL)
        {
            DWORD AIPtype,TagType,AntId,readCount;
            BYTE dsfid;
            WORD rssi;
            BYTE uid[8];
            BYTE uidlen;
            int rtn= ISO15693_ParseTagDataReportEx(dnhReport,&AIPtype,&TagType,&AntId,&dsfid,&rssi,&readCount,uid);
            if(rtn == NO_ERR)
            {
                AddNewISO15693Tag(AIPtype,TagType,AntId,dsfid,uid,rssi);
            }
            rtn = ISO14443A_ParseTagDataReport(dnhReport,&AIPtype,&TagType,&AntId,uid,&uidlen);
            if(rtn  == NO_ERR)
            {
                AddNewISO14443ATag(AIPtype,TagType,AntId ,uid, uidlen);
            }
            dnhReport = RDR_GetTagDataReport(hr,RFID_SEEK_NEXT);
        }
        if (iret == -ERR_STOPTRRIGOCUR) // stop trigger occur,need to inventory left tags
            iret = NO_ERR;
    }
    if(dnInvenParamList)
    {
        DNODE_Destroy(dnInvenParamList);
    }
    return iret;
}


void CAEDevice_HF::AddNewISO15693Tag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 dsfid,UINT8 *uid,USHORT rssi)
{
    QString tagtype;
    if(picc_tid == RFID_ISO15693_PICC_ICODE_SLI_ID )
    {
        tagtype =QString("NXP ICODE SLI");
    }
    else if(picc_tid ==RFID_ISO15693_PICC_TI_HFI_PLUS_ID)
    {
        tagtype = QString("Ti HF-I Plus");
    }
    else if(picc_tid ==RFID_ISO15693_PICC_ST_M24LR64_ID )
    {
        tagtype = QString("ST M24LR64");
    }
    else if(picc_tid == RFID_ISO15693_PICC_ST_M24LR16E_ID)
    {
        tagtype = QString("ST M24LR16E");
    }
    else if(picc_tid == RFID_ISO15693_PICC_ST25TV02K_ID) {
        tagtype = QString("ST25TV02K");
    }
    else if(picc_tid == RFID_ISO15693_PICC_FUJ_MB89R118C_ID)
    {
        tagtype = QString("Fujitsu MB89R118C");
    }
    else if(picc_tid == RFID_ISO15693_PICC_ICODE_SLIX_ID)
    {
        tagtype = QString("NXP ICODE SLIX");
    }
    else if(picc_tid == RFID_ISO15693_PICC_TIHFI_STANDARD_ID)
    {
        tagtype = QString("Ti HF-I Standard");
    }
    else if(picc_tid == RFID_ISO15693_PICC_TIHFI_PRO_ID)
    {
        tagtype = QString("Ti HF-I Pro");
    }
    else if(picc_tid == RFID_ISO15693_PICC_ICODE_SLIX2_ID){
        tagtype = QString("NXP ICODE SLIX2");
    }
    else
    {
        tagtype = QString("Unknown Tag");
    }
    CHAR suid[32];
    memset(suid,0,sizeof(suid));
    BytesToHexStr(uid,8,suid);
    CHAR sdsfid[8];
    memset(sdsfid,0,sizeof(sdsfid));
    BytesToHexStr(&dsfid,1,sdsfid);
    UINT32 i;
    CTag_HF* pTag;
    for( i=0;i<m_tags_hf.size();i++)
    {
        pTag=(CTag_HF*)&m_tags_hf.at(i);
        if(pTag->m_uid == suid && pTag->m_antNo == ant_id)
        {
            break;
        }
    }
    if(i>=m_tags_hf.size())
    {
        CTag_HF newtag;
        newtag.m_counter = 1;
        newtag.m_uid =suid;
        newtag.m_type = picc_tid;
        newtag.m_antNo = ant_id;
        newtag.m_AIP =apl_tid ;
        newtag.m_rssi=rssi;
        m_tags_hf.push_back(newtag);
    }
    else
    {
        pTag->m_counter++;
        pTag->m_rssi=rssi;
        if(pTag->m_counter>=500000)
        {
            pTag->m_counter = 1;
        }
    }
}


void CAEDevice_HF::AddNewISO14443ATag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 *uid,UINT8 uidlen)
{
    QString tagtype;
    switch(picc_tid)
    {
    case RFID_ISO14443A_PICC_NXP_ULTRALIGHT_ID:
        tagtype = QString("NXP Mifare Ultralight");
        break;
    case RFID_ISO14443A_PICC_NXP_MIFARE_S50_ID:
        tagtype = QString("NXP Mifare S50");
        break;
    case RFID_ISO14443A_PICC_NXP_MIFARE_S70_ID:
        tagtype = QString("NXP Mifare S70");
        break;
    default:
        tagtype = QString("unknown tag");

    }
    CHAR c_uid[32];
    memset(c_uid,0,sizeof(c_uid));
    BytesToHexStr(uid,uidlen,c_uid);

    QString suid = QString("%1").arg(c_uid);

    INT32 i;
    CTag_HF* pTag;
    for( i=0;i<m_tags_hf.size();i++)
    {
        pTag=(CTag_HF*)&m_tags_hf.at(i);
        if(pTag->m_uid == suid && pTag->m_antNo == ant_id)
        {
            break;
        }
    }
    if(i>=m_tags_hf.size())
    {
        CTag_HF newtag;
        newtag.m_counter = 1;
        newtag.m_uid = suid;
        newtag.m_type = picc_tid;
        newtag.m_antNo =ant_id;
        newtag.m_AIP =apl_tid ;
        newtag.m_rssi=0;
        m_tags_hf.push_back(newtag);
    }
    else
    {
        pTag->m_counter++;
        if(pTag->m_counter>=500000)
        {
            pTag->m_counter = 1;
        }
    }
}
