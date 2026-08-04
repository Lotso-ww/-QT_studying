#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QListWidgetItem"
#include "qmessagebox.h"
#include "gfunction.h"
#include "rfidtagservice.h"
#include "tagpayloadcodec.h"
#include "./c++_lib/inc/rfidlib.h"

#include <QMetaType>
#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QHeaderView>
#include <QRegularExpression>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QRegularExpressionValidator>

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
    const int scanTabIndex = ui->myTabs->indexOf(ui->tab_scan_mode);
    if (scanTabIndex >= 0) {
        ui->myTabs->removeTab(scanTabIndex);
        ui->myTabs->insertTab(0, ui->tab_scan_mode, QStringLiteral("业务扫描"));
    }
    ui->myTabs->setTabText(ui->myTabs->indexOf(ui->tab_inventory), QStringLiteral("持续盘点"));
    ui->myTabs->setCurrentWidget(ui->tab_scan_mode);
    qRegisterMetaType<std::vector<CTag_HF>>("vector<CTag_HF>");
    qRegisterMetaType<std::vector<CTag_HF>>("std::vector<CTag_HF>");
    qRegisterMetaType<RfidOperationResult>("RfidOperationResult");

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

    // 强制覆盖旧版 ui_mainwindow.h 中 ScanMode 页签的禁用状态。
    ui->tab_scan_mode->setEnabled(true);
    create_business_view();
    businessLogger = new RfidLogDispatcher(this);
    const QString logDirectory = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("log"));
    QDir().mkpath(logDirectory);
    const QString logFileName = QStringLiteral("rfid-%1-%2.log")
            .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss-zzz")))
            .arg(QCoreApplication::applicationPid());
    const QString logFilePath = QDir(logDirectory).filePath(logFileName);
    businessLogger->setFileLoggingEnabled(true, logFilePath);
    connect(businessLogger, &RfidLogDispatcher::fileWriteFailed, this, [](const QString &message) {
        qWarning().noquote() << message;
    });
    log_business(RfidLogLevel::Info, QStringLiteral("INIT"), 0,
                 QStringLiteral("Reader drivers loaded."),
                 {{QStringLiteral("driverCount"), QString::number(drvCnt)},
                  {QStringLiteral("logFile"), logFilePath}});
    businessWorker = new RfidBusinessWorker(this);
    connect(businessWorker, &RfidBusinessWorker::completed, this, &MainWindow::on_business_completed);
    connect(businessWorker, &RfidBusinessWorker::attemptStarted,
            this, &MainWindow::on_business_attempt_started);
    connect(businessWorker, &RfidBusinessWorker::retryScheduled,
            this, &MainWindow::on_business_retry_scheduled);
    connect(businessWorker, &RfidBusinessWorker::deviceStage,
            this, &MainWindow::on_business_device_stage);
    update_business_tag_state();
}

void MainWindow::create_business_view()
{
    QGroupBox *group = new QGroupBox(QStringLiteral("业务数据"), ui->fra_access_hf);
    group->setGeometry(870, 78, 255, 116);
    QVBoxLayout *layout = new QVBoxLayout(group);
    QPushButton *readButton = new QPushButton(QStringLiteral("读取业务数据"), group);
    readButton->setObjectName(QStringLiteral("btn_business_read"));
    QPushButton *writeButton = new QPushButton(QStringLiteral("写入业务数据"), group);
    writeButton->setObjectName(QStringLiteral("btn_business_write"));
    layout->addWidget(readButton);
    layout->addWidget(writeButton);
    connect(readButton, &QPushButton::clicked, this, &MainWindow::on_business_read_clicked);
    connect(writeButton, &QPushButton::clicked, this, &MainWindow::on_business_write_clicked);
}

