#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QListWidgetItem"
#include "qmessagebox.h"
#include "gfunction.h"
#include "./c++_lib/inc/rfidlib.h"

#include <QMetaType>
#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

using namespace Qt;
// 把 TCHAR(宽字符)字符串转换为 QString
// 在 UNICODE 模式下用 fromWCharArray，否则用本地编码转换
QString TCHARToQString(const TCHAR* str) {
#ifdef UNICODE
    return QString::fromWCharArray(str);
#else
    return QString::fromLocal8Bit(str);
#endif
}


// 主窗口构造函数：初始化界面，加载读写器驱动，枚举串口和驱动
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<std::vector<CTag_HF>>("vector<CTag_HF>");
    qRegisterMetaType<std::vector<CTag_HF>>("std::vector<CTag_HF>");

    int iret = -1;     // 操作返回值
    int drvCnt = 0;   // 已加载的驱动个数


    // 第1步：从程序目录加载读写器驱动，避免依赖当前工作目录或系统根目录
    QString driverPath = QDir(QCoreApplication::applicationDirPath()).filePath("Drivers");
    const wchar_t* connStrPtr = reinterpret_cast<const wchar_t*>(driverPath.utf16());
    iret = RDR_LoadReaderDrivers(connStrPtr);
    drvCnt = RDR_GetLoadedReaderDriverCount();
    qDebug() << "RDR_LoadReaderDrivers path:" << driverPath
             << "ret:" << iret
             << "driverCount:" << drvCnt;

    // 第2步：枚举本机所有可用的串口，填入"串口名"下拉框
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


    // 第3步：遍历所有已加载的驱动，筛选出"Reader"类型的驱动，填入"读写器类型"下拉框
    for(int i=0;i<drvCnt ;i++)
    {
        DWORD nSize ;
        CRdrInfo rdrinfo ;
        TCHAR tmp[64] ;
        memset(tmp,0,sizeof(tmp)) ;
        nSize = sizeof(tmp) / sizeof(TCHAR);
        // 定义要查询的驱动属性键名
        QString catalogStr = LOADED_RDRDVR_OPT_CATALOG;
        QString readerStr = RDRDVR_TYPE_READER;
        QString drvNameStr = LOADED_RDRDVR_OPT_NAME;
        QString drvIDStr = LOADED_RDRDVR_OPT_ID;
        QString commTypeStr = LOADED_RDRDVR_OPT_COMMTYPESUPPORTED;
        std::wstring wstr = catalogStr.toStdWString();
        // 把 QString 转为 LPCTSTR(宽字符指针)，供 SDK 函数使用
        const wchar_t* catalog = reinterpret_cast<const wchar_t*>(catalogStr.utf16());
        LPCTSTR pdrvName = reinterpret_cast<LPCTSTR>(drvNameStr.utf16());
        LPCTSTR pdrvID =reinterpret_cast<LPCTSTR>(drvIDStr.utf16());
        LPCTSTR pcommType =reinterpret_cast<LPCTSTR>(commTypeStr.utf16());


        // 读取驱动的"分类"属性
        RDR_GetLoadedReaderDriverOpt(i,catalog ,tmp, &nSize) ;
        rdrinfo.m_Catalog= TCHARToQString(tmp) ;
        if(rdrinfo.m_Catalog ==(RDRDVR_TYPE_READER)){
            // 是 Reader 类型，继续读取 名称、ID、支持的通信类型
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

            // 把驱动名称加入"读写器类型"下拉框
            ui->cmb_rdtype->addItem(rdrinfo.m_name);
        }
    }

    // 默认选中第一个串口
    if(ui->cmb_com_name->count() > 0)
    {
        ui->cmb_com_name->setCurrentIndex(0) ;
    }

}

// 获取天线数量，填充天线列表框(用 QListWidgetItem + 复选框表示)
// 默认第1个天线勾选，其余不勾选
void MainWindow::bind_antennas()
{
    ui->lw_ants->clear();
    DWORD ant_cnt=RDR_GetAntennaInterfaceCount(hr);  // 获取读写器天线数量
    QString str;
    for (int i = 1; i <= ant_cnt; ++i) {
        str.sprintf("#%d",i);                          // 显示文本: #1, #2, ...
        QListWidgetItem* chk=new QListWidgetItem();
        chk->setData(Qt::DisplayRole,"text");
        chk->setData(Qt::CheckStateRole, Checked);
        chk->setData(Qt::UserRole, i);   // 保存天线编号到 UserRole(从1开始)
        chk->setText(str);
        if(i==1)
            chk->setCheckState(Qt::CheckState::Checked);   // 第1个默认勾选
        else
            chk->setCheckState(Qt::CheckState::Unchecked); // 其余不勾选
        ui->lw_ants->addItem(chk);
    }
}

