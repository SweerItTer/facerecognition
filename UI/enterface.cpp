#include "enterface.h"
#include "ui_enterface.h"
#pragma execution_character_set("UTF-8")
#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

enterface::enterface(QWidget *parent, Yolo *yolo,  FaceNet *facenet, FaceDatabase *database) 
    : QWidget(parent)
    , ui(new Ui::enterface)
    , capture(new cv::VideoCapture)
    , timer(new QTimer(this))
    , yolo_(yolo) // 传递配置好的yolo对象
    , facenet_ (facenet) // 传递配置好的facenet对象
    , database_(database) // 传递配置好的数据库对象
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    InitStyle();
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &enterface::pagechangebutton);
    
    // page = ui->stackedWidget->currentIndex();
    // vec_face.resize(10);

    
    connect(timer,&QTimer::timeout,this,&enterface::updateFrame);

}

enterface::~enterface()
{
    capture->release();
    delete ui;
    delete timer;
    delete capture;
}


// 样式初始化
void enterface::InitStyle()
{
    // 窗口
    this->setWindowFlags(Qt::FramelessWindowHint);      //隐藏最大最小化等按键
    this->setAttribute(Qt::WA_TranslucentBackground);         //设置窗口透明化
    this->setStyleSheet(QString("QWidget{"
                                 "background-color:rgb(236, 237, 255);"
                                 "border-radius: 10px;"
                                 "border-style:solid;"
                                 "border-width:2px;"
                                 "border-color:rgba(0, 0, 0, 0);"
                                 "}"));
    // 标题栏
    ui->widget->setStyleSheet(QString("QWidget{"
                                        "background-color: rgb(82, 85, 193);"
                                        "border-top-left-radius:10px;"
                                        "border-top-right-radius:10px;"
                                        "border-bottom-right-radius:0px;"
                                        "border-bottom-left-radius:0px;"
                                        "border-style:none;"
                                        "border-width:1px;"
                                        "border-color:rgba(82, 85, 193, 0);"
                                        "min-height: 30px;"
                                        "}"
                                        "QPushButton{"
                                        "background-color: rgba(103, 188, 234, 0);"
                                        "border-radius:0px;"
                                        "border-style:none;"
                                        "}"
                                        "QPushButton:hover{"
                                        "background-color: rgb(255, 89, 89);"
                                        "border-radius:10px;"
                                        "border-style:solid;"
                                        "border-color: rgb(255, 89, 89);"
                                        "}"));
    QIcon offIcon; // 关闭按钮图标
    offIcon.addFile(":/off.png", QSize(), QIcon::Normal);     // 正常状态
    ui->but_off->setIcon(offIcon);
    ui->but_off->setIconSize(QSize(30, 30)); // 设置图标的大小
    // 主体框
    ui->widget_2->setStyleSheet(QString("QWidget{"
                                        "background-color: rgb(236, 237, 255);"
                                        "border-top-left-radius:0px;"
                                        "border-top-right-radius:0px;"
                                        "border-bottom-right-radius:10px;"
                                        "border-bottom-left-radius:10px;"
                                        "border-style:solid;"
                                        "border-width:1px;"
                                        "border-color:rgba(82, 85, 193);"
                                        "min-height: 30px;"
                                        "}")); 


    // 引索
    pagechangebutton();

    // 上一步下一步按钮
    ui->but_back->setStyleSheet(QString("QPushButton{" 
                                "min-width: 130px;"
                                "min-height: 35px;"
                                "background-color: rgb(82, 85, 193);"
                                "border-radius:17px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color: rgb(255, 255, 255);"
                                "font-size:18px;"
                                "color: rgb(255, 255, 255);"
                                "}"
                                "QPushButton:hover{"
                                "background-color: rgba(179, 222, 245, 0.8);"
                                "border-color: rgb(82, 85, 193);"
                                "color: rgb(82, 85, 193);"
                                "}")); 
    ui->but_next->setStyleSheet(QString("QPushButton{" 
                                "min-width: 130px;"
                                "min-height: 35px;"
                                "background-color: rgb(82, 85, 193);"
                                "border-radius:17px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color: rgb(255, 255, 255);"
                                "font-size:18px;"
                                "color: rgb(255, 255, 255);"
                                "}"
                                "QPushButton:hover{"
                                "background-color: rgba(179, 222, 245, 0.8);"
                                "border-color: rgb(82, 85, 193);"
                                "color: rgb(82, 85, 193);"
                                "}"));                            

    // page1 个人信息
    ui->stackedWidget->setStyleSheet(QString("QStackedWidget{"
                                        "background-color: rgba(236, 237, 255, 0);"
                                        "border-style:none;"
                                        "border-width:1px;"
                                        "border-color:rgba(82, 85, 193, 0);"
                                        "}"
                                        "QWidget{"
                                        "background-color: rgba(236, 237, 255, 0);"
                                        "border-style:none;"
                                        "border-width:1px;"
                                        "border-color:rgba(82, 85, 193, 0);"
                                        "}"));
    // 输入框样式
    QString QLineEditStyle = QString("QLineEdit{"
                                "min-height: 35px;"
                                "background-color: rgba(255, 255, 255, 0.8);"
                                "border-radius:5px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color:rgb(190, 190, 190);"
                                "font-size:18px;"
                                "color: rgb(0, 0, 0);"
                                "}");
    ui->le_major->setStyleSheet(QLineEditStyle);
    ui->le_name->setStyleSheet(QLineEditStyle);
    ui->le_ID->setStyleSheet(QLineEditStyle);
    // 下拉框样式
    QString QComboBoxStyle = QString("QComboBox{"
                                "background-color: rgba(255, 255, 255, 0.8);"
                                "border-radius:5px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color:rgb(190, 190, 190);"
                                "font-size:18px;"
                                "padding: 5px;"      // 设置内边距，可以根据需要调整
                                "color: rgb(0, 0, 0);"
                                "}"
                                "QComboBox::drop-down{"
                                "border-style:none;"
                                "width:30px;"
                                "}"
                                "QComboBox::down-arrow{"
                                "width:15px;"
                                "border-image:url(:/comboBox_downarrow.png);"
                                "}"
                                "QComboBox QAbstractItemView{"
                                "font-size:18px;"
                                "border-radius:0px;"
                                "border-style:solid;"
                                "border-width:2px;"
                                "border-color:rgb(208, 208, 211);"
                                "background-color: rgb(255, 255, 255);"
                                "selection-background-color: rgb(82, 85, 193);" // 设置选中项的背景色
                                "selection-color: rgb(255, 255, 255);" // 设置选中项的文字颜色
                                "}");
    ui->cbB_college->setStyleSheet(QComboBoxStyle);
    ui->cbB_grade->setStyleSheet(QComboBoxStyle);
    // 标签样式 
    QString setPage_labelStyle = QString("QLabel{"
                                "border-style:solid;"
                                "font-size:18px;"
                                "color: rgb(0, 0, 0);"
                                "}");
    ui->lb_college->setStyleSheet(setPage_labelStyle);
    ui->lb_grade->setStyleSheet(setPage_labelStyle);
    ui->lb_class->setStyleSheet(setPage_labelStyle);
    ui->lb_name->setStyleSheet(setPage_labelStyle);
    ui->lb_ID->setStyleSheet(setPage_labelStyle);

    // page2 录入人脸
    // 按钮样式
    QString setPage_butStyle = QString("QPushButton{"
                                "width:130px;"
                                "height:40px;"
                                "background-color: rgba(255, 255, 255, 0.8);"
                                "border-radius:20px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color:rgb(190, 190, 190);"
                                "font-size:20px;"
                                "color: rgb(0, 0, 0);"
                                "}"
                                "QPushButton:hover{"
                                "border-color:rgb(82, 85, 193);"
                                "color: rgb(82, 85, 193);"
                                "}");
    ui->but_camera->setStyleSheet(setPage_butStyle);
    ui->but_panorama->setStyleSheet(setPage_butStyle);
    ui->but_facecut->setStyleSheet(setPage_butStyle);
    ui->but_save->setStyleSheet(setPage_butStyle);

    but_del_style();
    // QIcon delIcon; // 删除按钮图标
    // delIcon.addFile(":/del_on.png", QSize(), QIcon::Disabled); // 禁用状态
    // delIcon.addFile(":/del.png", QSize(), QIcon::Normal); // 正常状态
    // ui->but_delet1->setIcon(delIcon);
    // ui->but_delet1->setIconSize(QSize(30, 30)); // 设置图标的大小
    // ui->but_delet2->setIcon(delIcon);
    // ui->but_delet2->setIconSize(QSize(30, 30)); // 设置图标的大小
    // ui->but_delet3->setIcon(delIcon);
    // ui->but_delet3->setIconSize(QSize(30, 30)); // 设置图标的大小



    // page3 验证人脸

    // page4 完成
    ui->but_nextpeople->setStyleSheet(setPage_butStyle);
    ui->but_end->setStyleSheet(setPage_butStyle);

    QDir dir("UI/Resources/images");
    // 获取文件夹中所有文件的列表，这里假设图片文件扩展名为.jpg
    QFileInfoList imageFiles = dir.entryInfoList(QStringList() << "*.jpg" << "*.png" << "*.jpeg", QDir::Files);
    if (!imageFiles.isEmpty()) 
    {
    // 随机选择一个文件
    int index = QRandomGenerator::global()->bounded(imageFiles.size());
    QFileInfo fileInfo = imageFiles.at(index);
    QPixmap pixmap(fileInfo.absoluteFilePath());
    pixmap = pixmap.scaled(ui->lb_test->size(),Qt::KeepAspectRatio);
    ui->lb_test->setPixmap(pixmap);
    ui->lb_test->setScaledContents(true);
    }


}

