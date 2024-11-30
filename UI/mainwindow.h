#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "UI/qcustomplot.h"

#include <QStyleOption>
#include <QPushButton>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QIcon>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

#include <functional> // For std::function
#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>
#include "./qcustomplot.h"
#include "../script.h"
#include "UI/enterface.h"
#include "UI/Login.h"
#include "facedbwidget.h"
#include "configure.h"

class Yolo;
class FaceNet;
class FaceDatabase;
class enterface;
class Login;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
    
    void StyleSheetInit();
    QPixmap getRoundRectPixmap(QPixmap srcPixMap, const QSize &size, int radius);
    QPointF splineSmooth(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, double t);
    void calculateControlPoints(const QVector<QPointF> &knots, QVector<QPointF> *firstControlPoints, QVector<QPointF> *secondControlPoints);
    void calculateFirstControlPoints(double *&result, const double *rhs, int n);
    QVector<QPointF> randomNumbers(int n, int min, int max);

	Ui::MainWindow *ui;
    

private slots:
    // 重绘窗口
    virtual void paintEvent(QPaintEvent *event) override; 
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    // 右上窗口按键
    void on_but_max_clicked();
    void on_but_min_clicked();
    void on_but_off_clicked();
    // 左侧边框按键
    void on_but_user_clicked();
    void on_but_home_clicked(); 
    void on_but_camera_clicked();
    void on_but_data_clicked();
    void on_but_set_clicked();
    void on_login_signal(std::string account);// 接收登录信号 signals:void loginSignal(std::string account);
    
    // page0 首页统计图
    void setBarChart();// 柱状图
    void setBarChart_2();
    void setProgressBar(double percentage);// 原型进度条
    void setPieChart();// 饼状图
    void setLineChart();// 折线图
    void setSplineChart();// 曲线图

    void on_pushButton_clicked();

    // page1 摄像头
    void on_but_stop_clicked();
    void on_but_save_clicked();
    void on_but_open_clicked();
    void on_but_history_clicked();
    void readSaveImages(); 

    // page3 设置页面
	void on_but_sure_clicked();
	void on_but_onnx_clicked();
    void on_but_facenetonnx_clicked();
	void on_but_storagefile_clicked();
    void on_but_enterface_clicked();
    void on_but_message_clicked();

private:
    bool m_leftMousePressed = false; //鼠标是否点中标题栏，是true，否false
    QPoint m_StartPoint ; // 窗口的全局位置
    
    QString but_on = QString("QPushButton{background-color: rgb(62, 69, 176);}");// 打开
    QString but_off = QString("QPushButton{background-color: rgba(0, 0, 0, 0);}");// 未打开
    
    // 自定义类变量
    DataDBInfo dbInfo; // 数据库信息
    Configure *conf; // 配置数据库悬浮窗
    Yolo *yolo = new Yolo(nullptr); // yolo对象
    FaceNet *facenet = new FaceNet(); // facenet对象
    FaceDatabase *database ; // 人脸数据库对象
	Script *callback = nullptr;
    enterface *ui_enterface = nullptr;
    Login *ui_login = nullptr;


    // 保存上一次打开的路径
    void MainWindow::saveLastPath(const QString& savePath, const QString& path) {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        QFile lastPathFile(configPath + savePath);
        // qDebug() << lastPathFile;
        if (lastPathFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&lastPathFile);
            out << path;
            lastPathFile.close();
        } else {
            qDebug() << "Unable to open "<<configPath << savePath<<" for writing.";
        }
    }

    // 读取上一次打开的路径
    QString MainWindow::loadLastPath(const QString& readPath) {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        QFile lastPathFile(configPath + readPath);
        qDebug() << lastPathFile;

        if (lastPathFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&lastPathFile);
            QString path = in.readAll();
            lastPathFile.close();
            return path;
        } else {
            qDebug() << "Unable to open last_path.txt for reading.";
            return QString();
        }
    }
};
#endif // MAINWINDOW_H