MainWindow::~MainWindow()
{
    // 读写器关闭会使标签句柄失效，必须先断开标签并清空本地状态。
    if (ht != nullptr)
        HF_TagDisconnect();
    if (hr != nullptr)
    {
        RDR_Close(hr);
        hr = nullptr;
    }
    delete ui;
}


// "打开读写器"按钮：根据界面选项生成连接字符串，打开读写器，创建子线程用于盘点
void MainWindow::on_pushButton_clicked()
{
    // 读取界面上的连接参数
    QString rdTypeStr = ui->cmb_rdtype->currentText();
    QString comStr = ui->cmb_com_name->currentText();
    QString baudStr = ui->cmb_com_baud->currentText();
    QString frameStr = ui->cmb_com_frame->currentText();
    QString commTypeStr = "COM";

    // 拼接 COM 连接字符串(此处拼接的 connStr 实际未使用，下面用 getConnectString 重新生成)
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

    // 调用 getConnectString 生成实际的连接字符串(根据当前通信方式)
    char connChar[255];
    char *pConn=connChar;
    getConnectString(pConn);
    wchar_t* conn = ANSIToUnicode(pConn);   // 转为宽字符
     LPCTSTR pconnStr =reinterpret_cast<LPCTSTR>(connStr.utf16());
    qDebug() << "RDR_Open conn:" << pConn;
    int iret = RDR_Open(conn,&hr) ;          // 打开读写器
    free(conn);
    if(iret == 0 ){                          // 参数和通信句柄打开成功
        // RDR_Open 可能只打开了 COM 句柄，并不保证读写器已经在线。
        // 通过需要设备回应的接口验证真实硬件连接。
        DWORD detectedAntCount = 0;
        int detectRet = RDR_DetectAntennaCount(hr, &detectedAntCount);
        int lastError = RDR_GetReaderLastReturnError(hr);
        qDebug() << "RDR_DetectAntennaCount ret:" << detectRet
                 << "lastError:" << lastError
                 << "count:" << detectedAntCount;
        // 部分读写器驱动不实现 RDR_DetectAntennaCount，会返回 -ERR_NOSYS。
        // 这表示接口不支持，不表示读写器连接失败，应继续使用已打开的句柄。
        const bool detectUnsupported = (detectRet == -ERR_NOSYS);
        if(detectRet != NO_ERR && !detectUnsupported)
        {
            RDR_Close(hr);
            hr = nullptr;
            QString errorText = QString("Reader probe failed: detect=%1, lastError=%2, antennaCount=%3")
                    .arg(detectRet).arg(lastError).arg(detectedAntCount);
            set_info(errorText, false);
            QMessageBox::warning(this, "Reader", errorText, QMessageBox::Ok);
            return;
        }

        DWORD antennaCount = RDR_GetAntennaInterfaceCount(hr);
        if(detectUnsupported)
        {
            set_info(QString("Reader opened; antenna probe is not supported"));
            qDebug() << "Reader antenna probe unsupported, fallback antennaCount:" << antennaCount;
        }
        else
        {
            set_info(QString("Open reader success"));
            qDebug() << "Reader antennaCount:" << antennaCount
                     << "detectedAntennaCount:" << detectedAntCount;
        }
        ui->pushButton_2->setEnabled(true);  // 启用"关闭"按钮
        ui->pushButton->setEnabled(false);   // 禁用"打开"按钮
        bind_antennas();                     // 填充天线列表

        // 创建子线程，并把 device 对象移到子线程运行(避免盘点阻塞界面)
        thread = new QThread();
        device = new CAEDevice_HF();
        device->moveToThread(thread);
        connect(thread, &QThread::finished, device, &QObject::deleteLater);
        // 连接信号和槽：主界面 <-> 子线程设备对象
        connect(this,&MainWindow::signals_Inventory,device,&CAEDevice_HF::Inventory);              // 主界面发盘点信号->子线程盘点
        connect(device,&CAEDevice_HF::sgnl_inventory_data_hf,this,&MainWindow::slot_inventory_data_hf); // 子线程盘点数据->主界面更新
        connect(device,&CAEDevice_HF::sgnl_inventory_end_loop,this,&MainWindow::slot_inventory_end_loop); // 子线程盘点结束->主界面
        connect(this,&MainWindow::signals_updateComplited,device,&CAEDevice_HF::onUpdateCompleted);   // 主界面更新完成->子线程唤醒
        thread->start();
    }
    else
    {
        set_info(QString("Open reader failed! err=%1").arg(iret), false);
        QMessageBox::warning(this, "Reader", QString("Open reader failed! err=%1").arg(iret), QMessageBox::Ok);
    }

}



