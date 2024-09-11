#ifndef YOLO_H
#define YOLO_H

//#include "ui_mainwindow.h"

#include <QObject>
#include <QDebug>
#include <QPointF>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>

#include <assert.h>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>


#include <onnxruntime_cxx_api.h>
#include <onnxruntime_run_options_config_keys.h>
#include <onnxruntime_session_options_config_keys.h>

#pragma execution_character_set("UTF-8")


typedef struct _DCSP_RESULT {
    int classId; // 类别ID
    float confidence;// 置信度
    cv::Rect box;// 盒子（矩形区域）
} DCSP_RESULT;

class Yolo : public QObject
{
    Q_OBJECT
public:

    explicit Yolo(QObject *parent = nullptr);
    ~Yolo();
    bool loadModel(QString onnxfile);//检测模型
    void runModel(QString o,QString f ,cv::Mat m,QString type);//运行模型
    void stopModel(QString o, QString f ,cv::Mat m,QString type);//停止检测
    void updateFrame();
    void Capture (QString c);

private:

    void printInputModel(Ort::Session *session);//打印模型输入信息
    void printOutputModel(Ort::Session *session);//打印模型输出信息
    void predictImage(Ort::Session *session, cv::Mat m);//预测图片
    void normalized(cv::Mat input_tensor, std::vector<float> &output_data);//图片归一化
    cv::Mat PreprocessImage(cv::Mat m);
    cv::Mat sessionRun(Ort::Session *session,cv::Mat final_mat,cv::Mat mat);


    size_t num_input_nodes; //输入节点数量
    std::vector<const char*> input_node_names;//输入节点的名字 []={"images","",""}
    std::vector<int64_t> input_node_dims;//输入节点的信息 .size()=4 []={1,3,640,640}
    size_t input_tensor_size;//输入节点的数值相乘 1*3*640*640
    size_t num_output_nodes;//输出节点数量
    std::vector<const char*> output_node_names;//输出节点名字 []={"output0","",""}
    std::vector<int64_t> output_node_dims;//输出节点信息{1,84,8400}
    int output_tensor_size;//输出节点的数值相乘 1*84*8400


    cv::dnn::Net net;
    QTimer *timer;
    QString filetype;
    QString onnxpath;//模型路径
    cv::VideoCapture *capture;
    bool seeion_run_over ;
    float aspect_mat;
    int fixed_w ;
    int fixed_h ;


signals:
    void signal_str(QString str);//发给mainwidget的信号
    void signal_mat(cv::Mat mat);
    void frame_mat(cv::Mat mat);

};

#endif // YOLO_H