// 重绘窗口
void enterface::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

// 监控鼠标点击的位置
void enterface::mousePressEvent(QMouseEvent *event)
{
    int x = ui->widget->x();
    int y = ui->widget->y();
    int w = ui->widget->width();
    int h = ui->widget->height();
    //只能是鼠标左键移动和改变大小
    if((event->button() == Qt::LeftButton) //处于左键状态
        &&(event->x()>x&&event->x()<x+w)  //x坐标位置判定
        &&(event->y()>y&&event->y()<y+h)) //y坐标位置判定
    {
    m_leftMousePressed = true;  //标志位置为真
    //按下时鼠标左键时，窗口在屏幕中的坐标
    m_StartPoint = event->globalPos();
    }
}

// 监控鼠标移动位置
void enterface::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口,由于取消了标题栏，因此需要自己实现拖动窗口功能
    if (m_leftMousePressed)
    {
        QPoint curPoint = event->globalPos();   //按住移动时的位置
        QPoint movePoint = curPoint - m_StartPoint; //与初始坐标做差，得位移
        //普通窗口
        QPoint mainWinPos = this->pos();
        //设置窗口的全局坐标
        this->move(mainWinPos.x() + movePoint.x(), mainWinPos.y() + movePoint.y());
        m_StartPoint = curPoint;
     }
}

