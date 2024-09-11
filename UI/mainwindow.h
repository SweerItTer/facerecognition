#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include <opencv2/opencv.hpp>
#include "qcustomplot.h"
#include "../script.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
    QPixmap getRoundRectPixmap(QPixmap srcPixMap, const QSize &size, int radius);
    QPointF splineSmooth(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, double t);
    void calculateControlPoints(const QVector<QPointF> &knots, QVector<QPointF> *firstControlPoints, QVector<QPointF> *secondControlPoints);
    void calculateFirstControlPoints(double *&result, const double *rhs, int n);
    QVector<QPointF> randomNumbers(int n, int min, int max);


private slots:
    virtual void paintEvent(QPaintEvent *event) override; // 重绘窗口
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    void on_but_home_clicked();
    void on_but_camera_clicked();
    void on_but_data_clicked();
    void on_but_set_clicked();

    void setBarChart();// 柱状图
    void setBarChart_2();
    void setProgressBar();// 原型进度条
    void setPieChart();// 饼状图
    void setLineChart();// 折线图
    void setSplineChart();// 曲线图


    void on_but_max_clicked();
    void on_but_min_clicked();
    void on_but_off_clicked();



    void on_pushButton_clicked();

	void on_but_sure_clicked();

private:
    Ui::MainWindow *ui;
    bool m_leftMousePressed = false; //鼠标是否点中标题栏，是true，否false
    QPoint m_StartPoint ; // 窗口的全局位置

    QString but_on = QString("QPushButton{background-color: rgb(62, 69, 176);}");// 打开
    QString but_off = QString("QPushButton{background-color: rgba(0, 0, 0, 0);}");// 未打开






	Script *callback;
};
#endif // MAINWINDOW_H
