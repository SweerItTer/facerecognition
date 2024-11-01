#ifndef ENTERFACE_H
#define ENTERFACE_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QString>
<<<<<<< HEAD
#include <QPainter>
#include <QMouseEvent>
=======
#include <QCloseEvent>
>>>>>>> 474f277b4b09ca6b0e46b4e0db1b2056b3611a5c

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
    explicit enterface(QWidget *parent = nullptr, Yolo *yolo = nullptr);
    ~enterface();
    void InitStyle();

    void setSession(Ort::Session *&session);
    void setCallback(std::function<void()> callback_){
        callback = callback_;
    }

private:
    Ui::enterface *ui;
    bool m_leftMousePressed = false; //鼠标是否点中标题栏，是true，否false
    QPoint m_StartPoint ; // 窗口的全局位置
    QTimer *timer;
    cv::VideoCapture *capture;
    Yolo *yolo_ = nullptr;

    int page = 0;
    int retImgIndex = 0;

    QPixmap save_1;
    QPixmap save_2;
    QPixmap save_3;

    const QPixmap *save_becake;


private slots:
    // 重绘窗口
    virtual void paintEvent(QPaintEvent *event) override; 
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    // 关闭窗口
    void on_but_off_clicked();
    // 引索
    void on_but_data_clicked(); 
    void on_but_face_clicked(); 
    void on_but_finish_clicked(); 
    void on_but_verify_clicked(); 

    void pagechangebutton(); // 页面改变按钮
    // 上一步下一步
    void on_but_back_clicked(); // back
    void on_but_next_clicked(); // next

    // 录入人脸相关槽函数
    void on_but_camera_clicked(); // 打开摄像头
    void updateFrame();

    void on_but_panorama_clicked(); // 打开全景图
    void on_but_facecut_clicked(); // 人脸分割
    void on_but_save_clicked(); // 保存图片
<<<<<<< HEAD

    void on_but_delet1_clicked(); // 删除图片1
    void on_but_delet2_clicked(); // 删除图片2
    void on_but_delet3_clicked(); // 删除图片3
=======
    void on_but_cancel_clicked(); // 取消操作

// --------
private:
    // 关闭事件
    void closeEvent(QCloseEvent *event){
        callback(); // 调用回调函数(继续线程)
        event->accept();
    }

    std::function<void()> callback; // 无返回,无参数
>>>>>>> 474f277b4b09ca6b0e46b4e0db1b2056b3611a5c

    void on_but_end_clicked(); // 结束录入
    void on_but_nextpeople_clicked(); // 录入下一个人
};

#endif // ENTERFACE_H