// 监控鼠标释放位置
void enterface::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMousePressed = false;//释放鼠标，标志位置为假
}

void enterface::on_but_off_clicked()
{
    enterface::close();
}



void enterface::on_but_data_clicked()
{
    page = 0;
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::on_but_face_clicked()
{
    page = 1;
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::on_but_verify_clicked()
{
    page = 2;
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::on_but_finish_clicked()
{
    page = 3;
    ui->stackedWidget->setCurrentIndex(page);
}

void enterface::pagechangebutton()
{
    // 按键样式
    QString  tickStyle = QString("QToolButton{"
                                "background-color: rgba(255, 255, 255, 0);"
                                "border-radius:8px;"
                                "border-style:none;"
                                "border-width:1px;"
                                "border-color:rgba(0, 0, 0, 0);"
                                "font-size:20px;"
                                "font-weight:bold;"
                                "color: rgb(138, 138, 138);"
                                "}");
    QString  tick_onStyle = QString("QToolButton{"
                                "background-color: rgba(255, 255, 255, 0);"
                                "border-radius:8px;"
                                "border-style:none;"
                                "border-width:1px;"
                                "border-color:rgba(0, 0, 0, 0);"
                                "font-size:20px;"
                                "font-weight:bold;"
                                "color: rgb(82, 85, 193);"
                                "}");
    QIcon tickIcon; // 关闭按钮图标
    tickIcon.addFile(":/tick.png", QSize(), QIcon::Normal);     // 正常状态
    QIcon tickIcon_on; 
    tickIcon_on.addFile(":/tick_on.png", QSize(), QIcon::Normal);     // 按下状态

    ui->but_data->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_face->setIconSize(QSize(30, 30));
    ui->but_verify->setIconSize(QSize(30, 30));
    ui->but_finish->setIconSize(QSize(30, 30));

    ui->but_data->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 设置按钮的样式
    ui->but_face->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->but_verify->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->but_finish->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // 标签
    QPixmap rail = QPixmap::fromImage(QImage(QString(":/rail.png")));
    QPixmap rail_on = QPixmap::fromImage(QImage(QString(":/rail_on.png")) );
    rail = rail.scaled(ui->rail_2->size(), Qt::KeepAspectRatio);
    rail_on = rail_on.scaled(ui->rail_2->size(), Qt::KeepAspectRatio);
    // ui->rail_1->setScaledContents(true);
    // ui->rail_2->setScaledContents(true);
    // ui->rail_3->setScaledContents(true);

    QString  labelStyle = QString("QLabel{"
                            "background-color: rgba(255, 255, 255, 0);"
                            "border-style:none;"
                            "border-color:rgba(0, 0, 0, 0);"
                            "color: rgba(0, 0, 0, 0);"
                            "}");
    ui->rail_1->setStyleSheet(labelStyle);
    ui->rail_2->setStyleSheet(labelStyle);
    ui->rail_3->setStyleSheet(labelStyle);

    switch (ui->stackedWidget->currentIndex())
    {
    case 0 :
        ui->but_data->setIcon(tickIcon_on);
        ui->but_face->setIcon(tickIcon);
        ui->but_verify->setIcon(tickIcon);
        ui->but_finish->setIcon(tickIcon);

        ui->but_data->setStyleSheet(tick_onStyle);
        ui->but_face->setStyleSheet(tickStyle);
        ui->but_verify->setStyleSheet(tickStyle);
        ui->but_finish->setStyleSheet(tickStyle);

        ui->rail_1->setPixmap(rail);
        ui->rail_2->setPixmap(rail);
        ui->rail_3->setPixmap(rail);

        ui->but_back->show();
        ui->but_next->show();
        break;
    case 1 :
        ui->but_data->setIcon(tickIcon_on);
        ui->but_face->setIcon(tickIcon_on);
        ui->but_verify->setIcon(tickIcon);
        ui->but_finish->setIcon(tickIcon);

        ui->but_data->setStyleSheet(tick_onStyle); 
        ui->but_face->setStyleSheet(tick_onStyle);
        ui->but_verify->setStyleSheet(tickStyle);
        ui->but_finish->setStyleSheet(tickStyle);

        ui->rail_1->setPixmap(rail_on);
        ui->rail_2->setPixmap(rail);
        ui->rail_3->setPixmap(rail); 

        ui->but_back->show();
        ui->but_next->show();
        break;
    case 2 :
        ui->but_data->setIcon(tickIcon_on);
        ui->but_face->setIcon(tickIcon_on);
        ui->but_verify->setIcon(tickIcon_on);
        ui->but_finish->setIcon(tickIcon);

        ui->but_data->setStyleSheet(tick_onStyle);
        ui->but_face->setStyleSheet(tick_onStyle);
        ui->but_verify->setStyleSheet(tick_onStyle);
        ui->but_finish->setStyleSheet(tickStyle);

        ui->rail_1->setPixmap(rail_on);
        ui->rail_2->setPixmap(rail_on);
        ui->rail_3->setPixmap(rail);

        ui->but_back->show();
        ui->but_next->show();
        break;
    case 3 :
        ui->but_data->setIcon(tickIcon_on);
        ui->but_face->setIcon(tickIcon_on);
        ui->but_verify->setIcon(tickIcon_on);
        ui->but_finish->setIcon(tickIcon_on);

        ui->but_data->setStyleSheet(tick_onStyle);
        ui->but_face->setStyleSheet(tick_onStyle);
        ui->but_verify->setStyleSheet(tick_onStyle);
        ui->but_finish->setStyleSheet(tick_onStyle);

        ui->rail_1->setPixmap(rail_on);
        ui->rail_2->setPixmap(rail_on);
        ui->rail_3->setPixmap(rail_on);

        ui->but_back->hide();
        ui->but_next->hide();
        break;
    }
}

// 上一步
void enterface::on_but_back_clicked()
{
    page--;
    ui->stackedWidget->setCurrentIndex(page);
}

// 下一步
void enterface::on_but_next_clicked()
{
    if(page == 0){
        if(ui->le_major->text().isEmpty() || ui->le_name->text().isEmpty() || ui->le_ID->text().isEmpty() 
            || ui->cbB_college->currentText().isEmpty() || ui->cbB_grade->currentText().isEmpty())
        {
            QMessageBox::warning(this, tr("Incomplete information"),
             tr("Please complete the personal information first"));
            return;
        }
    }else if(page == 1){
        // if(ui->lb_photo1->pixmap() == nullptr || ui->lb_photo1->pixmap()->isNull()
        //     || ui->lb_photo2->pixmap() == nullptr || ui->lb_photo2->pixmap()->isNull()
        //     || ui->lb_photo3->pixmap() == nullptr || ui->lb_photo3->pixmap()->isNull())
        if(vec_face.size() < 10) // 以人脸图片为准
        {
            QMessageBox::warning(this, tr("Incomplete information"),
             tr("Please complete the face information first"));
            return;
        }
        EnterInformation();
    }

    // else if(page == 2){
    //     if(ui->lb_camera->pixmap() == nullptr || ui->lb_camera->pixmap()->isNull())
    //     {
    //         QMessageBox::warning(this, tr("Incomplete information"),
    //          tr("Please complete the verification information first"));
    //         return;
    //     }
    // }

    page++;
    ui->stackedWidget->setCurrentIndex(page);
}

// ------------------------

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
        ui->but_facecut->setEnabled(true);
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
    } else return;

    if(src.empty()){
        return;
    }

    // 使用 YOLO 模型进行图像处理
    yolo_->runModel(src, "image", retImg);

    mat_panorama = retImg[0];
    if(retImg.size() > 1){
        mat_face = retImg[1];
        retImg[retImgIndex].copyTo(frame);
    }

    retImg[0].copyTo(frame);    

    cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
    cv::flip(frame,frame,1);
    QImage videoimg = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(videoimg);
    pixmap = pixmap.scaled(ui->lb_camera->size(),Qt::KeepAspectRatio);
    ui->lb_camera->setPixmap(pixmap);//将调整后的图片放到标签上
}


// 打开全景图
void enterface::on_but_panorama_clicked()
{
    retImgIndex = 0;
    ui->but_panorama->setEnabled(false);
    ui->but_facecut->setEnabled(true);

} 

// 人脸分割
void enterface::on_but_facecut_clicked()
{
    retImgIndex = 1;
    ui->but_panorama->setEnabled(true);
    ui->but_facecut->setEnabled(false);
} 


// 保存图片
void enterface::on_but_save_clicked()
{
    if(mat_face.empty()) return;
    timer->stop();

    if(vec_face.size() >= 10){
        QMessageBox::warning(this, tr("Save images excessively"),
            tr("The image has reached the maximum limit \n please delete the image first"));
        but_del_style();
        timer->start(100);
        return;    
    }

    // 保存当前图像到vec_face中
    cv::Mat current_face = mat_face.clone();
    cv::cvtColor(current_face, current_face, cv::COLOR_BGR2RGB);
    cv::flip(current_face, current_face, 1);
    vec_face.push_back(current_face);

    // 将最新图片显示到下一个 QLabel 中，循环覆盖显示
    static int label_index = 0;  // 跟踪显示位置
    std::vector<QLabel*> photos_l = {ui->lb_photo1, ui->lb_photo2, ui->lb_photo3};

    cv::Mat img = vec_face.back();
    QImage qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(qimg).scaled(photos_l[label_index]->size(), Qt::KeepAspectRatio);
    photos_l[label_index]->setPixmap(pixmap);

    // 更新下一个标签位置
    label_index = (label_index + 1) % photos_l.size();
    /*
	if(ui->lb_photo1->pixmap() == nullptr || ui->lb_photo1->pixmap()->isNull())
    {
        mat_1 = mat_face.clone();
        cv::cvtColor(mat_1, mat_1, cv::COLOR_BGR2RGB);
        cv::flip(mat_1, mat_1, 1);
        QImage img = QImage(mat_1.data, mat_1.cols, mat_1.rows, mat_1.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap = pixmap.scaled(ui->lb_photo1->size(),Qt::KeepAspectRatio);
        ui->lb_photo1->setPixmap(pixmap);
    }
    else if(ui->lb_photo2->pixmap() == nullptr || ui->lb_photo2->pixmap()->isNull())
    {
        mat_2 = mat_face.clone();
        cv::cvtColor(mat_2, mat_2, cv::COLOR_BGR2RGB);
        cv::flip(mat_2, mat_2, 1);
        QImage img = QImage(mat_2.data, mat_2.cols, mat_2.rows, mat_2.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap = pixmap.scaled(ui->lb_photo2->size(),Qt::KeepAspectRatio);
        ui->lb_photo2->setPixmap(pixmap);
    }
    else if(ui->lb_photo3->pixmap() == nullptr || ui->lb_photo3->pixmap()->isNull())
    {
        mat_3 = mat_face.clone();
        cv::cvtColor(mat_3, mat_3, cv::COLOR_BGR2RGB);
        cv::flip(mat_3, mat_3, 1);
        QImage img = QImage(mat_3.data, mat_3.cols, mat_3.rows, mat_3.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap = pixmap.scaled(ui->lb_photo3->size(),Qt::KeepAspectRatio);
        ui->lb_photo3->setPixmap(pixmap);
    }
    else
    {
        QMessageBox::warning(this, tr("Save images excessively"),
         tr("The image has reached the maximum limit \n please delete the image first"));
    }
    */
    but_del_style();
    timer->start(100);
}

 // 删除按键的样式
void enterface::but_del_style()
{
    QIcon delIcon; // 删除按钮图标
    delIcon.addFile(":/del_on.png", QSize(), QIcon::Normal); // 禁用状态
    delIcon.addFile(":/del.png", QSize(), QIcon::Disabled); // 正常状态

    ui->but_delet1->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_delet2->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_delet3->setIconSize(QSize(30, 30)); // 设置图标的大小

    ui->but_delet1->setIcon(delIcon);
    ui->but_delet2->setIcon(delIcon);    
    ui->but_delet3->setIcon(delIcon);

    if(ui->lb_photo1->pixmap() == nullptr || ui->lb_photo1->pixmap()->isNull()){
        ui->but_delet1->setEnabled(false);
    }else{
        ui->but_delet1->setEnabled(true);
    }
    if(ui->lb_photo2->pixmap() == nullptr || ui->lb_photo2->pixmap()->isNull()){
        ui->but_delet2->setEnabled(false);
    }else{
        ui->but_delet2->setEnabled(true);
    }
    if(ui->lb_photo3->pixmap() == nullptr || ui->lb_photo3->pixmap()->isNull()){
        ui->but_delet3->setEnabled(false);
    }else{
        ui->but_delet3->setEnabled(true);
    }
}
// 删除图片
// 目前只是删除表面的图片，实际上容器内的图片并不会被删除
void enterface::on_but_delet1_clicked()
{
    ui->lb_photo1->clear();
    but_del_style();
}
void enterface::on_but_delet2_clicked()
{
    ui->lb_photo2->clear();
    but_del_style();
}
void enterface::on_but_delet3_clicked()
{
    ui->lb_photo3->clear();
    but_del_style();
}

// 录入信息
void enterface::EnterInformation()
{
    // if(name_ == name) return; // 防止重复录入同一个人

    name_ = ui->le_name->text().toStdString();
    std::string ID = ui->le_ID->text().toStdString();
    std::string major = ui->le_major->text().toStdString();
    std::string grade = ui->cbB_grade->currentText().toStdString();
    std::string college = ui->cbB_college->currentText().toStdString();

    std::vector<std::vector<float>> feature_vectors;
     // 循环遍历 vec_face 中的每一张人脸图片，提取特征向量
    for (const cv::Mat& face : vec_face)
    {
        // 这里假设 facenet_->outputs 处理输入是 {1, 3, 160, 160} 尺寸的图片
        std::vector<float> feature_vector = facenet_->outputs(face, {1, 3, 160, 160});
        feature_vectors.push_back(feature_vector);
    }

    bool ret = database_->insertFeatures(name_, feature_vectors);
    if(!ret) MYLOG << "insert features failed 插入特征失败";

    bool ret2 = database_->insertIDinformation(name_, ID, major, grade, college);
    if(!ret2) MYLOG << "insert ID information failed 插入ID信息失败";
    
    MYLOG<< "insert features successfully 插入特征成功";
    vec_face.clear();
    name = name_;

}

// ------------------------
// 结束录入
void enterface::on_but_end_clicked()
{
    on_but_off_clicked();
}
// 录入下一个人
void enterface::on_but_nextpeople_clicked()
{
    ui->le_name->clear();
    ui->le_ID->clear();
    ui->le_major->clear();
    ui->cbB_college->setCurrentIndex(0);
    ui->cbB_grade->setCurrentIndex(0);

    ui->stackedWidget->setCurrentIndex(0);
    pagechangebutton();

    ui->lb_photo1->clear();
    ui->lb_photo2->clear();
    ui->lb_photo3->clear();
}
