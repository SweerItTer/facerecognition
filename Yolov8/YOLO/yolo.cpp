﻿#include "yolo.h"
#pragma execution_character_set("UTF-8")

#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"


Yolo::Yolo(QObject *parent)
    :QObject(parent)
    , capture(new cv::VideoCapture)
{

    timer = new QTimer ;
    connect(timer,&QTimer::timeout,this,&Yolo::updateFrame);
}
Yolo::~Yolo()
{
    delete capture;
}



// 加载ONNX模型文件
bool Yolo::loadModel(QString filename)
{
    if(!QFile::exists(filename)){// 检查文件是否存在
       emit signal_str("**********未选中模型**********");
        return false; // 如果文件不存在，则直接返回
    }else{
        MYLOG<<"加载ONNX模型-开始";
        Ort::Env env;
//        const ORTCHAR_T* onnxfile = filename.toStdWString().c_str();
        Ort::Session session(env,filename.toStdWString().c_str(),Ort::SessionOptions{nullptr});
        MYLOG<<"创建会话-成功";

        // 检查是否有可用的CUDA设备（即检查是否可以使用GPU进行加速）
        int deviceID = cv::cuda::getCudaEnabledDeviceCount();
        MYLOG<<"可用的CUDA数："<<deviceID;
        if(deviceID == 1)
        {
            // 如果有可用的CUDA设备，将网络的推理后端设置为CUDA以使用GPU
            this->net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            this->net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
            emit signal_str("将使用CUDA推理\n");
        }
        else
            emit signal_str("未检测到可用GPU，将使用CPU推理！");

        emit signal_str("**********模型信息**********");
        printInputModel(&session);
        printOutputModel(&session);
        MYLOG<<"模型加载成功，返回true\n";
        return true; // 模型加载成功，返回true
    }
}

// 运行模型
void Yolo::runModel(QString o, QString f ,cv::Mat m,QString type)
{

	//  为什么在运行了 loadModel(onnxfile) 后还要在这里出现加载一次?
    if(!QFile::exists(o)){// 检查模型是否存在
       emit signal_str("**********未选中模型**********");
        return; // 如果文件不存在，则直接返回
    }else{
        MYLOG<<"开始运行模型";
        filetype = type;//文件类型
        onnxpath = o;//模型路径
        Ort::Env env;
        Ort::Session session(env,onnxpath.toStdWString().c_str(),Ort::SessionOptions{nullptr});

        if(filetype == "image"){
            MYLOG<<"对图像进行识别";
            cv::Mat final_mat = PreprocessImage(m);
            cv::Mat mat = sessionRun(&session,final_mat,m);
            emit signal_mat(mat);

        }else if(filetype == "video"){
            MYLOG<<"对视频进行识别";
//            double frameRate = capture->get(cv::CAP_PROP_FPS);
//            timer->start(1000/frameRate); // 根据帧率开始播放
            timer->start(100);
        }else{
            MYLOG<<"对摄像头进行识别";
            capture->open(0);
            timer->start(100);
        }
    }
}

//停止检测
void Yolo::stopModel(QString o, QString f ,cv::Mat m,QString type)
{
    if(filetype == "video"){
        MYLOG<<"对视频进行识别";
        timer->stop();
        cv::Mat mat;
        capture->read(mat);
        emit signal_mat(mat);
        capture->release();
    }else{
        MYLOG<<"对摄像头进行识别";
        timer->stop();
        capture->release();
    }
}

// 打印模型输入信息
void Yolo::printInputModel(Ort::Session *session)
{
    Ort::AllocatorWithDefaultOptions allocator;
    //打印模型输入节点的数量
    num_input_nodes = session->GetInputCount();
    emit signal_str(QString("输入节点数量 = %1").arg(num_input_nodes));
    input_node_names.resize(num_input_nodes);//节点名字分配空间
    // 遍历所有输入节点
    for (int i = 0; i < num_input_nodes; i++) {
        // 打印输入节点名称
		//--------------------
		char* input_name = session->GetInputName(i, allocator);
		//--------------------
//        char* input_name = session->GetInputNameAllocated(i, allocator).get();
        emit signal_str(QString("第 %1 个: name=%2").arg(i).arg(input_name));
        input_node_names[i] = input_name;

        // 打印输入节点类型
		Ort::TypeInfo type_info = session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        ONNXTensorElementDataType type = tensor_info.GetElementType();
        emit signal_str(QString(": type=%1").arg(type));

        // 打印输入 shapes/dims
        input_node_dims = tensor_info.GetShape();
        emit signal_str(QString(": num_dims=%1").arg(input_node_dims.size()));
        for (int j = 0; j < input_node_dims.size(); j++)
            emit signal_str(QString(": dim %1=%2 ").arg(j).arg(input_node_dims[j]));
    }
}