void MainWindow::on_business_read_clicked()
{
    InventoryObservation tag;
    if (!selected_business_tag(&tag))
        return;
    log_business(RfidLogLevel::Info, QStringLiteral("READ"), 0,
                 QStringLiteral("Reading connected tag."),
                 {{QStringLiteral("uid"), tag.uid}});
    findChild<QPushButton *>(QStringLiteral("btn_business_read"))->setEnabled(false);
    findChild<QPushButton *>(QStringLiteral("btn_business_write"))->setEnabled(false);
    RfidBusinessWorker *worker = businessWorker;
    void *reader = hr;
    void *connectedTag = ht;
    businessOperationRunning = true;
    businessReadOperation = true;
    ui->pushButton_2->setEnabled(false);
    QMetaObject::invokeMethod(worker, [worker, reader, connectedTag, tag]() {
        worker->read(reader, connectedTag, tag);
    }, Qt::QueuedConnection);
}

void MainWindow::on_business_write_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("写入业务数据"));
    QFormLayout *form = new QFormLayout(&dialog);
    QSpinBox *formatVersion = new QSpinBox(&dialog);
    formatVersion->setRange(0, 255);
    formatVersion->setValue(TagPayloadCodec::FormatVersion);
    formatVersion->setToolTip(QStringLiteral("0 到 255，写入标签的第一个字节。"));
    QSpinBox *dishNumber = new QSpinBox(&dialog);
    dishNumber->setRange(0, 255);
    dishNumber->setToolTip(QStringLiteral("0 到 255，写入标签的皿序号。"));
    QDateTimeEdit *inseminationTime = new QDateTimeEdit(QDateTime::currentDateTime(), &dialog);
    inseminationTime->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm"));
    inseminationTime->setMinimumDateTime(QDateTime(QDate(2000, 1, 1), QTime(0, 0)));
    inseminationTime->setMaximumDateTime(QDateTime(QDate(2099, 12, 31), QTime(23, 59)));
    inseminationTime->setToolTip(QStringLiteral("2000-01-01 00:00 到 2099-12-31 23:59。"));
    QLineEdit *femaleName = new QLineEdit(&dialog);
    femaleName->setPlaceholderText(QStringLiteral("1-4 个汉字，或最多 8 个英文字符"));
    femaleName->setToolTip(QStringLiteral("按 GBK 编码最多 8 字节，不足部分自动在前补空格。"));
    QLineEdit *medicalRecord = new QLineEdit(&dialog);
    medicalRecord->setPlaceholderText(QStringLiteral("1-64 位：A-Z、a-z、0-9、_、-"));
    medicalRecord->setValidator(new QRegularExpressionValidator(
            QRegularExpression(QStringLiteral("[A-Za-z0-9_-]{0,64}")), medicalRecord));
    medicalRecord->setToolTip(QStringLiteral("仅允许 ASCII 字母、数字、下划线和连字符。"));
    form->addRow(QStringLiteral("格式版本号（0-255）"), formatVersion);
    form->addRow(QStringLiteral("皿序号（0-255）"), dishNumber);
    form->addRow(QStringLiteral("授精时间"), inseminationTime);
    form->addRow(QStringLiteral("女方姓名"), femaleName);
    form->addRow(QStringLiteral("病历号"), medicalRecord);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted)
        return;

    TagPayload payload;
    payload.formatVersion = static_cast<quint8>(formatVersion->value());
    payload.dishNumber = static_cast<quint8>(dishNumber->value());
    payload.inseminationTime = inseminationTime->dateTime();
    payload.femaleName = femaleName->text();
    payload.medicalRecordNumber = medicalRecord->text().toLatin1();
    QByteArray raw;
    QString error;
    if (!TagPayloadCodec::encode(payload, &raw, &error)) {
        QMessageBox::warning(this, QStringLiteral("写入业务数据"), error, QMessageBox::Ok);
        return;
    }
    InventoryObservation tag;
    if (!selected_business_tag(&tag))
        return;
    log_business(RfidLogLevel::Info, QStringLiteral("WRITE"), 0,
                 QStringLiteral("Payload encoded."),
                 {{QStringLiteral("uid"), tag.uid},
                  {QStringLiteral("name"), payload.femaleName},
                  {QStringLiteral("medicalRecord"), QString::fromLatin1(payload.medicalRecordNumber)},
                  {QStringLiteral("payloadHex"), TagPayloadCodec::toHex(raw)}});
    if (QMessageBox::question(this, QStringLiteral("确认写入"),
                              QStringLiteral("将数据写入当前连接的标签，并在写入后自动读取校验。是否继续？"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    findChild<QPushButton *>(QStringLiteral("btn_business_read"))->setEnabled(false);
    findChild<QPushButton *>(QStringLiteral("btn_business_write"))->setEnabled(false);
    RfidBusinessWorker *worker = businessWorker;
    void *reader = hr;
    void *connectedTag = ht;
    businessOperationRunning = true;
    businessReadOperation = false;
    ui->pushButton_2->setEnabled(false);
    QMetaObject::invokeMethod(worker, [worker, reader, connectedTag, tag, payload]() {
        worker->write(reader, connectedTag, tag, payload);
    }, Qt::QueuedConnection);
}

void MainWindow::on_business_cancel_clicked()
{
    if (businessWorker) {
        RfidBusinessWorker *worker = businessWorker;
        QMetaObject::invokeMethod(worker, [worker]() { worker->cancel(); }, Qt::QueuedConnection);
    }
    log_business(RfidLogLevel::Warn, QStringLiteral("CANCEL"), 0,
                 QStringLiteral("Cancellation requested."));
}

bool MainWindow::selected_business_tag(InventoryObservation *tag) const
{
    if (!tag)
        return false;
    if (accessTagSource != AccessTagSource::ScanMode) {
        const_cast<MainWindow *>(this)->set_info(QStringLiteral("Business access requires a tag from Business Scan."), false);
        QMessageBox::information(const_cast<MainWindow *>(this), QStringLiteral("业务数据"),
                                 QStringLiteral("请先在“业务扫描”中扫描标签。持续盘点结果仅用于 Access 的块读写。"));
        return false;
    }
    if (hr == nullptr || ht == nullptr || m_tags_hf.size() != 1) {
        const_cast<MainWindow *>(this)->set_info(QStringLiteral("Connect the single Business Scan tag before business access."), false);
        return false;
    }
    const CTag_HF &item = m_tags_hf.front();
    tag->uid = item.m_uid;
    tag->tagType = item.m_type;
    tag->antenna = item.m_antNo;
    return true;
}

void MainWindow::on_business_completed(const RfidOperationResult &result)
{
    businessOperationRunning = false;
    const bool rawDataOnly = businessReadOperation && result.success && !result.payloadDecoded
            && !result.rawData.isEmpty();
    const RfidLogLevel level = result.success ? RfidLogLevel::Info : RfidLogLevel::Error;
    QMap<QString, QString> fields;
    fields.insert(QStringLiteral("sdkError"), QString::number(result.sdkErrorCode));
    if (!result.rawData.isEmpty())
        fields.insert(QStringLiteral("payloadHex"), TagPayloadCodec::toHex(result.rawData));
    if (result.success) {
        fields.insert(QStringLiteral("name"), result.payload.femaleName);
        fields.insert(QStringLiteral("medicalRecord"), QString::fromLatin1(result.payload.medicalRecordNumber));
    }
    log_business(level, result.success ? QStringLiteral("COMPLETE") : QStringLiteral("FAILED"),
                 result.attemptCount, result.message, fields);
    ui->pushButton_2->setEnabled(hr != nullptr);
    update_business_tag_state();

    if (businessReadOperation && result.success && result.payloadDecoded) {
        QDialog dialog(this);
        dialog.setWindowTitle(QStringLiteral("Business data"));
        dialog.setMinimumWidth(520);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QTableWidget *table = new QTableWidget(5, 2, &dialog);
        table->setHorizontalHeaderLabels({QStringLiteral("字段"), QStringLiteral("内容")});
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setFocusPolicy(Qt::NoFocus);

        const QList<QPair<QString, QString>> rows = {
            {QStringLiteral("格式版本号"), QStringLiteral("0x%1").arg(result.payload.formatVersion, 2, 16,
                                                                  QLatin1Char('0')).toUpper()},
            {QStringLiteral("皿序号"), QString::number(result.payload.dishNumber)},
            {QStringLiteral("授精时间"), result.payload.inseminationTime.toString(QStringLiteral("yyyy-MM-dd HH:mm"))},
            {QStringLiteral("女方姓名"), result.payload.femaleName},
            {QStringLiteral("病历号"), QString::fromLatin1(result.payload.medicalRecordNumber)}
        };
        for (int row = 0; row < rows.size(); ++row) {
            table->setItem(row, 0, new QTableWidgetItem(rows.at(row).first));
            table->setItem(row, 1, new QTableWidgetItem(rows.at(row).second));
        }
        table->resizeRowsToContents();
        layout->addWidget(table);

        QLabel *rawLabel = new QLabel(QStringLiteral("原始 HEX"), &dialog);
        QPlainTextEdit *rawHex = new QPlainTextEdit(&dialog);
        rawHex->setReadOnly(true);
        rawHex->setPlainText(TagPayloadCodec::toHex(result.rawData));
        rawHex->setFixedHeight(64);
        layout->addWidget(rawLabel);
        layout->addWidget(rawHex);

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
        connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        layout->addWidget(buttons);
        dialog.exec();
        return;
    }

    QMessageBox message(this);
    message.setWindowTitle(businessReadOperation ? QStringLiteral("Business data")
                                                  : QStringLiteral("Write business data"));
    message.setIcon(result.success ? QMessageBox::Information : QMessageBox::Warning);
    if (rawDataOnly) {
        const QString version = result.rawData.isEmpty()
                ? QStringLiteral("unknown")
                : QStringLiteral("0x%1").arg(static_cast<quint8>(result.rawData.at(0)), 2, 16,
                                               QLatin1Char('0')).toUpper();
        message.setText(QStringLiteral("Tag data was read successfully as raw data. Its format version is %1 and "
                                       "does not match the business-data layout used when writing. The tag was not changed.\n\n"
                                       "Details: %2\n\nSee Raw HEX below.")
                        .arg(version, result.message));
    } else {
        message.setText(result.success ? QStringLiteral("Tag data was written and verified.") : result.message);
    }
    if (!result.rawData.isEmpty())
        message.setDetailedText(QStringLiteral("Raw HEX:\n%1").arg(TagPayloadCodec::toHex(result.rawData)));
    message.exec();
}

void MainWindow::on_business_attempt_started(int attempt)
{
    log_business(RfidLogLevel::Info, QStringLiteral("ATTEMPT"), attempt,
                 QStringLiteral("Business operation started."));
}

void MainWindow::on_business_retry_scheduled(int failedAttempt, int delayMs)
{
    log_business(RfidLogLevel::Warn, QStringLiteral("RETRY"), failedAttempt,
                 QStringLiteral("Retry scheduled."),
                 {{QStringLiteral("delayMs"), QString::number(delayMs)}});
}

void MainWindow::on_business_device_stage(const QString &stage, const QString &message)
{
    log_business(RfidLogLevel::Info, stage, 0, message);
}

void MainWindow::log_business(RfidLogLevel level, const QString &stage, int attempt,
                              const QString &message, const QMap<QString, QString> &fields)
{
    if (businessLogger)
        businessLogger->log(level, stage, attempt, message, fields);
}

void MainWindow::update_business_tag_state()
{
    QPushButton *readButton = findChild<QPushButton *>(QStringLiteral("btn_business_read"));
    QPushButton *writeButton = findChild<QPushButton *>(QStringLiteral("btn_business_write"));
    if (!readButton || !writeButton)
        return;
    const bool canOperate = hr != nullptr && ht != nullptr && !running && !scanRunning
            && !businessOperationRunning && accessTagSource == AccessTagSource::ScanMode
            && m_tags_hf.size() == 1;
    readButton->setEnabled(canOperate);
    writeButton->setEnabled(canOperate);
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
    if (businessWorker)
        businessWorker->requestCancel();
    if (thread) {
        thread->quit();
        thread->wait();
    }
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
        log_business(RfidLogLevel::Info, QStringLiteral("INIT"), 1,
                     QStringLiteral("Reader opened successfully."),
                     {{QStringLiteral("antennaCount"), QString::number(antennaCount)},
                      {QStringLiteral("probeResult"), QString::number(detectRet)}});
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
        connect(this,&MainWindow::signals_ScanOnce,device,&CAEDevice_HF::ScanOnce);
        connect(device,&CAEDevice_HF::sgnl_inventory_data_hf,this,&MainWindow::slot_inventory_data_hf); // 子线程盘点数据->主界面更新
        connect(device,&CAEDevice_HF::sgnl_inventory_end_loop,this,&MainWindow::slot_inventory_end_loop); // 子线程盘点结束->主界面
        connect(device,&CAEDevice_HF::sgnl_scan_data_hf,this,&MainWindow::slot_scan_data_hf);
        connect(device,&CAEDevice_HF::sgnl_scan_finished,this,&MainWindow::slot_scan_finished);
        connect(this,&MainWindow::signals_updateComplited,device,&CAEDevice_HF::onUpdateCompleted);   // 主界面更新完成->子线程唤醒
        thread->start();
    }
    else
    {
        set_info(QString("Open reader failed! err=%1").arg(iret), false);
        log_business(RfidLogLevel::Error, QStringLiteral("INIT"), 1,
                     QStringLiteral("Reader open failed."),
                     {{QStringLiteral("sdkError"), QString::number(iret)}});
        QMessageBox::warning(this, "Reader", QString("Open reader failed! err=%1").arg(iret), QMessageBox::Ok);
    }

}



// "关闭读写器"按钮：断开读写器，断开信号槽连接
void MainWindow::on_pushButton_2_clicked()
{

    if(running || scanRunning || businessOperationRunning)        // 正在盘点、扫描或业务读写则不允许关闭
    {
        return;
    }
    if (hr == nullptr)
    {
        resetTagConnectionState();
        accessTagSource = AccessTagSource::None;
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
        accessTagSource = AccessTagSource::None;
        ui->cmb_access_tags->clear();
        m_accessBlockSize = 4;
        set_info(QString("Reader closed"));
        log_business(RfidLogLevel::Info, QStringLiteral("CLOSE"), 0,
                     QStringLiteral("Reader closed."));
        // 断开所有信号槽连接
        disconnect(this,&MainWindow::signals_Inventory,device,&CAEDevice_HF::Inventory);
        disconnect(this,&MainWindow::signals_ScanOnce,device,&CAEDevice_HF::ScanOnce);
        disconnect(device,&CAEDevice_HF::sgnl_inventory_data_hf,this,&MainWindow::slot_inventory_data_hf);
        disconnect(device,&CAEDevice_HF::sgnl_inventory_end_loop,this,&MainWindow::slot_inventory_end_loop);
        disconnect(device,&CAEDevice_HF::sgnl_scan_data_hf,this,&MainWindow::slot_scan_data_hf);
        disconnect(device,&CAEDevice_HF::sgnl_scan_finished,this,&MainWindow::slot_scan_finished);
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
        log_business(RfidLogLevel::Error, QStringLiteral("CLOSE"), 0,
                     QStringLiteral("Reader close failed."),
                     {{QStringLiteral("sdkError"), QString::number(iret)}});
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

    // 表头: 天线号 | 空中协议 | 标签类型 | UID | 信号强度 | 读到次数
    strList<<tr("AntID")<<tr("Air Protocol")<<tr("Tag Type")<<tr("UID")<<tr("RSSI")<<tr("Read Count");

    ui->tbw_inventory_tags->setColumnCount(strList.length());
    ui->tbw_inventory_tags->setHorizontalHeaderLabels(strList);
    ui->tbw_inventory_tags->setRowCount(0);

    // 表格行为设置：整行选中、单选、不可编辑
    ui->tbw_inventory_tags->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbw_inventory_tags->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbw_inventory_tags->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbw_inventory_tags->verticalHeader()->setVisible(true);
    ui->tbw_inventory_tags->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbw_inventory_tags->horizontalHeader()->setMinimumSectionSize(80);

}

void MainWindow::create_scan_mode_view()
{
    QStringList headers;
    headers << tr("AntID") << tr("Air Protocol") << tr("Tag Type")
            << tr("UID") << tr("RSSI") << tr("Read Count");
    ui->tbw_scan_mode_tags->clear();
    ui->tbw_scan_mode_tags->setColumnCount(headers.size());
    ui->tbw_scan_mode_tags->setHorizontalHeaderLabels(headers);
    ui->tbw_scan_mode_tags->setRowCount(0);
    ui->tbw_scan_mode_tags->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbw_scan_mode_tags->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbw_scan_mode_tags->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbw_scan_mode_tags->verticalHeader()->setVisible(true);
    ui->tbw_scan_mode_tags->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbw_scan_mode_tags->horizontalHeader()->setMinimumSectionSize(80);
}

// 重绘事件：当窗口首次加载或尺寸变化时，重建盘点表格
void MainWindow::paintEvent(QPaintEvent *e)
{
    QSize size = ui->tbw_inventory_tags->size();

    if(!loaded || size!=formSize ){   // 未加载过 或 尺寸有变化
        loaded =true;
        formSize = ui->tbw_inventory_tags->size();
        create_inventory_view();
        create_scan_mode_view();
    }

}

// 把盘点到的标签填充到"标签操作"下拉框，并初始化块地址下拉框
void MainWindow::bind_access_tags()
{
    ui->cmb_access_tags->clear();
    ui->cmb_access_block_start->clear();
    ui->cmb_access_block_count->clear();
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
    update_business_tag_state();
}

// "开始盘点"按钮：检查天线和读写器，发送盘点信号给子线程
void MainWindow::on_btn_inventory_start_clicked()
{

    if (scanRunning)
    {
        QMessageBox::warning(this, "Inventory", "A single scan is in progress.", QMessageBox::Ok);
        return;
    }

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

    // A tag handle belongs to the same reader session as inventory. Release it before
    // starting a new inventory, then use only the freshly discovered tag list.
    if (ht != nullptr)
        HF_TagDisconnect();
    m_tags_hf.clear();
    accessTagSource = AccessTagSource::Inventory;
    bind_access_tags();

    // 清空表格内容
    ui->tbw_inventory_tags->clearContents();
    ui->tbw_inventory_tags->setRowCount(0);


    running =true;
    update_business_tag_state();
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

void MainWindow::on_btn_scan_mode_start_clicked()
{
    if (running || scanRunning)
    {
        QMessageBox::warning(this, "Scan", "Another reader operation is in progress.", QMessageBox::Ok);
        return;
    }
    if (hr == nullptr || device == nullptr)
    {
        QMessageBox::warning(this, "Scan", "Please open reader first!", QMessageBox::Ok);
        return;
    }

    if (ht != nullptr)
        HF_TagDisconnect();

    m_tags_hf.clear();
    accessTagSource = AccessTagSource::ScanMode;
    bind_access_tags();

    int antCount = ui->lw_ants->count();
    BYTE antennas[32] = {0};
    get_selected_antennas(antennas, antCount);
    if (antCount <= 0)
    {
        QMessageBox::warning(this, "Scan", "No Antenna Selected!", QMessageBox::Ok);
        return;
    }

    ui->tbw_scan_mode_tags->clearContents();
    ui->tbw_scan_mode_tags->setRowCount(0);
    ui->lbl_scan_mode->setText("Tags: 0\nTime: 0 ms\nScan: running");
    scanRunning = true;
    update_business_tag_state();
    ui->btn_scan_mode_start->setEnabled(false);
    ui->btn_inventory_start->setEnabled(false);
    emit signals_ScanOnce(hr, QByteArray(reinterpret_cast<const char*>(antennas), antCount), antCount);
}

void MainWindow::on_btn_scan_mode_clear_clicked()
{
    if (scanRunning)
        return;
    ui->tbw_scan_mode_tags->clearContents();
    ui->tbw_scan_mode_tags->setRowCount(0);
    ui->lbl_scan_mode->setText("Tags: 0\nTime: 0 ms\nScan: 0");
    if (accessTagSource == AccessTagSource::ScanMode) {
        if (ht != nullptr)
            const_cast<MainWindow *>(this)->HF_TagDisconnect();
        m_tags_hf.clear();
        accessTagSource = AccessTagSource::None;
        bind_access_tags();
        update_business_tag_state();
    }
}

void MainWindow::slot_scan_data_hf(int tag_count, vector<CTag_HF> tags, int use_time)
{
    // 业务扫描结果是当前业务访问的标签集合。
    m_tags_hf = tags;
    accessTagSource = AccessTagSource::ScanMode;
    bind_access_tags();
    ui->tbw_scan_mode_tags->setRowCount(tag_count);
    for (int i = 0; i < tag_count; ++i)
    {
        const CTag_HF &tag = tags.at(i);
        const QString values[] = {
            QString::number(tag.m_antNo), QString::number(tag.m_AIP),
            QString::number(tag.m_type), tag.m_uid,
            QString::number(tag.m_rssi), QString::number(tag.m_counter)
        };
        for (int column = 0; column < 6; ++column)
        {
            QTableWidgetItem *item = new QTableWidgetItem(values[column]);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbw_scan_mode_tags->setItem(i, column, item);
        }
    }
    ui->lbl_scan_mode->setText(QString("Tags: %1\nTime: %2 ms\nScan: complete")
                               .arg(tag_count).arg(use_time));
    update_business_tag_state();
}

void MainWindow::slot_scan_finished(int iret)
{
    scanRunning = false;
    ui->btn_scan_mode_start->setEnabled(true);
    ui->btn_inventory_start->setEnabled(true);
    update_business_tag_state();
    if (iret != NO_ERR)
    {
        ui->lbl_scan_mode->setText(QString("Scan failed: err=%1").arg(iret));
        QMessageBox::warning(this, "Scan", QString("Scan failed: err=%1").arg(iret), QMessageBox::Ok);
        return;
    }
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
        const int lastError = RDR_GetReaderLastReturnError(hr);
        qWarning() << "ISO15693_ReadMultiBlocks failed"
                   << "sdkResult:" << iret
                   << "lastError:" << lastError
                   << "start:" << addr
                   << "count:" << num
                   << "blockSize:" << m_accessBlockSize
                   << "uid:" << ui->cmb_access_tags->currentText();
        set_info(QString("Read Block Failed! err=%1 lastError=%2").arg(iret).arg(lastError), false);
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
    if (running || scanRunning || businessOperationRunning)
    {
        set_info("Connect tag failed: stop inventory and wait for it to finish first", false);
        QMessageBox::warning(this, QStringLiteral("Tag access"),
                             QStringLiteral("The reader is busy with inventory. Stop inventory, wait until it has stopped, then connect the tag."),
                             QMessageBox::Ok);
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
    update_business_tag_state();
}

// 断开当前连接的HF标签
void MainWindow::HF_TagDisconnect()
{
    if(ht == NULL)
    {
        resetTagConnectionState();
        update_business_tag_state();
        return;
    }
    int iret = (hr != nullptr) ? RDR_TagDisconnect(hr,ht) : NO_ERR;
    resetTagConnectionState();
    if (iret != NO_ERR)
        set_info(QString("Disconnect tag failed! err=%1").arg(iret), false);
    else
        set_info("Disconnect tag success");
    update_business_tag_state();
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
    if (running || scanRunning || businessOperationRunning)
    {
        set_info(operation + " failed: reader is busy with inventory", false);
        QMessageBox::warning(this, QStringLiteral("Tag access"),
                             QStringLiteral("Stop inventory and wait until it has finished before accessing a tag."),
                             QMessageBox::Ok);
        return false;
    }
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
