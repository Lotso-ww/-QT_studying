#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 设置窗口标题
    this->setWindowTitle("客户端");

    // 实例化出对象
    socket = new QTcpSocket(this);

    // 建立连接
    socket->connectToHost("127.0.1", 9090);

    // 关联信号槽收到应答
    connect(socket, &QTcpSocket::readyRead, this, [=](){
        QString response = socket->readAll();
        ui->listWidget->addItem("服务端说: " + response);
    });

    // 等待连接完成
    bool ret = socket->waitForConnected();
    if(!ret)
    {
        QMessageBox::critical(this, "连接服务器失败", socket->errorString());
        exit(1);
    }
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    // 获取输入框中的内容
    const QString& text = ui->lineEdit->text();

    // 发送
    socket->write(text.toUtf8());

    // 显示在页面上
    ui->listWidget->addItem("客户端说: " + text);

    // 清空输入框的内容
    ui->lineEdit->setText("");
}
