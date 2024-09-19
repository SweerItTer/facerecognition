/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include <qchartview.h>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer;
    QPushButton *but_user;
    QPushButton *but_home;
    QPushButton *but_camera;
    QPushButton *but_data;
    QPushButton *but_set;
    QWidget *widget_2;
    QGridLayout *gridLayout_6;
    QPushButton *but_off;
    QPushButton *but_max;
    QSpacerItem *horizontalSpacer;
    QPushButton *but_min;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QGridLayout *gridLayout_5;
    QLabel *lb_cameramin;
    QWidget *widget_calendar;
    QGridLayout *gridLayout_13;
    QWidget *widget_4;
    QWidget *widget_rgbar;
    QGridLayout *gridLayout_11;
    QLabel *label_9;
    QLabel *label_10;
    QPushButton *pushButton;
    QLabel *label_8;
    QLabel *lb_roundprogessbar;
    QWidget *widget_spline;
    QGridLayout *gridLayout_7;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *box_spline;
    QLabel *label_5;
    QCustomPlot *chart_spline;
    QWidget *widget_pie;
    QGridLayout *gridLayout_10;
    QChartView *chart_pie;
    QGridLayout *gridLayout_12;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *horizontalSpacer_4;
    QLabel *lb_piedata;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *verticalSpacer_5;
    QComboBox *box_pie;
    QLabel *label_7;
    QWidget *widget_bar;
    QGridLayout *gridLayout_8;
    QCustomPlot *chart_bar;
    QWidget *widget_bar2;
    QGridLayout *gridLayout_9;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_3;
    QCustomPlot *chart_bar2;
    QWidget *page_2;
    QGridLayout *gridLayout_3;
    QLabel *lb_cameramessage;
    QLabel *lb_camera;
    QWidget *page_3;
    QWidget *page_4;
    QGridLayout *gridLayout_4;
    QPushButton *but_cancel;
    QPushButton *but_storagefile;
    QLabel *lb_rtsp;
    QLineEdit *le_storagefile;
    QPushButton *but_sure;
    QLabel *bl_onnx;
    QPushButton *but_rtsp;
    QLabel *lb_storagefile;
    QSpacerItem *verticalSpacer_3;
    QLineEdit *le_onnx;
    QTextEdit *te_onnxmessage;
    QPushButton *but_onnx;
    QLineEdit *le_rtsp;
    QSpacerItem *verticalSpacer_6;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1103, 786);
        MainWindow->setWindowTitle(QString::fromUtf8("MainWindow"));
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        MainWindow->setAnimated(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(1, 0, 0, 0);
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMaximumSize(QSize(60, 16777215));
        widget->setStyleSheet(QString::fromUtf8(""));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(0);
        gridLayout_2->setVerticalSpacing(25);
        gridLayout_2->setContentsMargins(0, 20, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout_2->addItem(verticalSpacer_2, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 6, 0, 1, 1);

        but_user = new QPushButton(widget);
        but_user->setObjectName(QString::fromUtf8("but_user"));
        but_user->setStyleSheet(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/user.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_user->setIcon(icon);
        but_user->setIconSize(QSize(30, 30));

        gridLayout_2->addWidget(but_user, 0, 0, 1, 1);

        but_home = new QPushButton(widget);
        but_home->setObjectName(QString::fromUtf8("but_home"));
        but_home->setMinimumSize(QSize(0, 0));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/home.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_home->setIcon(icon1);
        but_home->setIconSize(QSize(30, 30));

        gridLayout_2->addWidget(but_home, 2, 0, 1, 1);

        but_camera = new QPushButton(widget);
        but_camera->setObjectName(QString::fromUtf8("but_camera"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/camare.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_camera->setIcon(icon2);
        but_camera->setIconSize(QSize(30, 30));

        gridLayout_2->addWidget(but_camera, 3, 0, 1, 1);

        but_data = new QPushButton(widget);
        but_data->setObjectName(QString::fromUtf8("but_data"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/data.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_data->setIcon(icon3);
        but_data->setIconSize(QSize(30, 30));

        gridLayout_2->addWidget(but_data, 4, 0, 1, 1);

        but_set = new QPushButton(widget);
        but_set->setObjectName(QString::fromUtf8("but_set"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/set.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_set->setIcon(icon4);
        but_set->setIconSize(QSize(30, 30));

        gridLayout_2->addWidget(but_set, 5, 0, 1, 1);


        gridLayout->addWidget(widget, 1, 1, 2, 1);

        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMinimumSize(QSize(0, 0));
        widget_2->setMaximumSize(QSize(16777215, 30));
        widget_2->setStyleSheet(QString::fromUtf8(""));
        gridLayout_6 = new QGridLayout(widget_2);
        gridLayout_6->setSpacing(0);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        but_off = new QPushButton(widget_2);
        but_off->setObjectName(QString::fromUtf8("but_off"));
        but_off->setMinimumSize(QSize(70, 30));
        but_off->setMaximumSize(QSize(70, 30));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/off.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_off->setIcon(icon5);

        gridLayout_6->addWidget(but_off, 1, 6, 1, 1);

        but_max = new QPushButton(widget_2);
        but_max->setObjectName(QString::fromUtf8("but_max"));
        but_max->setMinimumSize(QSize(70, 30));
        but_max->setMaximumSize(QSize(70, 30));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/max.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_max->setIcon(icon6);

        gridLayout_6->addWidget(but_max, 1, 5, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer, 1, 3, 1, 1);

        but_min = new QPushButton(widget_2);
        but_min->setObjectName(QString::fromUtf8("but_min"));
        but_min->setMinimumSize(QSize(70, 30));
        but_min->setMaximumSize(QSize(70, 30));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/min.png"), QSize(), QIcon::Normal, QIcon::Off);
        but_min->setIcon(icon7);

        gridLayout_6->addWidget(but_min, 1, 4, 1, 1);


        gridLayout->addWidget(widget_2, 0, 1, 1, 3);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setStyleSheet(QString::fromUtf8(""));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        gridLayout_5 = new QGridLayout(page);
        gridLayout_5->setSpacing(10);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        lb_cameramin = new QLabel(page);
        lb_cameramin->setObjectName(QString::fromUtf8("lb_cameramin"));
        lb_cameramin->setMinimumSize(QSize(500, 300));
        lb_cameramin->setMaximumSize(QSize(16777215, 16777215));
        lb_cameramin->setStyleSheet(QString::fromUtf8("background-color: rgb(253, 224, 255);"));

        gridLayout_5->addWidget(lb_cameramin, 0, 0, 2, 2);

        widget_calendar = new QWidget(page);
        widget_calendar->setObjectName(QString::fromUtf8("widget_calendar"));
        widget_calendar->setMinimumSize(QSize(0, 200));
        widget_calendar->setStyleSheet(QString::fromUtf8("background-color: rgb(222, 255, 206);"));
        gridLayout_13 = new QGridLayout(widget_calendar);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));

        gridLayout_5->addWidget(widget_calendar, 0, 2, 1, 1);

        widget_4 = new QWidget(page);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        widget_4->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 170, 170);"));

        gridLayout_5->addWidget(widget_4, 0, 3, 3, 1);

        widget_rgbar = new QWidget(page);
        widget_rgbar->setObjectName(QString::fromUtf8("widget_rgbar"));
        widget_rgbar->setMaximumSize(QSize(16777215, 16777215));
        widget_rgbar->setStyleSheet(QString::fromUtf8("background-color: rgb(188, 219, 255);"));
        gridLayout_11 = new QGridLayout(widget_rgbar);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        label_9 = new QLabel(widget_rgbar);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_11->addWidget(label_9, 1, 0, 1, 1);

        label_10 = new QLabel(widget_rgbar);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_11->addWidget(label_10, 3, 0, 1, 1);

        pushButton = new QPushButton(widget_rgbar);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(0, 40));

        gridLayout_11->addWidget(pushButton, 4, 0, 1, 1);

        label_8 = new QLabel(widget_rgbar);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_11->addWidget(label_8, 0, 0, 1, 1);

        lb_roundprogessbar = new QLabel(widget_rgbar);
        lb_roundprogessbar->setObjectName(QString::fromUtf8("lb_roundprogessbar"));
        lb_roundprogessbar->setMinimumSize(QSize(200, 200));
        lb_roundprogessbar->setMaximumSize(QSize(16777215, 16777215));
        lb_roundprogessbar->setStyleSheet(QString::fromUtf8(""));

        gridLayout_11->addWidget(lb_roundprogessbar, 2, 0, 1, 1);


        gridLayout_5->addWidget(widget_rgbar, 1, 2, 3, 1);

        widget_spline = new QWidget(page);
        widget_spline->setObjectName(QString::fromUtf8("widget_spline"));
        widget_spline->setMinimumSize(QSize(0, 200));
        widget_spline->setMaximumSize(QSize(16777215, 16777215));
        widget_spline->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 203, 229);"));
        gridLayout_7 = new QGridLayout(widget_spline);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_7->setHorizontalSpacing(6);
        gridLayout_7->setVerticalSpacing(5);
        gridLayout_7->setContentsMargins(10, 5, 10, 5);
        horizontalSpacer_2 = new QSpacerItem(317, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_2, 0, 1, 1, 1);

        box_spline = new QComboBox(widget_spline);
        box_spline->addItem(QString());
        box_spline->addItem(QString());
        box_spline->addItem(QString());
        box_spline->setObjectName(QString::fromUtf8("box_spline"));
        box_spline->setMinimumSize(QSize(100, 30));
        box_spline->setIconSize(QSize(16, 16));

        gridLayout_7->addWidget(box_spline, 0, 2, 1, 1);

        label_5 = new QLabel(widget_spline);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMaximumSize(QSize(16777215, 20));

        gridLayout_7->addWidget(label_5, 0, 0, 1, 1);

        chart_spline = new QCustomPlot(widget_spline);
        chart_spline->setObjectName(QString::fromUtf8("chart_spline"));
        chart_spline->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 223, 158);"));

        gridLayout_7->addWidget(chart_spline, 1, 0, 1, 3);


        gridLayout_5->addWidget(widget_spline, 2, 0, 2, 2);

        widget_pie = new QWidget(page);
        widget_pie->setObjectName(QString::fromUtf8("widget_pie"));
        widget_pie->setStyleSheet(QString::fromUtf8("background-color: rgb(213, 255, 247);"));
        gridLayout_10 = new QGridLayout(widget_pie);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        chart_pie = new QChartView(widget_pie);
        chart_pie->setObjectName(QString::fromUtf8("chart_pie"));
        chart_pie->setStyleSheet(QString::fromUtf8(""));
        gridLayout_12 = new QGridLayout(chart_pie);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        gridLayout_12->setContentsMargins(9, 9, 9, 9);
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_12->addItem(verticalSpacer_4, 0, 1, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(49, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_12->addItem(horizontalSpacer_4, 1, 0, 1, 1);

        lb_piedata = new QLabel(chart_pie);
        lb_piedata->setObjectName(QString::fromUtf8("lb_piedata"));
        lb_piedata->setMinimumSize(QSize(100, 100));

        gridLayout_12->addWidget(lb_piedata, 1, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(48, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_12->addItem(horizontalSpacer_5, 1, 2, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_12->addItem(verticalSpacer_5, 2, 1, 1, 1);


        gridLayout_10->addWidget(chart_pie, 1, 0, 1, 2);

        box_pie = new QComboBox(widget_pie);
        box_pie->addItem(QString());
        box_pie->addItem(QString());
        box_pie->setObjectName(QString::fromUtf8("box_pie"));
        box_pie->setMinimumSize(QSize(100, 30));

        gridLayout_10->addWidget(box_pie, 0, 1, 1, 1);

        label_7 = new QLabel(widget_pie);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_10->addWidget(label_7, 0, 0, 1, 1);


        gridLayout_5->addWidget(widget_pie, 3, 3, 2, 1);

        widget_bar = new QWidget(page);
        widget_bar->setObjectName(QString::fromUtf8("widget_bar"));
        widget_bar->setMinimumSize(QSize(378, 200));
        widget_bar->setMaximumSize(QSize(400, 16777215));
        widget_bar->setStyleSheet(QString::fromUtf8("background-color: rgb(220, 180, 255);"));
        gridLayout_8 = new QGridLayout(widget_bar);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        chart_bar = new QCustomPlot(widget_bar);
        chart_bar->setObjectName(QString::fromUtf8("chart_bar"));
        chart_bar->setMaximumSize(QSize(16777215, 16777215));
        chart_bar->setStyleSheet(QString::fromUtf8("background-color: rgb(207, 255, 169);"));

        gridLayout_8->addWidget(chart_bar, 0, 0, 1, 1);


        gridLayout_5->addWidget(widget_bar, 4, 0, 1, 1);

        widget_bar2 = new QWidget(page);
        widget_bar2->setObjectName(QString::fromUtf8("widget_bar2"));
        widget_bar2->setMinimumSize(QSize(300, 0));
        widget_bar2->setStyleSheet(QString::fromUtf8("background-color: rgb(176, 201, 255);"));
        gridLayout_9 = new QGridLayout(widget_bar2);
        gridLayout_9->setSpacing(0);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_9->setContentsMargins(9, 9, 9, 9);
        label_6 = new QLabel(widget_bar2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMaximumSize(QSize(16777215, 15));

        gridLayout_9->addWidget(label_6, 0, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 15, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_3, 0, 1, 1, 1);

        chart_bar2 = new QCustomPlot(widget_bar2);
        chart_bar2->setObjectName(QString::fromUtf8("chart_bar2"));
        chart_bar2->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 160, 162);"));

        gridLayout_9->addWidget(chart_bar2, 1, 0, 1, 2);


        gridLayout_5->addWidget(widget_bar2, 4, 1, 1, 2);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        gridLayout_3 = new QGridLayout(page_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        lb_cameramessage = new QLabel(page_2);
        lb_cameramessage->setObjectName(QString::fromUtf8("lb_cameramessage"));
        lb_cameramessage->setMaximumSize(QSize(200, 16777215));
        lb_cameramessage->setStyleSheet(QString::fromUtf8("background-color: rgb(220, 227, 255);"));

        gridLayout_3->addWidget(lb_cameramessage, 0, 2, 1, 1);

        lb_camera = new QLabel(page_2);
        lb_camera->setObjectName(QString::fromUtf8("lb_camera"));
        lb_camera->setMinimumSize(QSize(600, 600));
        lb_camera->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 232, 172);"));

        gridLayout_3->addWidget(lb_camera, 0, 1, 1, 1);

        stackedWidget->addWidget(page_2);
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        stackedWidget->addWidget(page_3);
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        gridLayout_4 = new QGridLayout(page_4);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        but_cancel = new QPushButton(page_4);
        but_cancel->setObjectName(QString::fromUtf8("but_cancel"));

        gridLayout_4->addWidget(but_cancel, 7, 4, 1, 1);

        but_storagefile = new QPushButton(page_4);
        but_storagefile->setObjectName(QString::fromUtf8("but_storagefile"));

        gridLayout_4->addWidget(but_storagefile, 4, 4, 1, 1);

        lb_rtsp = new QLabel(page_4);
        lb_rtsp->setObjectName(QString::fromUtf8("lb_rtsp"));

        gridLayout_4->addWidget(lb_rtsp, 1, 0, 1, 1);

        le_storagefile = new QLineEdit(page_4);
        le_storagefile->setObjectName(QString::fromUtf8("le_storagefile"));

        gridLayout_4->addWidget(le_storagefile, 4, 2, 1, 2);

        but_sure = new QPushButton(page_4);
        but_sure->setObjectName(QString::fromUtf8("but_sure"));

        gridLayout_4->addWidget(but_sure, 7, 3, 1, 1);

        bl_onnx = new QLabel(page_4);
        bl_onnx->setObjectName(QString::fromUtf8("bl_onnx"));

        gridLayout_4->addWidget(bl_onnx, 2, 0, 1, 1);

        but_rtsp = new QPushButton(page_4);
        but_rtsp->setObjectName(QString::fromUtf8("but_rtsp"));

        gridLayout_4->addWidget(but_rtsp, 1, 4, 1, 1);

        lb_storagefile = new QLabel(page_4);
        lb_storagefile->setObjectName(QString::fromUtf8("lb_storagefile"));

        gridLayout_4->addWidget(lb_storagefile, 4, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_3, 5, 0, 1, 1);

        le_onnx = new QLineEdit(page_4);
        le_onnx->setObjectName(QString::fromUtf8("le_onnx"));

        gridLayout_4->addWidget(le_onnx, 2, 2, 1, 2);

        te_onnxmessage = new QTextEdit(page_4);
        te_onnxmessage->setObjectName(QString::fromUtf8("te_onnxmessage"));
        te_onnxmessage->setMaximumSize(QSize(16777215, 200));

        gridLayout_4->addWidget(te_onnxmessage, 3, 2, 1, 2);

        but_onnx = new QPushButton(page_4);
        but_onnx->setObjectName(QString::fromUtf8("but_onnx"));

        gridLayout_4->addWidget(but_onnx, 2, 4, 1, 1);

        le_rtsp = new QLineEdit(page_4);
        le_rtsp->setObjectName(QString::fromUtf8("le_rtsp"));
        le_rtsp->setMinimumSize(QSize(700, 0));

        gridLayout_4->addWidget(le_rtsp, 1, 2, 1, 2);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_6, 0, 0, 1, 1);

        stackedWidget->addWidget(page_4);

        gridLayout->addWidget(stackedWidget, 1, 3, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        but_user->setText(QString());
        but_home->setText(QString());
        but_camera->setText(QString());
        but_data->setText(QString());
        but_set->setText(QString());
        but_off->setText(QString());
        but_max->setText(QString());
        but_min->setText(QString());
        lb_cameramin->setText(QApplication::translate("MainWindow", "\346\221\204\345\203\217\346\234\272 \345\260\217\345\261\225\347\244\272", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Duuuuuuuuuuuuuuuuuu", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "llllllllllllllllllllllllllll", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Suuuuuuuu", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "Lllllllllll", nullptr));
        lb_roundprogessbar->setText(QApplication::translate("MainWindow", "lb_show", nullptr));
        box_spline->setItemText(0, QApplication::translate("MainWindow", "All time", nullptr));
        box_spline->setItemText(1, QApplication::translate("MainWindow", "2", nullptr));
        box_spline->setItemText(2, QApplication::translate("MainWindow", "3", nullptr));

        label_5->setText(QApplication::translate("MainWindow", "\346\233\262\347\272\277\345\233\276", nullptr));
        lb_piedata->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        box_pie->setItemText(0, QApplication::translate("MainWindow", "mon", nullptr));
        box_pie->setItemText(1, QApplication::translate("MainWindow", "sun'", nullptr));

        label_7->setText(QApplication::translate("MainWindow", "Cccccccc", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Aaaaaaa", nullptr));
        lb_cameramessage->setText(QApplication::translate("MainWindow", "\344\277\241\346\201\257\346\230\276\347\244\272", nullptr));
        lb_camera->setText(QApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264", nullptr));
        but_cancel->setText(QApplication::translate("MainWindow", "\345\217\226\346\266\210", nullptr));
        but_storagefile->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266\345\244\271", nullptr));
        lb_rtsp->setText(QApplication::translate("MainWindow", "RTSP\357\274\232", nullptr));
        but_sure->setText(QApplication::translate("MainWindow", "\347\241\256\350\256\244", nullptr));
        bl_onnx->setText(QApplication::translate("MainWindow", "\350\257\206\345\210\253\346\250\241\345\236\213\346\226\207\344\273\266\357\274\232", nullptr));
        but_rtsp->setText(QApplication::translate("MainWindow", "\344\277\256\346\224\271", nullptr));
        lb_storagefile->setText(QApplication::translate("MainWindow", "\345\275\225\345\205\245\344\277\241\346\201\257\345\255\230\345\202\250\344\275\215\347\275\256\357\274\232", nullptr));
		le_onnx->setText(QApplication::translate("MainWindow", "D:\\Program\\project\\gitee\\facerecognition\\Yolov8\\sourse\\best.onnx", nullptr));
        te_onnxmessage->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:7.2pt;\">\346\211\223\345\215\260\346\250\241\345\236\213\347\232\204\347\233\270\345\205\263\344\277\241\346\201\257</span></p></body></html>", nullptr));
        but_onnx->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\250\241\345\236\213", nullptr));
        le_rtsp->setText(QApplication::translate("MainWindow", "rtsp://admin:15935746jk@Liu@172.18.107.147:554/h264/ch1/main/av_stream", nullptr));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