// 打印模型输出信息
void Yolo::printOutputModel(Ort::Session *session)
{
    Ort::AllocatorWithDefaultOptions allocator;
    //打印模型输出节点的数量
    num_output_nodes = session->GetOutputCount();
    output_node_names.resize(num_output_nodes);
    emit signal_str(QString("\n输出节点数量 = %1").arg(num_output_nodes));
    // 遍历所有输出节点
    for (int i = 0; i < num_output_nodes; i++) {
        // 打印输出节点名称
		//---------------
		char* output_name = session->GetOutputName(i, allocator);
		//---------------
//        char* output_name = session->GetOutputNameAllocated(i, allocator).get();
        emit signal_str(QString("第 %1 个: name=%2").arg(i).arg(output_name));
        output_node_names[i] = output_name;

        // 打印输出节点类型
        Ort::TypeInfo type_info = session->GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        ONNXTensorElementDataType type = tensor_info.GetElementType();
        emit signal_str(QString(": type=%1").arg(type));

        // 打印输出 shapes/dims
        output_node_dims = tensor_info.GetShape();
        emit signal_str(QString(": num_dims=%1").arg(output_node_dims.size()));
        for (int j = 0; j < output_node_dims.size(); j++)
            emit signal_str(QString(": dim %1=%2").arg(j).arg(output_node_dims[j]));
    }
}

//预处理图片
cv::Mat Yolo::PreprocessImage(cv::Mat m)
{
//    MYLOG<<"预处理图片-开始";
    //调整输入图片尺寸
    if(m.empty()) return m;
    cv::Mat mat = m ;
    float aspect_ratio = static_cast<float>(mat.size().width) / static_cast<float>(mat.size().height);
    if(mat.size().width>mat.size().height)
    {
        fixed_w = input_node_dims[2];
        fixed_h = input_node_dims[3]/aspect_ratio;
    }else{
        fixed_w = input_node_dims[2]*aspect_ratio;
        fixed_h = input_node_dims[3];
    }
    cv::Mat resize_mat;
    cv::resize(mat,resize_mat,cv::Size(fixed_w,fixed_h));
    //设置黑色背景
    aspect_mat =static_cast<float>(mat.size().width) / static_cast<float>(resize_mat.size().width);
    cv::Mat final_mat(input_node_dims[2], input_node_dims[3], resize_mat.type(), cv::Scalar(0, 0, 0));
    cv::Rect roi((input_node_dims[2] - fixed_w) / 2, (input_node_dims[3] - fixed_h) / 2, fixed_w, fixed_h);
    resize_mat.copyTo(final_mat(roi));
//    MYLOG << "预处理图片-结束";
    return final_mat;
}

//运行模型，画框
cv::Mat Yolo::sessionRun(Ort::Session *session,cv::Mat final_mat,cv::Mat mat)
{
    MYLOG << "运行模型-开始";
    // 从数据值创建输入张量对象
    //输入节点的数值相乘 1*3*640*640
    input_tensor_size = input_node_dims[0]*input_node_dims[1]*
                        input_node_dims[2]*input_node_dims[3];

    std::vector<float> input_data(input_tensor_size);
    normalized(final_mat, input_data);//图片归一化
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info,
                                                              input_data.data(),
                                                              input_tensor_size,
                                                              input_node_dims.data(),//输入节点的数值{1，3，640，640}
                                                              input_node_dims.size());//输入节点的大小：4

    std::vector<Ort::Value> ort_inputs;
    ort_inputs.push_back(std::move(input_tensor));

    // 设计输入和输出的名字
    std::vector<const char*> input_node_names = {"images"};
    std::vector<const char*> output_node_names = {"output0"};
//    MYLOG <<input_node_names.data();// 输入节点名称数组 "images"
//    MYLOG <<ort_inputs.data();// 输入张量数组
//    MYLOG <<num_input_nodes;// 输入张量数量 1
//    MYLOG <<output_node_names.data();// 输出节点名称数组 "output0"
//    MYLOG <<num_output_nodes;// 输出张量数量 1


    // 输入图片数据，运行模型获取预测结果
    std::vector<Ort::Value> output_tensors = session->Run(Ort::RunOptions{nullptr}, // 使用默认选项
                                                          input_node_names.data(),  // 输入节点名称数组 "images"
                                                          ort_inputs.data(),        // 输入张量数组
                                                          num_input_nodes,          // 输入张量数量 1
                                                          output_node_names.data(), // 输出节点名称数组 "output0"
                                                          num_output_nodes);        // 输出张量数量 1


    MYLOG << "运行模型-结束";

