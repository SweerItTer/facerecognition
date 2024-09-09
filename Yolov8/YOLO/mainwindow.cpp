

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"
#pragma execution_character_set("UTF-8")
using namespace cv;

/*
	yolo.loadModel(onnxfile); // 加载模型,返回int
	yolo.Capture(filename);// 检测视频,返回void
	yolo.runModel(onnxfile,filename,mat,filetype); //filename为无效参数
	// 在加载模型后又一次加载模型,不懂
	//mat:图像数据, filetype:文件类型
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , capture(new cv::VideoCapture)
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateFrame);
    //接收yolo传来的信号，有关ui的
    connect(&yolo,&Yolo::signal_str,this,&MainWindow::slots_str);
    connect(&yolo,&Yolo::signal_mat,this,&MainWindow::slots_mat);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete capture; // 确保释放 VideoCapture 对象
}

//接收yolo发来的信号
void MainWindow::slots_str(QString str)
{
    ui->te_message->append(str);
}
// 返回的,检测到人脸的图像数据
void MainWindow::slots_mat(cv::Mat m)
{
    QImage img = QImage(m.data, m.cols, m.rows, m.step, QImage::Format_RGB888);
    QPixmap mmp = QPixmap::fromImage(img);
    mmp = mmp.scaled(ui->lb_show->size(),Qt::KeepAspectRatio);
    ui->lb_show->setPixmap(mmp);
}


//打开文件
void MainWindow::on_btn_file_clicked()
{
    filename = QFileDialog::getOpenFileName(this,
                                            QStringLiteral("打开文件"),
                                            "./sourse",
                                            "*.mp4 *.avi *.png *.jpg *.jpeg *.bmp");
    if(!QFile::exists(filename))
    {
        ui->te_message->append("**********未选中文件**********");
        return; 
    }
    ui->te_message->append("**********打开图片**********");
    ui->te_message->append(filename);

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filename);
    ui->te_message->append(QString("打开文件的类型为：%1").arg(mime.name()));
    // 如果文件是图片类型
    if(mime.name().startsWith("image/")){
        filetype ="image";
        // 使用OpenCV读取图片
        cv::Mat src = cv::imread(filename.toStdString());
        // 检查图片是否正确读取，即图片数据是否非空
        if(src.empty()){
            ui->lb_show->setText("图像不存在"); // 如果图片不存在，显示错误消息
            return;
        }
        cv::Mat temp;
        if(src.channels() == 4){
            cv::cvtColor(src, temp, cv::COLOR_BGRA2RGB);
        }else if(src.channels() == 3){
            cv::cvtColor(src, temp, cv::COLOR_BGR2RGB);
        }else{
            cv::cvtColor(src, temp, cv::COLOR_GRAY2RGB);
        }
        mat = temp;
        QImage img = QImage(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        QPixmap mmp = QPixmap::fromImage(img);
        mmp = mmp.scaled(ui->lb_show->size(),Qt::KeepAspectRatio);
        ui->lb_show->setPixmap(mmp);
        ui->te_message->append("*********图片加载成功*********\n");
        return;
    }
    //如果打开的是视频
    if(mime.name().startsWith("video/")){
        filetype ="video";
        capture->open(filename.toStdString());
        long frameToStart = 0;
        capture->set(cv::CAP_PROP_POS_FRAMES,frameToStart);
		yolo.Capture(filename);// 检测视频接口
    }

}

//更新帧率，视频，摄像头
void MainWindow::updateFrame()
{
    cv::Mat frame;
    if(filetype=="video"){
        if(capture->read(frame)){
            cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
            QImage videoimg = QImage(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
            QPixmap mmp = QPixmap::fromImage(videoimg);
            mmp = mmp.scaled(ui->lb_show->size(),Qt::KeepAspectRatio);
            ui->lb_show->setPixmap(mmp);//将调整后的图片放到标签上
        }
    }
    else if(filetype=="camera")
    {
        cv::Mat src;
        if(capture->isOpened())
        {
            //将摄像头数据放入src
            *capture >> src;
            if(src.data == nullptr)
                return;
        }
        //将图像转换为qt能够处理的格式
        cv::cvtColor(src,frame,cv::COLOR_BGR2RGB);
        cv::flip(frame,frame,1);
        //将opcv的mat对象转换为img对象
        QImage videoimg = QImage(frame.data,frame.cols,frame.rows,frame.step,
                                 QImage::Format_RGB888);
        //根据标签大小调整图片
        QPixmap mmp = QPixmap::fromImage(videoimg);
        mmp = mmp.scaled(ui->lb_show->size(),Qt::KeepAspectRatio);
        ui->lb_show->setPixmap(mmp);//将调整后的图片放到标签上
    }
}

//打开摄像头
void MainWindow::on_btn_camera_clicked()
{
    filetype = "camera";
    if(ui->btn_camera->text() == "打开摄像头"){
        ui->btn_camera->setText("关闭摄像头");
        capture->open(0);
        timer->start(100);
    }else{
        ui->btn_camera->setText("打开摄像头");
        capture->release();
        timer->stop();
        ui->lb_show->clear();
    }
}

//开始检测
void MainWindow::on_btn_start_clicked()
{
    ui->btn_file->setEnabled(false);
    ui->btn_model->setEnabled(false);
    ui->btn_camera->setEnabled(false);
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);

    if(!(QFile::exists(onnxfile)))
    {
        ui->te_message->append("******未选择文件或模型******");
        return;
    }else{
        yolo.runModel(onnxfile,filename,mat,filetype);
        timer->stop();
    }

}

//加载模型
void MainWindow::on_btn_model_clicked()
{
    // 使用文件系统弹出对话框获得用户选择的文件路径
    onnxfile = QFileDialog::getOpenFileName(this,
                                            QStringLiteral("打开文件"),
                                            "./sourse",
                                            "*.onnx");
    ui->te_message->append("\n**********加载模型**********");
    if(!QFile::exists(onnxfile)){// 检查文件是否存在
        ui->te_message->append("**********未选中模型**********");
        return; // 如果文件不存在，则直接返回
    }else{
        ui->te_message->append(onnxfile);
        if(!yolo.loadModel(onnxfile)){
            ui->te_message->append("*********加载模型失败*********");
            return;
        }else{
             ui->te_message->append("*********加载模型成功*********");
        }
        return;
    }
}


//停止检测
void MainWindow::on_btn_stop_clicked()
{
    ui->btn_file->setEnabled(true);
    ui->btn_model->setEnabled(true);
    ui->btn_camera->setEnabled(true);
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(true);

//    MYLOG<<filetype;
    if(filetype == "image"){
        cv::Mat src = cv::imread(filename.toStdString());
        if(src.empty()){
            ui->lb_show->setText("图像不存在");
            return;
        }
        cv::Mat temp;
        if(src.channels() == 4){
            cv::cvtColor(src, temp, cv::COLOR_BGRA2RGB);
        }else if(src.channels() == 3){
            cv::cvtColor(src, temp, cv::COLOR_BGR2RGB);
        }else{
            cv::cvtColor(src, temp, cv::COLOR_GRAY2RGB);
        }
        QImage img = QImage(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        QPixmap mmp = QPixmap::fromImage(img);
        mmp = mmp.scaled(ui->lb_show->size(),Qt::KeepAspectRatio);
        ui->lb_show->setPixmap(mmp);
    }else if(filetype == "video"){
        yolo.stopModel(onnxfile,filename,mat,filetype);
        capture->open(filename.toStdString());
        long frameToStart = 0;
        capture->set(cv::CAP_PROP_POS_FRAMES,frameToStart);
        yolo.Capture(filename);
    }else{
        yolo.stopModel(onnxfile,filename,mat,filetype);
        capture->open(0);
        timer->start(100);
    }
}
