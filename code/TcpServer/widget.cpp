#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1. 设置窗口标题
    this->setWindowTitle("服务端");

    // 2. 实例化对象
    tcpServer = new QTcpServer(this);

    // 3. 关联信号槽
    connect(tcpServer, &QTcpServer::newConnection, this, &Widget::processConnection);

    // 4. 绑定和监听
    bool ret = tcpServer->listen(QHostAddress::Any, 9090);
    if(!ret)
    {
        QMessageBox::critical(this, "服务器启动失败", tcpServer->errorString());
        exit(1);
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::processConnection()
{
    // 1. 通过TcpServer拿到一个Socket对象, 这个对象实现客户端和服务端的通信
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    QString log = "[" + clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort()) + "] 客户端上线!";
        ui->listWidget->addItem(log);

    // 2. 进行通信, 信号槽
    connect(clientSocket, &QTcpSocket::readyRead, this, [=](){
       // a) 读取出请求数据. 此处 readAll 返回的是 QByteArray, 通过赋值转成 QString
       QString request = clientSocket->readAll();
       // b) 根据请求处理响应
       const QString& response = process(request);
       // c) 把应答写回给客户端
       clientSocket->write(response.toUtf8());
       // d) 把上述信息记录到日志中.
       QString log = "[" + clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort()) + "] "
               + " req: " + request + ", resp: " + response;
       ui->listWidget->addItem(log);
    });


    // 3. 断开连接, 信号槽处理
    connect(clientSocket, &QTcpSocket::disconnected, this, [=](){
        // a) 把断开连接的信息通过日志显示出来.
        QString log = "[" + clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort()) + "] 客户端下线!";
        ui->listWidget->addItem(log);

        // b) 手动释放 clientSocket. 直接使用 delete 是下策, 使用 deleteLater 更加合适的.
        // delete clientSocket;
        clientSocket->deleteLater();
    });

}

QString Widget::process(const QString &request)
{
    // 回显客户端没啥特别的处理
    return request;
}