//    MYLOG << "画框-开始";
    //输出节点的数值相乘 1*84*8400
    output_tensor_size = output_node_dims[0]*output_node_dims[1]*output_node_dims[2];
//    MYLOG << output_tensor_size;
    for(int i = 0; i < output_tensors.size(); i++) {
        Ort::Value *output = &output_tensors[i];
        float *value = output->GetTensorMutableData<float>();//
        std::vector<int64_t> shape = output->GetTypeInfo()//张量的类型信息
                                            .GetTensorTypeAndShapeInfo()//张量的类型和形状信息
                                            .GetShape();//张量的形状
        std::vector<std::vector<QPointF>> boxes;
        int index = 0;
        for(int x=0; x < shape[0]; x++){//获取识别框
            bool should_continue = true;
            for (index = 0; index < output_tensor_size; ++index) { //输出节点的数值相乘 1*84*8400
                if (value[index] > 0 && value[index] < 1) { //识别的分数
                    if (value[index] > 0.6) { //人脸识别的阈值
                        float  strValue = QString::number(value[index], 'f', 2).toFloat(); // 精确到两位小数
                        int id_1 = index/output_node_dims[2];
                        int id_2 = index-(id_1*output_node_dims[2]);
                        int id0 = id_2;
                        int id1 = output_node_dims[2] + id_2;
                        int id2 = 2*output_node_dims[2] + id_2;
                        int id3 = 3*output_node_dims[2] + id_2;
//                            MYLOG<<index<<value[index]<<id_1<<id_2;
//                            MYLOG<<value[id0]<<value[id1]<<value[id2]<<value[id3];
                        std::vector<QPointF> points;
                        points.push_back(QPointF(qRound(value[id0]),qRound(value[id1])));
                        points.push_back(QPointF(qRound(value[id2]),qRound(value[id3])));
                        points.push_back(QPointF(strValue,0));
                        boxes.push_back(points);
                    }
                }
                else should_continue = false;
            }
        }

//        MYLOG<<"获取到的识别框数量："<<boxes.size();
        if(boxes.size()==0) break;

//        MYLOG<<boxes[1];//std::vector(QPointF(277.26,116.166), QPointF(73.5992,102.491))
//        MYLOG<<boxes[1][0];//QPointF(73.5992,102.491)
//        MYLOG<<boxes[1][0].x();//73.5992

//        MYLOG<<"排序";
        for (int i = 0; i < boxes.size() - 1; ++i) {//排序
            for (int j = 0; j < boxes.size() - i - 1; ++j) {
                if (boxes[j][0].x() > boxes[j + 1][0].x()) {
                    qSwap(boxes[j], boxes[j + 1]);
                }
            }
        }
//        MYLOG<<"删除重复框";
        for (int i = 0; i < boxes.size() - 1; ++i) {//删除重复框
            float p1x = boxes[i][0].x() / boxes[i + 1][0].x() ;
            float p1y = boxes[i][0].y() / boxes[i + 1][0].y() ;
            float p2x = boxes[i][1].x() / boxes[i + 1][1].x() ;
            float p2y = boxes[i][1].y() / boxes[i + 1][1].y() ;
            float ppp = p1x+p1y+p2x+p2y;
            if (ppp>3.6&&ppp<=4.5){
                boxes.erase(boxes.begin() + i);
                --i;
            }
        }

//        MYLOG<<"删除重复识别框，剩余数量："<<boxes.size();
        if(boxes.size()==0) break;

//        MYLOG<<"opencv画框";
        for(int i=0 ; i < boxes.size() ; i++){//识别框：中心点，宽高=》左上，右下，删除黑色背景,回归原本大小
            std::vector<QPointF> boxe = boxes[i];
            QPointF point1 = boxe[0];
            QPointF point2 = boxe[1];
            float threshold = boxe[2].x();//人脸识别阈值
            //回归原本大小
            float center_x = point1.x()-(416 - fixed_w) / 2;
            float center_y = point1.y()-(416 - fixed_h) / 2;

            float w = point2.x() * aspect_mat;
            float h = point2.y() * aspect_mat;

            float x_1 = center_x * aspect_mat - w/2;//左下
            float y_1 = center_y * aspect_mat + h/2;
            float x_2 = center_x * aspect_mat + w/2;//右上
            float y_2 = center_y * aspect_mat - h/2;

            float x_3 = center_x * aspect_mat - w/2;//左上
            float y_3 = center_y * aspect_mat - h/2;

//            QString directoryPath = "./imgs_cut";
//            QDir directory(directoryPath);
//            QStringList filters;
//            filters << "*.jpg"; // 只匹配.jpg文件
//            directory.setNameFilters(filters); // 设置过滤器
//            QFileInfoList list = directory.entryInfoList();
//            int jpgCount = list.size();
//            //裁剪图片
//            cv::Mat cut_mat ;
//            cv::cvtColor(mat,cut_mat,cv::COLOR_BGR2RGB);
//            QImage img(cut_mat.data, cut_mat.cols, cut_mat.rows, cut_mat.step, QImage::Format_BGR888);
//            QImage cut_img = img.copy(x_3,y_3,w,h);
//            cut_img.save(QString("./imgs_cut/img_cut_%1.jpg").arg(jpgCount++));

            //人脸识别框
            cv::rectangle(mat,
                          cv::Point(x_1,y_1),//左下
                          cv::Point(x_2,y_2),//右上
                          cv::Scalar(0, 255, 0), //颜色
                          2 * aspect_mat);//粗细
            //文字
            const char* textChar = QString("face %1").arg(threshold).toLocal8Bit().constData();
            cv::putText(mat, textChar,
                        cv::Point(x_1,y_1-h-2 * aspect_mat), // 文字的起始位置
                        cv::FONT_HERSHEY_SIMPLEX,
                        2.0 * aspect_mat/4, // 字体大小
                        cv::Scalar(0, 255, 0), // 字体颜色
                        1 * aspect_mat); // 字体粗细
        }
    }
    MYLOG << "画框-结束\n";
    return mat;
}


