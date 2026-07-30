#include "CAEDevice_HF.h"
#include "gfunction.h"
#include <QElapsedTimer>


// 构造函数
// parent: 父对象(这里传入的是 QThread 线程对象，用于把本对象移到子线程运行)
CAEDevice_HF::CAEDevice_HF(QObject *parent) : QObject(parent)
{
    // 把内部信号 updateConfirmed 连接到事件循环的 quit 槽
    // 当主界面更新完成后触发 updateConfirmed，loopEvt 退出阻塞，继续下一轮盘点
    connect(this, &CAEDevice_HF::updateConfirmed, &loopEvt, &QEventLoop::quit);
}

CAEDevice_HF::~CAEDevice_HF()
{

}


// 启动盘点(预留接口，暂未实现具体逻辑)
err_t CAEDevice_HF::Start_Inventory()
{
    return 0;
}

// 停止盘点
// 原理: 把循环标志 loop 置为 false，Inventory 中的 while 循环就会自然退出
err_t CAEDevice_HF::End_Inventory()
{
    loop=false;
    return ERR_OK;
}


// 盘点主循环(槽函数，在子线程中执行)
// hreader: 读写器句柄, antennasSrc: 参与盘点的天线数组, ant_cnt: 天线个数
void CAEDevice_HF::Inventory(void* hreader,BYTE antennasSrc[], BYTE ant_cnt)
{
    err_t iret;
    DWORD tagCount ;

    // 保存天线信息到成员变量，供 func_Inventory 使用
    memset(antennas,0,ant_count);
    memcpy(antennas,antennasSrc,ant_cnt);
    ant_count=ant_cnt;
    hr = hreader;        // 保存读写器句柄

    int loop_count=0;            // 盘点轮数计数
    m_tags_hf.clear();           // 清空标签集合
    loop = true;                 // 设置循环标志为true，开始盘点循环
    QElapsedTimer timer;         // 计时器，用于统计每轮盘点耗时
    waitSgl =false;              // 初始不等待信号


    // 盘点主循环：每次循环执行一轮盘点，直到 loop 被置为 false(用户点停止)
    while(loop)
    {
        // 如果上一轮盘点后正在等待主界面更新完成，则阻塞在这里
        // 直到主界面发来 signals_updateComplited 信号，触发 updateConfirmed 唤醒
        if(waitSgl){

            loopEvt.exec();      // 进入事件循环并阻塞，等待 updateConfirmed 信号唤醒
            waitSgl = false;
        }
        timer.start();           // 开始计时
        m_tags_hf.clear();       // 清空上一轮的标签，准备本轮盘点

        iret = func_Inventory(); // 执行一次盘点
        qint64 sec = timer.elapsed();  // 获取本轮盘点耗时(毫秒)

        if(iret == NO_ERR)
        {
            loop_count++;        // 盘点成功，轮数+1
            tagCount =m_tags_hf.size();  // 本轮盘点到的标签数量
            // 发送信号给主界面：标签数、标签列表、耗时、轮数
            emit sgnl_inventory_data_hf(tagCount,m_tags_hf,sec,loop_count);
            waitSgl = true;      // 标记需要等待主界面更新完成后再继续下一轮
        }
        else
        {
            break;               // 盘点出错，退出循环
        }

    }

    // 盘点循环结束后，复位读写器通信超时状态
    if(hr != NULL)
        RDR_ResetCommuImmeTimeout(hr);
    loop=false;
    // 发送盘点结束信号给主界面(带错误码)
    emit sgnl_inventory_end_loop(iret);

}


// 盘点完成后的更新槽函数
// 由主界面更新完表格后发送 signals_updateComplited 信号触发
// 作用: 转发一个内部信号 updateConfirmed，用于唤醒事件循环 loopEvt，让子线程继续下一轮盘点
void CAEDevice_HF::onUpdateCompleted() {
    emit updateConfirmed();   // 发送内部唤醒信号
    waitSgl = false;          // 清除等待标志
}

// 执行一次盘点操作(核心盘点函数)
// 返回: 错误码，NO_ERR 表示成功
err_t CAEDevice_HF::func_Inventory()
{
    err_t iret;
    BYTE newAI= AI_TYPE_NEW;   // 盘点类型：只读新进入的标签(每次都从新开始)

    // RFID_DN_HANDLE dnInvenParamList= RFID_HANDLE_NULL;
    // 创建盘点参数列表，用于指定要盘点的协议类型
    RFID_DN_HANDLE dnInvenParamList= RFID_HANDLE_NULL;
    dnInvenParamList = RDR_CreateInvenParamSpecList();
    if(dnInvenParamList)
    {
        // 添加要盘点的协议：ISO15693 和 ISO14443A
        ISO15693_CreateInvenParam(dnInvenParamList,0,false,0,0x00);
        ISO14443A_CreateInvenParam(dnInvenParamList,0);
    }
    else
        return -ERR_MEM;       // 创建失败，返回内存错误

    // 执行盘点：用指定的天线集合按指定的协议参数列表进行盘点
    iret = RDR_TagInventory(hr,newAI,ant_count,antennas,dnInvenParamList);
    // 成功 或 停止触发(读到一定数量后自动停止)都视为可解析数据
    if(iret == NO_ERR || iret == -ERR_STOPTRRIGOCUR)
    {
        // 获取第一张标签的数据报告
        RFID_DN_HANDLE dnhReport = RDR_GetTagDataReport(hr,RFID_SEEK_FIRST);
        while(dnhReport != RFID_HANDLE_NULL)
        {
            DWORD AIPtype,TagType,AntId,readCount;  // 协议类型、标签类型、天线ID、读次数
            BYTE dsfid;                             // 数据存储格式标识
            WORD rssi;                              // 信号强度
            BYTE uid[8];                            // 标签UID(8字节)
            BYTE uidlen;                            // UID长度
            // 尝试按 ISO15693 协议解析标签数据报告
            int rtn= ISO15693_ParseTagDataReportEx(dnhReport,&AIPtype,&TagType,&AntId,&dsfid,&rssi,&readCount,uid);
            if(rtn == NO_ERR)
            {
                // 解析成功，添加到标签集合
                AddNewISO15693Tag(AIPtype,TagType,AntId,dsfid,uid,rssi);
            }
            // 尝试按 ISO14443A 协议解析标签数据报告
            rtn = ISO14443A_ParseTagDataReport(dnhReport,&AIPtype,&TagType,&AntId,uid,&uidlen);
            if(rtn  == NO_ERR)
            {
                // 解析成功，添加到标签集合
                AddNewISO14443ATag(AIPtype,TagType,AntId ,uid, uidlen);
            }
            // 获取下一张标签的数据报告
            dnhReport = RDR_GetTagDataReport(hr,RFID_SEEK_NEXT);
        }
        // 如果盘点是因为停止触发而结束的(读到一定数量)，仍视为成功
        if (iret == -ERR_STOPTRRIGOCUR) // stop trigger occur,need to inventory left tags
            iret = NO_ERR;
    }
    // 销毁盘点参数列表，释放资源
    if(dnInvenParamList)
    {
        DNODE_Destroy(dnInvenParamList);
    }
    return iret;
}


