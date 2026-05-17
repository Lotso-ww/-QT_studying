#include "widget.h"
#include "ui_widget.h"
#include <QNetworkRequest>
#include <QNetworkReply>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("客户端");

    manager = new QNetworkAccessManager(this);

    qDebug() << "Supports SSL:" << QSslSocket::supportsSsl();
    qDebug() << "SSL library version:" << QSslSocket::sslLibraryVersionString();
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    QUrl url(ui->lineEdit->text());

    // 构建请求
    QNetworkRequest request(url);

    // 发送请求 -- 会收到一个应答
    QNetworkReply *response = manager->get(request);

    // 通过信号槽处理响应
    connect(manager, &QNetworkAccessManager::finished, this, [=](){
        if(response->error() == QNetworkReply::NoError)
        {
            // 响应正确获取到了
            QString html = response->readAll();
            ui->plainTextEdit->setPlainText(html);
        }
        else
        {
            // 响应获取失败了
            ui->plainTextEdit->setPlainText(response->errorString());
        }

        response->deleteLater();
    });
}