void Yolo::predictImage(Ort::Session *session, cv::Mat m)
{

}

void Yolo::normalized(cv::Mat input_tensor, std::vector<float> &output_data)
{
    std::size_t counter = 0;
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < input_tensor.rows; i++)
        {
            for (int j = 0; j < input_tensor.cols; j++)
            {
                output_data[counter++]=static_cast<float>(input_tensor.at<cv::Vec3b>(i, j)[k]) / 255.0;
            }
        }
    }
}

//更新帧率，视频，摄像头
void Yolo::updateFrame()
{
    Ort::Env env;
    Ort::Session session(env,onnxpath.toStdWString().c_str(),Ort::SessionOptions{nullptr});

    cv::Mat mat_f;
    if(filetype=="video"){
        capture->read(mat_f);
//        int frameNumber = static_cast<int>(capture->get(cv::CAP_PROP_POS_FRAMES));
//        MYLOG<<"当前帧："<<frameNumber;
        if(mat_f.data == nullptr) return;
        cv::cvtColor(mat_f,mat_f,cv::COLOR_BGR2RGB);
        cv::Mat final_mat = PreprocessImage(mat_f);
        mat_f = sessionRun(&session,final_mat,mat_f);

    }else if(filetype=="camera"){
        cv::Mat src;
        if(capture->isOpened())
        {
            *capture >> src;
            if(src.data == nullptr) return ;
            //将图像转换为qt能够处理的格式
            cv::cvtColor(src,mat_f,cv::COLOR_BGR2RGB);
            cv::flip(mat_f,mat_f,1);
            cv::Mat final_mat = PreprocessImage(mat_f);
            mat_f = sessionRun(&session,final_mat,mat_f);
            emit signal_mat(mat_f);
        }
    }
    session.release();
    emit signal_mat(mat_f);
}

void Yolo::Capture (QString c)
{
    capture->open(c.toStdString());//用opcv打开视频
    if(!capture->isOpened()){
        emit signal_str("mp4文件打开失败");//mp4文件打开失败
        return;
    }
    //获取视频信息
    long totalFrame = capture->get(cv::CAP_PROP_FRAME_COUNT);//显示信息
    int width = capture->get(cv::CAP_PROP_FRAME_WIDTH);//宽
    int height = capture->get(cv::CAP_PROP_FRAME_HEIGHT);//高
    emit signal_str(QString("整个视频共 %1 帧, 宽=%2 高=%3 ").arg(totalFrame).arg(width).arg(height));

    //设置从视频的开始帧开始
    long frameToStart = 0;
    capture->set(cv::CAP_PROP_POS_FRAMES,frameToStart);
    emit signal_str(QString("从 %1 开始读").arg(frameToStart));

    //获取视频帧数
    double framRate = capture->get(cv::CAP_PROP_FPS);
    emit signal_str(QString("帧率为： %1").arg(framRate));

    //读取视频第一帧
    cv::Mat mat_f;
    capture->read(mat_f);
    cv::cvtColor(mat_f,mat_f,cv::COLOR_BGR2RGB);
    emit signal_mat(mat_f);//将调整后的图片放到标签上
}