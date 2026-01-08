#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 给第一个 label 设置成纯文本
    ui->label->setTextFormat(Qt::PlainText);
    // ui->label->setText("这是一段纯文本");
    // ui->label->setText("<b>这是一段纯文本</b>");
    ui->label->setText("# 这是一段纯文本");

    // 给第二个 label 设置成富文本
    ui->label_2->setTextFormat(Qt::RichText);
    ui->label_2->setText("<b>这是一段富文本</b>");

    // 给第三个 label 设置成 markdown
    ui->label_3->setTextFormat(Qt::MarkdownText);
    ui->label_3->setText("# 这是一段 Markdown");
}

Widget::~Widget()
{
    delete ui;
}

