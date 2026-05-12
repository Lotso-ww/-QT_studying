#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1. 对象实例化出来
    socket = new QUdpSocket(this);

    // 2. 设置窗标题
    this->setWindowTitle("回显服务器");

    // 3. 关联信号槽
    connect(socket, &QUdpSocket::readyRead, this, &Widget::processRequest);

    // 4. 绑定
    bool flag = socket->bind(QHostAddress::Any, 9090);
    if(!flag)
    {
        QMessageBox::critical(this, "绑定失败", socket->errorString());
        return;
    }


}

Widget::~Widget()
{
    delete ui;
}

void Widget::processRequest()
{
    // 1. 读取并解析请求
    const QNetworkDatagram requestDataGram = socket->receiveDatagram();
    QString request = requestDataGram.data();

    // 2. 计算并处理请求
    const QString response = process(request);

    // 3. 响应写回客户端
    const QNetworkDatagram responseDataGram(response.toUtf8(), requestDataGram.senderAddress(), requestDataGram.senderPort());
    socket->writeDatagram(responseDataGram);

    // 4. 把这次交互的信息显示在界面上
    QString log = "[" + requestDataGram.senderAddress().toString() + ":" + QString::number(requestDataGram.senderPort())
            + "] req: " + request + ", resp: " + response;
    ui->listWidget->addItem(log);

}

QString Widget::process(const QString &request)
{
    // 当前是回显服务器, 请求和响应都是一样的
    // 但是我们实际上处理是有业务逻辑的
    return request;
}

