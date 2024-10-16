#include "enterface.h"
#include "ui_enterface.h"
#pragma execution_character_set("UTF-8")
#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

enterface::enterface(QWidget *parent, QString YoloMPath, QString FaceMPath, Yolo *yolo) 
    : QWidget(parent)
    , YoloMPath_(YoloMPath)
    , FaceMPath_(FaceMPath)
    , ui(new Ui::enterface)
    , capture(new cv::VideoCapture)
    , timer(new QTimer(this))
    , yolo_(yolo) // 传递配置好的yolo对象
{
    ui->setupUi(this);
    
    page = ui->stackedWidget->currentIndex();
    StackedWInit();
    
    connect(timer,&QTimer::timeout,this,&enterface::updateFrame);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, [=](int index){
        page = index;
        StackedWInit();
    });
    
}

enterface::~enterface()
{
    delete ui;
}

void enterface::setSession(Ort::Session *&session)
{
    session_ = session;
}

void enterface::on_but_data_clicked()
{
    page = 0;
    StackedWInit();
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::on_but_face_clicked()
{
    page = 1;
    StackedWInit();
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::on_but_verify_clicked()
{
    page = 2;
    StackedWInit();
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::on_but_finish_clicked()
{
    page = 3;
    StackedWInit();
    ui->stackedWidget->setCurrentIndex(page);
}

// 上一步
void enterface::on_but_back_clicked()
{
    page--;
    StackedWInit();
    ui->stackedWidget->setCurrentIndex(page);
}

// 下一步
void enterface::on_but_next_clicked()
{
    page++;
    StackedWInit();
    ui->stackedWidget->setCurrentIndex(page);
}

//堆叠容器初始化
void enterface::StackedWInit()
{
    if(page == 0){
    ui->but_back->setEnabled(false);
    }else if (page == 3){
        ui->but_back->hide();
        ui->but_next->hide();
    }else{
       ui->but_back->show();
       ui->but_next->show();
       ui->but_back->setEnabled(true); 
    }
}

// 打开摄像头
void enterface::on_but_camera_clicked()
{
    if (ui->but_camera->text() == "open camera")
    {
        capture->open(0);
        if(!capture->isOpened()){
            MYLOG<< "camera open failed 摄像头打开失败";
            return;
        }
        ui->but_camera->setText("close camera");
        timer->start(100);
    }else{
        ui->but_camera->setText("open camera");
        capture->release();
        timer->stop();
        ui->lb_camera->clear();
    }
}

//更新帧率，视频，摄像头
void enterface::updateFrame()
{
    std::vector<cv::Mat> retImg;
    cv::Mat frame;
    cv::Mat src;
    if(capture->isOpened())
    {
        //将摄像头数据放入src
        *capture >> src;
        if(src.data == nullptr)
            return;
    }

    // 使用 YOLO 模型进行图像处理
    yolo_->runModel(src, "image", retImg);
    frame = retImg[0];     
    //将图像转换为qt能够处理的格式
    cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
    cv::flip(frame,frame,1);
    //将opcv的mat对象转换为img对象
    QImage videoimg = QImage(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
    //根据标签大小调整图片
    QPixmap pixmap = QPixmap::fromImage(videoimg);
    pixmap = pixmap.scaled(ui->lb_camera->size(),Qt::KeepAspectRatio);
    ui->lb_camera->setPixmap(pixmap);//将调整后的图片放到标签上
}


// 打开全景图
void enterface::on_but_panorama_clicked()
{
    qDebug() << "你不准玩原神";
} 

// 人脸分割
void enterface::on_but_facecut_clicked()
{

} 

// 截图
void enterface::on_bur_screenshot_clicked()
{

} 

// 保存图片
void enterface::on_but_save_clicked()
{

}

// 取消操作
void enterface::on_but_cancel_clicked()
{

}

