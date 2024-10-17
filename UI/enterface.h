#ifndef ENTERFACE_H
#define ENTERFACE_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QString>
#include <QCloseEvent>

#include <vector>

#include "opencv2/opencv.hpp"
#include "./Yolov8/YOLO/yolo.h"

class Yolo;

namespace Ui {
class enterface;
}

class enterface : public QWidget
{
    Q_OBJECT

public:
    explicit enterface(QWidget *parent = nullptr, QString mPath = "", QString FaceMPath = "", Yolo *yolo = nullptr);
    ~enterface();

    void setSession(Ort::Session *&session);
    void setCallback(std::function<void()> callback_){
        callback = callback_;
    }

private:
    Ui::enterface *ui;
    
    QTimer *timer;

    cv::VideoCapture *capture;

    int page = 0;
    QString YoloMPath_;
    QString FaceMPath_;

    // MainWindow *mw = nullptr;
    Yolo *yolo_ = nullptr;
    Ort::Session *session_;


private slots:
    void on_but_data_clicked(); 
    void on_but_face_clicked(); 
    void on_but_finish_clicked(); 
    void on_but_verify_clicked(); 

    void on_but_back_clicked(); // back
    void on_but_next_clicked(); // next

    void StackedWInit(); //堆叠容器初始化

    // 录入人脸相关槽函数
    void on_but_camera_clicked(); // 打开摄像头
    void updateFrame();

    void on_but_panorama_clicked(); // 打开全景图
    void on_but_facecut_clicked(); // 人脸分割
    void on_bur_screenshot_clicked(); // 截图
    void on_but_save_clicked(); // 保存图片
    void on_but_cancel_clicked(); // 取消操作

// --------
private:
    // 关闭事件
    void closeEvent(QCloseEvent *event){
        callback(); // 调用回调函数(继续线程)
        event->accept();
    }

    std::function<void()> callback; // 无返回,无参数

};

#endif // ENTERFACE_H