// 添加一张新发现的 ISO15693 标签到标签集合
// apl_tid: 空中协议类型, picc_tid: 标签类型ID, ant_id: 天线号, dsfid: 数据格式标识, uid: 标签UID(8字节), rssi: 信号强度
void CAEDevice_HF::AddNewISO15693Tag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 dsfid,UINT8 *uid,USHORT rssi)
{
    QString tagtype;
    // 根据 picc_tid(标签芯片类型ID)判断具体的标签型号名称
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
    // 把 UID(8字节)转换为十六进制字符串，例如 "E00401A012345678"
    CHAR suid[32];
    memset(suid,0,sizeof(suid));
    BytesToHexStr(uid,8,suid);
    // 把 dsfid(1字节)转为十六进制字符串
    CHAR sdsfid[8];
    memset(sdsfid,0,sizeof(sdsfid));
    BytesToHexStr(&dsfid,1,sdsfid);
    // 检查这张标签是否已经在标签集合中(根据UID和天线号判断)
    UINT32 i;
    CTag_HF* pTag;
    for( i=0;i<m_tags_hf.size();i++)
    {
        pTag=(CTag_HF*)&m_tags_hf.at(i);
        if(pTag->m_uid == suid && pTag->m_antNo == ant_id)
        {
            break;   // 找到了，跳出循环(i < size)
        }
    }
    // 如果没找到(i>=size)，说明是新标签，加入集合
    if(i>=m_tags_hf.size())
    {
        CTag_HF newtag;
        newtag.m_counter = 1;     // 读到次数=1
        newtag.m_uid =suid;       // 保存UID
        newtag.m_type = picc_tid;  // 保存标签类型
        newtag.m_antNo = ant_id;   // 保存天线号
        newtag.m_AIP =apl_tid ;    // 保存协议类型
        newtag.m_rssi=rssi;        // 保存信号强度
        m_tags_hf.push_back(newtag);
    }
    else
    {
        // 标签已存在，读到次数+1，并更新信号强度
        pTag->m_counter++;
        pTag->m_rssi=rssi;
        // 防止计数溢出，超过50万次则重置为1
        if(pTag->m_counter>=500000)
        {
            pTag->m_counter = 1;
        }
    }
}


// 添加一张新发现的 ISO14443A 标签到标签集合
// apl_tid: 空中协议类型, picc_tid: 标签类型ID, ant_id: 天线号, uid: 标签UID, uidlen: UID长度
void CAEDevice_HF::AddNewISO14443ATag(UINT32 apl_tid,UINT32 picc_tid,UINT32 ant_id,UINT8 *uid,UINT8 uidlen)
{
    QString tagtype;
    // 根据 picc_tid 判断标签型号名称
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
    // 把 UID 字节数组转换为十六进制字符串
    CHAR c_uid[32];
    memset(c_uid,0,sizeof(c_uid));
    BytesToHexStr(uid,uidlen,c_uid);

    QString suid = QString("%1").arg(c_uid);

    // 检查这张标签是否已在集合中(根据UID和天线号)
    INT32 i;
    CTag_HF* pTag;
    for( i=0;i<m_tags_hf.size();i++)
    {
        pTag=(CTag_HF*)&m_tags_hf.at(i);
        if(pTag->m_uid == suid && pTag->m_antNo == ant_id)
        {
            break;   // 找到了
        }
    }
    // 没找到则是新标签
    if(i>=m_tags_hf.size())
    {
        CTag_HF newtag;
        newtag.m_counter = 1;
        newtag.m_uid = suid;
        newtag.m_type = picc_tid;
        newtag.m_antNo =ant_id;
        newtag.m_AIP =apl_tid ;
        newtag.m_rssi=0;          // ISO14443A 不提供 RSSI，设为0
        m_tags_hf.push_back(newtag);
    }
    else
    {
        // 已存在则计数+1
        pTag->m_counter++;
        if(pTag->m_counter>=500000)
        {
            pTag->m_counter = 1;
        }
    }
}