// "关闭读写器"按钮：断开读写器，断开信号槽连接
void MainWindow::on_pushButton_2_clicked()
{

    if(running)        // 正在盘点则不允许关闭
    {
        return;
    }
    if (hr == nullptr)
    {
        resetTagConnectionState();
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(false);
        return;
    }

    // 标签句柄从属于读写器。关闭读写器前先释放它，避免下次打开继续使用旧句柄。
    HF_TagDisconnect();
    int iret = RDR_Close(hr);   // 关闭读写器
    if(iret == NO_ERR){
        hr = nullptr;
        resetTagConnectionState();
        ui->pushButton->setEnabled(true);    // 启用"打开"按钮
        ui->pushButton_2->setEnabled(false); // 禁用"关闭"按钮
        m_tags_hf.clear();
        ui->cmb_access_tags->clear();
        m_accessBlockSize = 4;
        set_info(QString("Reader closed"));
        // 断开所有信号槽连接
        disconnect(this,&MainWindow::signals_Inventory,device,&CAEDevice_HF::Inventory);
        disconnect(device,&CAEDevice_HF::sgnl_inventory_data_hf,this,&MainWindow::slot_inventory_data_hf);
        disconnect(device,&CAEDevice_HF::sgnl_inventory_end_loop,this,&MainWindow::slot_inventory_end_loop);
        disconnect(this,&MainWindow::signals_updateComplited,device,&CAEDevice_HF::onUpdateCompleted);
        thread->quit();
        thread->wait();
        delete thread;
        thread = nullptr;
        device = nullptr;
    }
    else
    {
        set_info(QString("Close reader failed! err=%1").arg(iret), false);
    }

}

// 设置控件可用状态，并立即处理事件和重绘(让界面即时响应)
void MainWindow::setWidgetEnable(QWidget *m_wid,bool enable)
{
    m_wid->setEnabled(enable);
    qApp->processEvents();   // 立即处理队列中的事件
    m_wid->repaint();        // 立即重绘
}

