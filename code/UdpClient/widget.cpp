#include "widget.h"
#include "ui_widget.h"

const QString& serverIP = "127.0.0.1";
qint16 serverPort = 9090;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1. 初始化socket对象
    socket = new QUdpSocket(this);

    // 2. 设置窗口标题
    this->setWindowTitle("客户端");

    // 3. 连接信号槽, 处理服务器发来的应答
    connect(socket, &QUdpSocket::readyRead, this, &Widget::processResponse);
}

Widget::~Widget()
{
    delete ui;
}

// 发送请求
void Widget::on_pushButton_clicked()
{
    // 1. 获取输入框中的内容，并且构建请求
    const QString& text = ui->lineEdit->text();
    QNetworkDatagram requestDataGram(text.toUtf8(), QHostAddress(serverIP), serverPort);

    // 2. 发送请求
    socket->writeDatagram(requestDataGram);

    // 3. 显示在界面上
    ui->listWidget->addItem("客户端说: " + text);

    // 4. 清空输入框内容
    ui->lineEdit->setText("");
}

// 处理应答
void Widget::processResponse()
{
    // 1. 接收应答
    const QNetworkDatagram& responseDataGram = socket->receiveDatagram();
    QString response = responseDataGram.data();

    // 2. 显示在界面上
    ui->listWidget->addItem("服务器说: " + response);
}

