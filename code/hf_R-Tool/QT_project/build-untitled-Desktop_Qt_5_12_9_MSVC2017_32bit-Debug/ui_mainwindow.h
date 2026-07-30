/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QComboBox *cmb_rdtype;
    QLabel *label_5;
    QComboBox *cmb_commtype;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_3;
    QComboBox *cmb_com_frame;
    QLabel *label_4;
    QLabel *label_3;
    QComboBox *cmb_com_name;
    QLabel *label_2;
    QComboBox *cmb_com_baud;
    QGroupBox *groupBox_4;
    QLabel *label_9;
    QComboBox *cmb_usb_sn_path;
    QComboBox *cmb_usb_opentype;
    QLabel *label_8;
    QGroupBox *groupBox_2;
    QLabel *label_11;
    QComboBox *cmb_local_tcp_ip;
    QLabel *label_12;
    QLineEdit *txt_tcp_ip;
    QLabel *label_10;
    QComboBox *cmb_tcp_port;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_10;
    QVBoxLayout *verticalLayout_2;
    QListWidget *lw_ants;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QTabWidget *myTabs;
    QWidget *tab_inventory;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *tbw_inventory_tags;
    QGroupBox *groupBox_5;
    QLabel *lbl_inventory;
    QPushButton *btn_inventory_start;
    QPushButton *btn_inventory_stop;
    QPushButton *btn_inventory_clear;
    QWidget *tab_access_hf;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_6;
    QFrame *fra_access_hf;
    QPushButton *btn_access_write_afi;
    QLineEdit *txt_access_afi;
    QPushButton *btn_access_read_block;
    QLabel *label_25;
    QLabel *label_16;
    QPushButton *btn_access_write_block;
    QLabel *label_13;
    QPushButton *btn_access_close_eas;
    QLabel *label_14;
    QComboBox *cmb_access_block_count;
    QComboBox *cmb_access_block_start;
    QLabel *label_15;
    QLineEdit *txt_access_block_data;
    QLabel *label_17;
    QPushButton *btn_access_get_eas;
    QComboBox *cmb_access_tags;
    QLabel *lbl_info;
    QPushButton *btn_access_open_eas;
    QLabel *label_24;
    QPushButton *btn_access_get_status;
    QPushButton *btn_connect;
    QPushButton *btn_Disconnect;
    QWidget *tab_scan_mode;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_9;
    QTableWidget *tbw_scan_mode_tags;
    QVBoxLayout *verticalLayout_9;
    QLabel *lbl_scan_mode;
    QPushButton *btn_scan_mode_start;
    QPushButton *btn_scan_mode_stop;
    QPushButton *btn_scan_mode_clear;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1237, 768);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setMaximumSize(QSize(100, 19));
        label->setContextMenuPolicy(Qt::DefaultContextMenu);
        label->setAcceptDrops(false);
        label->setAutoFillBackground(false);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label->setIndent(-1);

        horizontalLayout_3->addWidget(label);

        cmb_rdtype = new QComboBox(centralwidget);
        cmb_rdtype->setObjectName(QString::fromUtf8("cmb_rdtype"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cmb_rdtype->sizePolicy().hasHeightForWidth());
        cmb_rdtype->setSizePolicy(sizePolicy2);
        cmb_rdtype->setMaximumSize(QSize(200, 30));

        horizontalLayout_3->addWidget(cmb_rdtype);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMaximumSize(QSize(200, 19));
        label_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_5);

        cmb_commtype = new QComboBox(centralwidget);
        cmb_commtype->addItem(QString());
        cmb_commtype->addItem(QString());
        cmb_commtype->addItem(QString());
        cmb_commtype->setObjectName(QString::fromUtf8("cmb_commtype"));
        cmb_commtype->setMaximumSize(QSize(200, 30));

        horizontalLayout_3->addWidget(cmb_commtype);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 1);
        horizontalLayout_3->setStretch(2, 2);
        horizontalLayout_3->setStretch(3, 2);

        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMaximumSize(QSize(16777215, 121));
        cmb_com_frame = new QComboBox(groupBox_3);
        cmb_com_frame->addItem(QString());
        cmb_com_frame->addItem(QString());
        cmb_com_frame->addItem(QString());
        cmb_com_frame->setObjectName(QString::fromUtf8("cmb_com_frame"));
        cmb_com_frame->setGeometry(QRect(150, 80, 135, 25));
        cmb_com_frame->setEditable(false);
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(60, 80, 72, 16));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(60, 50, 72, 16));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cmb_com_name = new QComboBox(groupBox_3);
        cmb_com_name->setObjectName(QString::fromUtf8("cmb_com_name"));
        cmb_com_name->setGeometry(QRect(150, 20, 135, 25));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 20, 130, 16));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cmb_com_baud = new QComboBox(groupBox_3);
        cmb_com_baud->addItem(QString());
        cmb_com_baud->addItem(QString());
        cmb_com_baud->addItem(QString());
        cmb_com_baud->setObjectName(QString::fromUtf8("cmb_com_baud"));
        cmb_com_baud->setGeometry(QRect(150, 50, 135, 25));

        horizontalLayout->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(20, 70, 80, 25));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cmb_usb_sn_path = new QComboBox(groupBox_4);
        cmb_usb_sn_path->setObjectName(QString::fromUtf8("cmb_usb_sn_path"));
        cmb_usb_sn_path->setGeometry(QRect(103, 67, 180, 25));
        cmb_usb_opentype = new QComboBox(groupBox_4);
        cmb_usb_opentype->addItem(QString());
        cmb_usb_opentype->addItem(QString());
        cmb_usb_opentype->setObjectName(QString::fromUtf8("cmb_usb_opentype"));
        cmb_usb_opentype->setGeometry(QRect(103, 27, 180, 25));
        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 30, 80, 25));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(groupBox_4);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMaximumSize(QSize(16777215, 121));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(36, 50, 80, 25));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cmb_local_tcp_ip = new QComboBox(groupBox_2);
        cmb_local_tcp_ip->setObjectName(QString::fromUtf8("cmb_local_tcp_ip"));
        cmb_local_tcp_ip->setGeometry(QRect(120, 80, 141, 25));
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(36, 80, 80, 25));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txt_tcp_ip = new QLineEdit(groupBox_2);
        txt_tcp_ip->setObjectName(QString::fromUtf8("txt_tcp_ip"));
        txt_tcp_ip->setGeometry(QRect(120, 20, 145, 25));
        txt_tcp_ip->setFrame(true);
        txt_tcp_ip->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(36, 20, 80, 25));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cmb_tcp_port = new QComboBox(groupBox_2);
        cmb_tcp_port->addItem(QString());
        cmb_tcp_port->addItem(QString());
        cmb_tcp_port->addItem(QString());
        cmb_tcp_port->addItem(QString());
        cmb_tcp_port->setObjectName(QString::fromUtf8("cmb_tcp_port"));
        cmb_tcp_port->setGeometry(QRect(120, 50, 141, 25));

        horizontalLayout->addWidget(groupBox_2);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMaximumSize(QSize(16777215, 121));
        verticalLayout_10 = new QVBoxLayout(groupBox);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        lw_ants = new QListWidget(groupBox);
        lw_ants->setObjectName(QString::fromUtf8("lw_ants"));

        verticalLayout_2->addWidget(lw_ants);


        verticalLayout_10->addLayout(verticalLayout_2);


        horizontalLayout->addWidget(groupBox);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);
        horizontalLayout->setStretch(2, 1);
        horizontalLayout->setStretch(3, 1);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(30);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy3);
        pushButton_2->setMinimumSize(QSize(150, 30));

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        sizePolicy3.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy3);
        pushButton->setMinimumSize(QSize(150, 30));

        horizontalLayout_2->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        myTabs = new QTabWidget(centralwidget);
        myTabs->setObjectName(QString::fromUtf8("myTabs"));
        myTabs->setEnabled(true);
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(myTabs->sizePolicy().hasHeightForWidth());
        myTabs->setSizePolicy(sizePolicy4);
        myTabs->setMaximumSize(QSize(16777215, 610));
        myTabs->setFocusPolicy(Qt::ClickFocus);
        myTabs->setTabShape(QTabWidget::Rounded);
        tab_inventory = new QWidget();
        tab_inventory->setObjectName(QString::fromUtf8("tab_inventory"));
        tab_inventory->setEnabled(true);
        verticalLayout_4 = new QVBoxLayout(tab_inventory);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        groupBox_6 = new QGroupBox(tab_inventory);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        verticalLayout_5 = new QVBoxLayout(groupBox_6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        tbw_inventory_tags = new QTableWidget(groupBox_6);
        tbw_inventory_tags->setObjectName(QString::fromUtf8("tbw_inventory_tags"));
        tbw_inventory_tags->setMaximumSize(QSize(16777215, 16777215));
        tbw_inventory_tags->setBaseSize(QSize(0, 0));

        verticalLayout_3->addWidget(tbw_inventory_tags);


        verticalLayout_5->addLayout(verticalLayout_3);


        horizontalLayout_6->addWidget(groupBox_6);

        groupBox_5 = new QGroupBox(tab_inventory);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setEnabled(true);
        lbl_inventory = new QLabel(groupBox_5);
        lbl_inventory->setObjectName(QString::fromUtf8("lbl_inventory"));
        lbl_inventory->setGeometry(QRect(10, 20, 100, 81));
        sizePolicy2.setHeightForWidth(lbl_inventory->sizePolicy().hasHeightForWidth());
        lbl_inventory->setSizePolicy(sizePolicy2);
        lbl_inventory->setMaximumSize(QSize(100, 81));
        QFont font;
        font.setPointSize(12);
        lbl_inventory->setFont(font);
        lbl_inventory->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        btn_inventory_start = new QPushButton(groupBox_5);
        btn_inventory_start->setObjectName(QString::fromUtf8("btn_inventory_start"));
        btn_inventory_start->setGeometry(QRect(10, 110, 135, 35));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(btn_inventory_start->sizePolicy().hasHeightForWidth());
        btn_inventory_start->setSizePolicy(sizePolicy5);
        btn_inventory_start->setMaximumSize(QSize(135, 35));
        btn_inventory_stop = new QPushButton(groupBox_5);
        btn_inventory_stop->setObjectName(QString::fromUtf8("btn_inventory_stop"));
        btn_inventory_stop->setGeometry(QRect(10, 160, 135, 35));
        sizePolicy2.setHeightForWidth(btn_inventory_stop->sizePolicy().hasHeightForWidth());
        btn_inventory_stop->setSizePolicy(sizePolicy2);
        btn_inventory_stop->setMaximumSize(QSize(135, 35));
        btn_inventory_clear = new QPushButton(groupBox_5);
        btn_inventory_clear->setObjectName(QString::fromUtf8("btn_inventory_clear"));
        btn_inventory_clear->setGeometry(QRect(10, 210, 135, 35));
        sizePolicy2.setHeightForWidth(btn_inventory_clear->sizePolicy().hasHeightForWidth());
        btn_inventory_clear->setSizePolicy(sizePolicy2);
        btn_inventory_clear->setMaximumSize(QSize(135, 35));

        horizontalLayout_6->addWidget(groupBox_5);

        horizontalLayout_6->setStretch(0, 9);
        horizontalLayout_6->setStretch(1, 2);

        verticalLayout_4->addLayout(horizontalLayout_6);

        myTabs->addTab(tab_inventory, QString());
        tab_access_hf = new QWidget();
        tab_access_hf->setObjectName(QString::fromUtf8("tab_access_hf"));
        horizontalLayout_5 = new QHBoxLayout(tab_access_hf);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        fra_access_hf = new QFrame(tab_access_hf);
        fra_access_hf->setObjectName(QString::fromUtf8("fra_access_hf"));
        fra_access_hf->setFrameShape(QFrame::NoFrame);
        fra_access_hf->setFrameShadow(QFrame::Raised);
        btn_access_write_afi = new QPushButton(fra_access_hf);
        btn_access_write_afi->setObjectName(QString::fromUtf8("btn_access_write_afi"));
        btn_access_write_afi->setGeometry(QRect(273, 236, 130, 31));
        txt_access_afi = new QLineEdit(fra_access_hf);
        txt_access_afi->setObjectName(QString::fromUtf8("txt_access_afi"));
        txt_access_afi->setGeometry(QRect(161, 236, 61, 25));
        txt_access_afi->setFrame(true);
        txt_access_afi->setAlignment(Qt::AlignCenter);
        btn_access_read_block = new QPushButton(fra_access_hf);
        btn_access_read_block->setObjectName(QString::fromUtf8("btn_access_read_block"));
        btn_access_read_block->setGeometry(QRect(418, 139, 130, 31));
        label_25 = new QLabel(fra_access_hf);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setGeometry(QRect(126, 238, 31, 21));
        label_25->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_16 = new QLabel(fra_access_hf);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(297, 69, 111, 25));
        label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        btn_access_write_block = new QPushButton(fra_access_hf);
        btn_access_write_block->setObjectName(QString::fromUtf8("btn_access_write_block"));
        btn_access_write_block->setGeometry(QRect(273, 139, 130, 31));
        label_13 = new QLabel(fra_access_hf);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(46, 29, 80, 25));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        btn_access_close_eas = new QPushButton(fra_access_hf);
        btn_access_close_eas->setObjectName(QString::fromUtf8("btn_access_close_eas"));
        btn_access_close_eas->setGeometry(QRect(273, 188, 130, 31));
        label_14 = new QLabel(fra_access_hf);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(45, 109, 80, 25));
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cmb_access_block_count = new QComboBox(fra_access_hf);
        cmb_access_block_count->setObjectName(QString::fromUtf8("cmb_access_block_count"));
        cmb_access_block_count->setGeometry(QRect(411, 69, 138, 25));
        cmb_access_block_start = new QComboBox(fra_access_hf);
        cmb_access_block_start->setObjectName(QString::fromUtf8("cmb_access_block_start"));
        cmb_access_block_start->setGeometry(QRect(128, 68, 141, 25));
        label_15 = new QLabel(fra_access_hf);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(4, 69, 121, 25));
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txt_access_block_data = new QLineEdit(fra_access_hf);
        txt_access_block_data->setObjectName(QString::fromUtf8("txt_access_block_data"));
        txt_access_block_data->setGeometry(QRect(128, 109, 421, 26));
        label_17 = new QLabel(fra_access_hf);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(560, 115, 25, 21));
        QFont font1;
        font1.setPointSize(10);
        label_17->setFont(font1);
        label_17->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        btn_access_get_eas = new QPushButton(fra_access_hf);
        btn_access_get_eas->setObjectName(QString::fromUtf8("btn_access_get_eas"));
        btn_access_get_eas->setGeometry(QRect(418, 188, 130, 31));
        cmb_access_tags = new QComboBox(fra_access_hf);
        cmb_access_tags->setObjectName(QString::fromUtf8("cmb_access_tags"));
        cmb_access_tags->setGeometry(QRect(129, 28, 421, 25));
        lbl_info = new QLabel(fra_access_hf);
        lbl_info->setObjectName(QString::fromUtf8("lbl_info"));
        lbl_info->setGeometry(QRect(60, 270, 500, 30));
        QFont font2;
        font2.setPointSize(11);
        lbl_info->setFont(font2);
        lbl_info->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        btn_access_open_eas = new QPushButton(fra_access_hf);
        btn_access_open_eas->setObjectName(QString::fromUtf8("btn_access_open_eas"));
        btn_access_open_eas->setGeometry(QRect(128, 188, 130, 31));
        label_24 = new QLabel(fra_access_hf);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setGeometry(QRect(230, 241, 26, 21));
        label_24->setFont(font1);
        label_24->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        btn_access_get_status = new QPushButton(fra_access_hf);
        btn_access_get_status->setObjectName(QString::fromUtf8("btn_access_get_status"));
        btn_access_get_status->setGeometry(QRect(418, 236, 130, 31));
        btn_connect = new QPushButton(fra_access_hf);
        btn_connect->setObjectName(QString::fromUtf8("btn_connect"));
        btn_connect->setGeometry(QRect(580, 20, 111, 41));
        btn_Disconnect = new QPushButton(fra_access_hf);
        btn_Disconnect->setObjectName(QString::fromUtf8("btn_Disconnect"));
        btn_Disconnect->setGeometry(QRect(730, 20, 111, 41));

        verticalLayout_6->addWidget(fra_access_hf);


        horizontalLayout_5->addLayout(verticalLayout_6);

        myTabs->addTab(tab_access_hf, QString());
        tab_scan_mode = new QWidget();
        tab_scan_mode->setObjectName(QString::fromUtf8("tab_scan_mode"));
        tab_scan_mode->setEnabled(false);
        horizontalLayout_10 = new QHBoxLayout(tab_scan_mode);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        tbw_scan_mode_tags = new QTableWidget(tab_scan_mode);
        tbw_scan_mode_tags->setObjectName(QString::fromUtf8("tbw_scan_mode_tags"));
        tbw_scan_mode_tags->setBaseSize(QSize(791, 323));

        horizontalLayout_9->addWidget(tbw_scan_mode_tags);


        horizontalLayout_10->addLayout(horizontalLayout_9);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        lbl_scan_mode = new QLabel(tab_scan_mode);
        lbl_scan_mode->setObjectName(QString::fromUtf8("lbl_scan_mode"));
        lbl_scan_mode->setFont(font);
        lbl_scan_mode->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_9->addWidget(lbl_scan_mode);

        btn_scan_mode_start = new QPushButton(tab_scan_mode);
        btn_scan_mode_start->setObjectName(QString::fromUtf8("btn_scan_mode_start"));

        verticalLayout_9->addWidget(btn_scan_mode_start);

        btn_scan_mode_stop = new QPushButton(tab_scan_mode);
        btn_scan_mode_stop->setObjectName(QString::fromUtf8("btn_scan_mode_stop"));

        verticalLayout_9->addWidget(btn_scan_mode_stop);

        btn_scan_mode_clear = new QPushButton(tab_scan_mode);
        btn_scan_mode_clear->setObjectName(QString::fromUtf8("btn_scan_mode_clear"));

        verticalLayout_9->addWidget(btn_scan_mode_clear);


        horizontalLayout_10->addLayout(verticalLayout_9);

        horizontalLayout_10->setStretch(0, 8);
        horizontalLayout_10->setStretch(1, 2);
        myTabs->addTab(tab_scan_mode, QString());

        horizontalLayout_4->addWidget(myTabs);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 3);
        verticalLayout->setStretch(2, 1);
        verticalLayout->setStretch(3, 15);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1237, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        cmb_local_tcp_ip->setCurrentIndex(-1);
        cmb_tcp_port->setCurrentIndex(0);
        myTabs->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "<html><head/><body><p>ReaderType:</p></body></html>", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Communication Type", nullptr));
        cmb_commtype->setItemText(0, QApplication::translate("MainWindow", "COM", nullptr));
        cmb_commtype->setItemText(1, QApplication::translate("MainWindow", "USB", nullptr));
        cmb_commtype->setItemText(2, QApplication::translate("MainWindow", "TCP", nullptr));

        groupBox_3->setTitle(QApplication::translate("MainWindow", "COM", nullptr));
        cmb_com_frame->setItemText(0, QApplication::translate("MainWindow", "8N1", nullptr));
        cmb_com_frame->setItemText(1, QApplication::translate("MainWindow", "8E1", nullptr));
        cmb_com_frame->setItemText(2, QApplication::translate("MainWindow", "8O1", nullptr));

        label_4->setText(QApplication::translate("MainWindow", "frame:", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "<html><head/><body><p>Baud:</p></body></html>", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Serial Port:", nullptr));
        cmb_com_baud->setItemText(0, QApplication::translate("MainWindow", "9600", nullptr));
        cmb_com_baud->setItemText(1, QApplication::translate("MainWindow", "38400", nullptr));
        cmb_com_baud->setItemText(2, QApplication::translate("MainWindow", "115200", nullptr));

        groupBox_4->setTitle(QApplication::translate("MainWindow", "USB", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "SN/Path:", nullptr));
        cmb_usb_opentype->setItemText(0, QApplication::translate("MainWindow", "None addressed", nullptr));
        cmb_usb_opentype->setItemText(1, QApplication::translate("MainWindow", "SerialNumber", nullptr));

        label_8->setText(QApplication::translate("MainWindow", "Open Type:", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "TCP", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "IP Port:", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "Local IP:", nullptr));
        txt_tcp_ip->setText(QApplication::translate("MainWindow", "192.168.1.222", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "IP Address:", nullptr));
        cmb_tcp_port->setItemText(0, QApplication::translate("MainWindow", "4800:R-PAN", nullptr));
        cmb_tcp_port->setItemText(1, QApplication::translate("MainWindow", "6012:LSGate", nullptr));
        cmb_tcp_port->setItemText(2, QApplication::translate("MainWindow", "9900:RL8000", nullptr));
        cmb_tcp_port->setItemText(3, QApplication::translate("MainWindow", "9909:RD5200", nullptr));

        groupBox->setTitle(QString());
        pushButton_2->setText(QApplication::translate("MainWindow", "Close", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Open", nullptr));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "GroupBox", nullptr));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "GroupBox", nullptr));
        lbl_inventory->setText(QApplication::translate("MainWindow", "Tags: 0  \n"
"Time: 0 ms\n"
"Loop: 0", nullptr));
        btn_inventory_start->setText(QApplication::translate("MainWindow", "Start Inventory", nullptr));
        btn_inventory_stop->setText(QApplication::translate("MainWindow", "Stop Inventory", nullptr));
        btn_inventory_clear->setText(QApplication::translate("MainWindow", "Clear List", nullptr));
        myTabs->setTabText(myTabs->indexOf(tab_inventory), QApplication::translate("MainWindow", "Inventory", nullptr));
        btn_access_write_afi->setText(QApplication::translate("MainWindow", "Write AFI", nullptr));
        txt_access_afi->setText(QApplication::translate("MainWindow", "00", nullptr));
        btn_access_read_block->setText(QApplication::translate("MainWindow", "Read Block", nullptr));
        label_25->setText(QApplication::translate("MainWindow", "AFI:", nullptr));
        label_16->setText(QApplication::translate("MainWindow", "Block Count:", nullptr));
        btn_access_write_block->setText(QApplication::translate("MainWindow", "Write Block", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "Tags:", nullptr));
        btn_access_close_eas->setText(QApplication::translate("MainWindow", "Close EAS", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "New Data:", nullptr));
        label_15->setText(QApplication::translate("MainWindow", "Start Block:", nullptr));
        label_17->setText(QApplication::translate("MainWindow", "Hex", nullptr));
        btn_access_get_eas->setText(QApplication::translate("MainWindow", "Get EAS", nullptr));
        lbl_info->setText(QApplication::translate("MainWindow", "Info", nullptr));
        btn_access_open_eas->setText(QApplication::translate("MainWindow", "Open EAS", nullptr));
        label_24->setText(QApplication::translate("MainWindow", "Hex", nullptr));
        btn_access_get_status->setText(QApplication::translate("MainWindow", "Get Status", nullptr));
        btn_connect->setText(QApplication::translate("MainWindow", "Connect", nullptr));
        btn_Disconnect->setText(QApplication::translate("MainWindow", "Disconnect", nullptr));
        myTabs->setTabText(myTabs->indexOf(tab_access_hf), QApplication::translate("MainWindow", "Access", nullptr));
        lbl_scan_mode->setText(QApplication::translate("MainWindow", "Tags: 0  \n"
"Time: 0 ms\n"
"Loop: 0", nullptr));
        btn_scan_mode_start->setText(QApplication::translate("MainWindow", "Start Scan", nullptr));
        btn_scan_mode_stop->setText(QApplication::translate("MainWindow", "Stop  Scan", nullptr));
        btn_scan_mode_clear->setText(QApplication::translate("MainWindow", "Clear List", nullptr));
        myTabs->setTabText(myTabs->indexOf(tab_scan_mode), QApplication::translate("MainWindow", "ScanMode", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