// 获取用户在列表框中选中的天线编号
// ants: 输出的天线编号数组, len: 输入为缓冲区容量，输出为选中的天线个数
void MainWindow::get_selected_antennas(BYTE ants[],int &len)
{
    DWORD ant_cnt=ui->lw_ants->count();   // 列表框中的总项数
    if(len < ant_cnt)                    // 缓冲区不够大则返回0
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

// 根据界面上的通信方式选择，生成对应的连接字符串
// connStr: 输出的连接字符串缓冲区(char*)
void MainWindow::getConnectString(char *&connStr)
{

    RDType=ui->cmb_rdtype->currentText();        // 读写器类型
    QString CommType=ui->cmb_commtype->currentText(); // 通信方式
    int CommIndex=ui->cmb_commtype->currentIndex();   // 通信方式索引: 0=COM, 1=USB, 2=TCP
    if(CommIndex==0)//COM 串口方式
    {
        QString COMName=ui->cmb_com_name->currentText();    // 串口名
        int BaudRate=ui->cmb_com_baud->currentText().toInt(); // 波特率
        QString Frame=ui->cmb_com_frame->currentText();    // 校验位

        sprintf(connStr,"RDType=%s;CommType=%s;COMName=%s;BaudRate=%d;Frame=%s;BusAddr=255",
                RDType.toStdString().c_str(),
                CommType.toStdString().c_str(),
                COMName.toStdString().c_str(),
                BaudRate,
                Frame.toStdString().c_str());
    }
    else if(CommIndex==1)//USB 方式
    {
        int AddrMode=ui->cmb_usb_opentype->currentIndex();  // 寻址方式: 0=不指定, 1=按序列号
        QString SerNum="\0";                                // 序列号
        if(AddrMode > 0)                                    // 按序列号打开
        {
            SerNum=ui->cmb_usb_sn_path->currentText();
            if(SerNum.isEmpty()|| SerNum.isNull())
            {
                *connStr=NULL;   // 没选设备，置空
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
    else if(CommIndex==2)//TCP 网络方式
    {
        QString RemoteIP=ui->txt_tcp_ip->text().trimmed();          // 远程IP
        int RemotePort=ui->cmb_tcp_port->currentText().section(':', 0, 0).toInt();    // 远程端口
        QString LocalIP=ui->cmb_local_tcp_ip->currentText().trimmed(); // 本地IP
        sprintf(connStr,"RDType=%s;CommType=%s;RemoteIP=%s;RemotePort=%d;LocalIP=%s",
                RDType.toStdString().c_str(),
                CommType.toStdString().c_str(),
                RemoteIP.toStdString().c_str(),
                RemotePort,LocalIP.toStdString().c_str());
    }
    printf("connStr :%s\n",connStr);
}


// 创建盘点结果表格(设置列标题和列宽)
void MainWindow::create_inventory_view()
{
    ui->tbw_inventory_tags->clear();
    QStringList strList;
    BYTE* szRate;
    int columns=6;

    // 表头: 天线号 | 空中协议 | 标签类型 | UID | 信号强度 | 读到次数
    strList<<tr("AntID")<<tr("Air Protocol")<<tr("Tag Type")<<tr("UID")<<tr("RSSI")<<tr("Read Count");
    columns=strList.length();
    szRate=new BYTE[columns]{1,1,1,2,1,1};  // 各列宽度比例: UID 列占2份，其余占1份

    ui->tbw_inventory_tags->setColumnCount(columns);
    ui->tbw_inventory_tags->setHorizontalHeaderLabels(strList);
    int wid=ui->tbw_inventory_tags->size().width()-38;  // 表格总宽度(减去滚动条等)
    ui->tbw_inventory_tags->setRowCount(0);
    // 按比例计算每列宽度
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

    // 表格行为设置：整行选中、单选、不可编辑
    ui->tbw_inventory_tags->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbw_inventory_tags->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbw_inventory_tags->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

// 重绘事件：当窗口首次加载或尺寸变化时，重建盘点表格
void MainWindow::paintEvent(QPaintEvent *e)
{
    QSize size = ui->tbw_inventory_tags->size();

    if(!loaded || size!=formSize ){   // 未加载过 或 尺寸有变化
        loaded =true;
        formSize = ui->tbw_inventory_tags->size();
        create_inventory_view();
    }

}

// 把盘点到的标签填充到"标签操作"下拉框，并初始化块地址下拉框
void MainWindow::bind_access_tags()
{
    ui->cmb_access_tags->clear();
    // 把每张标签的UID加入下拉框
    for(int i=0;i<m_tags_hf.size();i++)
    {
        ui->cmb_access_tags->addItem(m_tags_hf[i].m_uid);
    }

    // 填充"起始块"和"块数"下拉框: 起始块 0~31, 块数 1~32
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

// 接收子线程发来的盘点数据，更新标签集合和表格
// tag_count: 本轮盘点标签数, tags: 标签列表, use_time: 本轮耗时(ms), loop_count: 第几轮
void MainWindow::slot_inventory_data_hf(int tag_count,vector<CTag_HF> tags,int use_time,int loop_count)
{
    if(tag_count<0)//inventory terminaled  盘点已终止
    {
        return;
    }
    bool exist = false;
    int sum_count = m_tags_hf.size();   // 当前累计的标签数
    // 把本轮读到的标签合并到总集合中：已存在的则计数+1，新标签则添加
    for (int i = 0; i < tag_count; ++i) {
        exist = false;
        for (int j=0; j < sum_count;++j) {
            if(tags.at(i).equal(m_tags_hf.at(j)))   // UID相同视为同一张
            {
                m_tags_hf.at(j).m_counter ++;        // 计数+1
                exist = true;
                break;
            }
        }
        if(!exist)
        {
            m_tags_hf.push_back(tags.at(i));         // 新标签加入集合
        }
    }


    sum_count = m_tags_hf.size();

    // 更新表格行数和列数
    ui->tbw_inventory_tags->setRowCount(sum_count);

    ui->tbw_inventory_tags->setColumnCount(6);
    //表头: AntID | Air Protocol | Tag Type | UID | RSSI | Read Count
    for(int i=0;i < sum_count;i++)
    {
        // 依次填入: 天线号、协议类型、标签类型、UID、信号强度、读到次数
        ui->tbw_inventory_tags->setItem(i,0,new QTableWidgetItem(QString::number(m_tags_hf[i].m_antNo)));
        ui->tbw_inventory_tags->setItem(i,1,new QTableWidgetItem(QString::number(m_tags_hf[i].m_AIP)));
        ui->tbw_inventory_tags->setItem(i,2,new QTableWidgetItem(QString::number(m_tags_hf[i].m_type)));
        ui->tbw_inventory_tags->setItem(i,3,new QTableWidgetItem(QString(m_tags_hf[i].m_uid)));
        ui->tbw_inventory_tags->setItem(i,4,new QTableWidgetItem(QString::number(m_tags_hf[i].m_rssi)));
        ui->tbw_inventory_tags->setItem(i,5,new QTableWidgetItem(QString::number(m_tags_hf[i].m_counter)));

        // 所有单元格内容居中对齐
        ui->tbw_inventory_tags->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tbw_inventory_tags->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        //ui->tbw_inventory_tags;
    }


    // 刷新表格显示
    ui->tbw_inventory_tags->show();
    ui->tbw_inventory_tags->setVisible(true);
    ui->tbw_inventory_tags->update();


    // 更新底部统计信息：标签数、耗时、轮数
    ui->lbl_inventory->setText(QString("Tags: %1  \nTime: %2 ms\nLoop: %3").arg(sum_count).arg(use_time).arg(loop_count));

    // 通过队列连接通知子线程：主界面已更新完成，可以继续下一轮盘点
    QMetaObject::invokeMethod(this, [this]() {
            emit signals_updateComplited();
        }, Qt::QueuedConnection);



}

// 盘点循环结束槽(由子线程发送 sgnl_inventory_end_loop 触发)
// iret: 错误码，非0则弹窗提示
void MainWindow::slot_inventory_end_loop(int iret)
{
    if(iret!= ERR_OK)
    {
        QString errMsg = QString("err_t = %1 !").arg(iret);
        QMessageBox::information(this,"Inventory",errMsg,QMessageBox::Ok);

    }
    running =false;
    setWidgetEnable(ui->btn_inventory_start,true);  // 重新启用"开始盘点"按钮
    bind_access_tags();                              // 把标签填入"标签操作"下拉框
}

// "开始盘点"按钮：检查天线和读写器，发送盘点信号给子线程
void MainWindow::on_btn_inventory_start_clicked()
{

    setWidgetEnable(ui->btn_inventory_start,false);  // 禁用"开始"按钮(防止重复点击)

    // 获取选中的天线
    int ant_count=ui->lw_ants->count();
    BYTE ants[32];
    memset(ants,0,sizeof(ants));
    get_selected_antennas(ants,ant_count);
    if(ant_count<=0)                               // 没选天线
    {
        setWidgetEnable(ui->btn_inventory_start,true);
        QMessageBox::warning(this,"Inventory","No Antenna Selected!",QMessageBox::Ok);
        return;
    }
    if(hr == nullptr)                               // 未打开读写器
    {
        setWidgetEnable(ui->btn_inventory_start,true);
        QMessageBox::warning(this,"Inventory","Please open reader first!",QMessageBox::Ok);
        return;
    }

    // 清空表格内容
    ui->tbw_inventory_tags->clearContents();
    ui->tbw_inventory_tags->setRowCount(0);


    running =true;
    // 发送盘点信号给子线程，触发盘点
    QByteArray antennasData(reinterpret_cast<const char*>(ants), ant_count);
    qDebug() << "Start inventory, ant_count:" << ant_count << "ants:" << antennasData.toHex(' ');
    emit MainWindow::signals_Inventory(hr, antennasData, ant_count);

}


// USB打开方式下拉框变化槽
// index: 0=不指定地址, 1=按序列号打开
void MainWindow::on_cmb_usb_opentype_currentIndexChanged(int index)
{
    if(index==0)//no addr  不指定地址，清空序列号列表
    {
        ui->cmb_usb_sn_path->clear();
    }
    else if(index==1)//SN  按序列号打开
    {
        ui->cmb_usb_sn_path->clear();
        QString rdTypeQStr = ui->cmb_rdtype->currentText();
         const wchar_t* rdTypeWStr = reinterpret_cast<const wchar_t *>(rdTypeQStr.utf16());
        int cnt=HID_Enum(rdTypeWStr);   // 枚举当前类型的USB设备
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

// "停止盘点"按钮：调用设备的 End_Inventory 停止盘点循环
void MainWindow::on_btn_inventory_stop_clicked()
{
    if(device){
        device->End_Inventory();
    }
}

// "清空列表"按钮：清空盘点结果表格
void MainWindow::on_btn_inventory_clear_clicked()
{
    ui->tbw_inventory_tags->clearContents();
    ui->tbw_inventory_tags->setRowCount(0);
}

// "读块"按钮：读取标签指定起始块和块数的数据
void MainWindow::on_btn_access_read_block_clicked()
{
    if (!ensureAccessReady("Read block"))
        return;
    bool read_secsta=false;   // 是否读取安全状态位(这里不读)


    DWORD byts_read_out=0;    // 实际读出的字节数

    DWORD addr,num;
    addr=ui->cmb_access_block_start->currentIndex();  // 起始块地址
    num=ui->cmb_access_block_count->currentIndex()+1; // 读取块数(+1因为索引从0开始)
    DWORD nSize=(read_secsta ? num * (m_accessBlockSize + 1) : num * m_accessBlockSize);
    DWORD numOfBlks_Readed=0;
    QByteArray buffer(static_cast<int>(nSize), 0);
    // 调用ISO15693读多块命令
    int iret=ISO15693_ReadMultiBlocks(hr,ht,read_secsta,addr,num,&numOfBlks_Readed,
                                      reinterpret_cast<BYTE *>(buffer.data()),nSize,&byts_read_out);
    if (iret != NO_ERR)
    {
        set_info(QString("Read Block Failed! err=%1").arg(iret), false);
        return;
    }
    if (byts_read_out > static_cast<DWORD>(buffer.size()))
    {
        set_info(QString("Read Block Failed: invalid data length"), false);
        return;
    }
    // 把读出的字节数组转为十六进制字符串显示
    char* strs = new char[byts_read_out*2+1];
    memset(strs,0,byts_read_out*2+1);
    BytesToHexStr(reinterpret_cast<BYTE *>(buffer.data()),byts_read_out,strs);//字节数组转十六进制字符串
    ui->txt_access_block_data->setText(QString(strs));//char*转QString
    delete[] strs;
    set_info(QString("Read Block Success: start=%1, blocks=%2, bytes=%3")
             .arg(addr).arg(numOfBlks_Readed).arg(byts_read_out));

}

// 连接选中的HF标签(建立标签句柄，用于后续读写操作)
void MainWindow::HF_TagConnect()
{
    if (hr == nullptr)
    {
        set_info("Connect tag failed: reader is not open", false);
        return;
    }
    int index = ui->cmb_access_tags->currentIndex();  // 获取选中的标签索引
    if (index < 0 || index >= static_cast<int>(m_tags_hf.size()))
    {
        set_info("Connect tag failed: no tag selected", false);
        return;
    }
    if (!SetAccessAntenna())
        return;
    if (ht != nullptr)
        HF_TagDisconnect();
    CHAR addr_mode = 1;   // 地址模式: 1=按UID寻址

    // 计算UID字节数(UID是十六进制字符串，每2个字符代表1个字节)
    int ulen=m_tags_hf[index].m_uid.length()>2?m_tags_hf[index].m_uid.length()/2:0;
    if (ulen <= 0 || ulen > 255)
    {
        set_info("Connect tag failed: invalid UID", false);
        return;
    }
    int blen=ulen;
    int iret = 0;

    // 把UID十六进制字符串转为字节数组
    BYTE* byt = new BYTE[ulen];
    memset(byt,0,ulen);
    if (!HexStrToBytes(m_tags_hf[index].m_uid.toUtf8().constData(),byt,blen) || blen != ulen)
    {
        delete[] byt;
        set_info("Connect tag failed: invalid UID format", false);
        return;
    }

     // 连接标签，获取标签句柄 ht
    ht=NULL;
    iret = ISO15693_Connect(hr,m_tags_hf[index].m_type,addr_mode,byt,&ht);
    delete[] byt;
    if(iret ==ERR_OK)   // 连接成功
    {
        ui->btn_connect->setEnabled(false);  // 禁用"连接"按钮
        set_info(QString("Connect tag success: %1").arg(m_tags_hf[index].m_uid));
    }
    else
    {
        ht = nullptr;
        set_info(QString("Connect tag failed! err=%1").arg(iret), false);
    }
}

// 断开当前连接的HF标签
void MainWindow::HF_TagDisconnect()
{
    if(ht == NULL)
    {
        resetTagConnectionState();
        return;
    }
    int iret = (hr != nullptr) ? RDR_TagDisconnect(hr,ht) : NO_ERR;
    resetTagConnectionState();
    if (iret != NO_ERR)
        set_info(QString("Disconnect tag failed! err=%1").arg(iret), false);
    else
        set_info("Disconnect tag success");
}

// 设置标签访问使用的天线(取用户选中的第一个天线)
bool MainWindow::SetAccessAntenna()
{
    int ant_count=ui->lw_ants->count();
    BYTE ants[32];
    memset(ants,0,sizeof(ants));
    get_selected_antennas(ants,ant_count);
    if(ant_count>0){
        int iret = RDR_SetAcessAntenna(hr,ants[0]);  // 设置访问天线为选中的第一个
        if (iret == NO_ERR)
            return true;
        set_info(QString("Set access antenna failed! err=%1").arg(iret), false);
        return false;
    }
    else
    {
        QMessageBox::warning(this,"Tag Connect","Please select antenna!",QMessageBox::Ok);
        set_info("Set access antenna failed: no antenna selected", false);
    }
    return false;
}

// "连接标签"按钮
void MainWindow::on_btn_connect_clicked()
{
    HF_TagConnect();
}

// "断开标签"按钮
void MainWindow::on_btn_Disconnect_clicked()
{
    HF_TagDisconnect();
}

// "写块"按钮：把输入的十六进制数据写入标签指定块
void MainWindow::on_btn_access_write_block_clicked()
{
    if (!ensureAccessReady("Write block"))
        return;
    int block_addr = ui->cmb_access_block_start->currentIndex();  // 起始块地址
    int block_num = ui->cmb_access_block_count->currentIndex()+1; // 写入块数
    QString qstrs=ui->txt_access_block_data->text().trimmed();    // 获取输入的十六进制数据

    QByteArray data;
    QString error;
    const int expectedBytes = block_num * static_cast<int>(m_accessBlockSize);
    if (!parseHexInput(qstrs, expectedBytes, data, error))
    {
        set_info(QString("Write Block Failed: %1").arg(error), false);
        return;
    }
    // 调用ISO15693写多块命令
    int iret = ISO15693_WriteMultipleBlocks(hr,ht,block_addr,block_num,
                                             reinterpret_cast<BYTE *>(data.data()),data.size());

    if(iret!=NO_ERR)
        set_info(QString("Write Block Failed! err=%1").arg(iret),false);
    else
        set_info(QString("Write Block Success: start=%1, blocks=%2, bytes=%3")
                 .arg(block_addr).arg(block_num).arg(data.size()));

}

// "打开EAS"按钮：启用标签的EAS(电子商品防盗)功能
void MainWindow::on_btn_access_open_eas_clicked()
{
    if (!ensureAccessReady("Open EAS"))
        return;
    int iret=NXPICODESLI_EableEAS(hr,ht);
    if(iret!=NO_ERR)
        set_info(QString("Open EAS Failed! err=%1").arg(iret),false);
    else
        set_info(QString("Open EAS Success"));
}

// "关闭EAS"按钮：禁用标签的EAS功能
void MainWindow::on_btn_access_close_eas_clicked()
{
    if (!ensureAccessReady("Close EAS"))
        return;
    int iret=NXPICODESLI_DisableEAS(hr,ht);
    if(iret!=NO_ERR)
        set_info(QString("Close EAS Failed! err=%1").arg(iret),false);
    else
        set_info(QString("Close EAS Success"));
}

// 在界面底部显示状态信息
// str: 要显示的文字, success: 是否成功(目前未区分颜色显示)
void MainWindow::set_info(QString str,bool success)
{
    ui->lbl_info->setText(str);
    ui->lbl_info->setStyleSheet(success ? "color: #197a30;" : "color: #b42318;");
}

// "获取EAS状态"按钮：检查标签EAS是否已开启
void MainWindow::on_btn_access_get_eas_clicked()
{
    if (!ensureAccessReady("Get EAS"))
        return;
    BYTE eas_sta;   // EAS状态: 1=已开启, 0=已关闭
    int iret=NXPICODESLI_EASCheck(hr,ht,&eas_sta);
    if(iret!=NO_ERR)
    {
        set_info(QString("Get EAS Failed! err=%1").arg(iret),false);
    }
    else
    {
        set_info(QString("Get EAS Success!"));
        // 弹窗显示EAS状态
        QMessageBox::information(this,"Get EAS",QString("Eas is %1").arg(eas_sta==1?"Opened":"Closed"),QMessageBox::Ok);
    }

}

// "写AFI"按钮：写入AFI(应用族标识)值到标签
void MainWindow::on_btn_access_write_afi_clicked()
{
    if (!ensureAccessReady("Write AFI"))
        return;
    QByteArray afiData;
    QString error;
    if (!parseHexInput(ui->txt_access_afi->text(), 1, afiData, error))
    {
        set_info(QString("Write AFI Failed: %1").arg(error), false);
        return;
    }
    BYTE afi = static_cast<BYTE>(afiData.at(0));
    int iret = ISO15693_WriteAFI(hr,ht,(BYTE)afi);
    if(iret!=NO_ERR)
    {
        set_info(QString("Write AFI Failed! err=%1").arg(iret),false);
    }
    else{
        set_info(QString("Write AFI Success!"));
    }

}

// "获取标签状态/系统信息"按钮：读取标签的系统信息(DSFID、AFI、块大小、块数等)
void MainWindow::on_btn_access_get_status_clicked()
{
    if (!ensureAccessReady("Get status"))
        return;
    BYTE dsfid;       // 数据存储格式标识
    BYTE afi;         // 应用族标识
    DWORD block_size; // 每块字节数
    DWORD block_num;  // 总块数
    BYTE ic_ref;      // IC参考值
    int iret = 0;

    int index = ui->cmb_access_tags->currentIndex();  // 选中的标签索引
    if (index < 0 || index >= static_cast<int>(m_tags_hf.size()))
    {
        set_info("Get Status Failed: no tag selected", false);
        return;
    }

    // 把标签UID转为字节数组
    int ulen=m_tags_hf[index].m_uid.length()>2?m_tags_hf[index].m_uid.length()/2:0;
    BYTE *uid =new BYTE[ulen];

    HexStrToBytes(m_tags_hf[index].m_uid.toUtf8().data(),uid,ulen);



    // 调用ISO15693获取系统信息命令
    iret=ISO15693_GetSystemInfo(hr,ht,uid,&dsfid,&afi,&block_size,&block_num,&ic_ref);
    delete[] uid;
    if(iret == NO_ERR)
    {
        // 显示AFI值(2位十六进制，大写)
        ui->txt_access_afi->setText(QString("%1").arg((int)afi,2,16,QChar('0')).toUpper());
        if (block_size > 0)
            m_accessBlockSize = block_size;
        set_info(QString("Get Status Success: AFI=%1, block size=%2 bytes, blocks=%3")
                 .arg(afi, 2, 16, QChar('0')).toUpper().arg(block_size).arg(block_num));
    }
    else
        set_info(QString("Get Status Failed! err=%1").arg(iret), false);


}

bool MainWindow::ensureAccessReady(const QString &operation)
{
    if (hr == nullptr)
    {
        set_info(operation + " failed: reader is not open", false);
        return false;
    }
    if (ht == nullptr)
    {
        set_info(operation + " failed: tag is not connected", false);
        return false;
    }
    return true;
}

bool MainWindow::parseHexInput(const QString &text, int expectedBytes, QByteArray &data, QString &error) const
{
    QString normalized = text;
    normalized.remove(QRegularExpression("\\s"));
    if (normalized.isEmpty())
    {
        error = "data is empty";
        return false;
    }
    if (normalized.size() % 2 != 0 || !QRegularExpression("^[0-9A-Fa-f]+$").match(normalized).hasMatch())
    {
        error = "enter an even number of hexadecimal characters (0-9, A-F)";
        return false;
    }
    if (normalized.size() / 2 != expectedBytes)
    {
        error = QString("expected %1 bytes (%2 hex characters), got %3 bytes")
                .arg(expectedBytes).arg(expectedBytes * 2).arg(normalized.size() / 2);
        return false;
    }
    data = QByteArray::fromHex(normalized.toLatin1());
    return true;
}

void MainWindow::resetTagConnectionState()
{
    ht = nullptr;
    ui->btn_connect->setEnabled(true);
}
