#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configure.h"

#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"
static bool isplay = false;


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 将创建好的对象传递给callback
	callback = new Script(this, yolo, facenet, database);
    ui_enterface = new enterface(nullptr, yolo, facenet, database);
    
    // 初始化
    // page0 首页统计图
    setBarChart();
    setBarChart_2();
    setProgressBar();
    setPieChart();
    setLineChart();
    setSplineChart();
    // page2 数据库

    StyleSheetInit(); // 样式初始化
}

MainWindow::~MainWindow()
{
    if (yolo) {
        delete yolo;
        yolo = nullptr; // 将指针设为nullptr
    }
    if (facenet) {
        delete facenet;
        facenet = nullptr; // 将指针设为nullptr
    }
    if (callback) {
        delete callback;
        callback = nullptr;
    }
    delete ui_enterface;
    delete ui;
}


// 样式初始化
void MainWindow::StyleSheetInit()
{
    // 窗口
    this->setWindowFlags(Qt::FramelessWindowHint);      //隐藏最大最小化等按键
    this->setAttribute(Qt::WA_TranslucentBackground);         //设置窗口透明化
    this->setStyleSheet(QString("QMainWindow{"
                                 "background-color:rgb(236, 237, 255);"
                                 "border-radius: 10px;"
                                 "border-style:none;"
                                 "border-width:0px;"
                                 "border-color:rgb(213, 213, 213);"
                                 "}"));

    // 侧边栏
    ui->but_user->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->but_user->setStyleSheet(QString("QToolButton{"
                                    "background-color: rgba(255, 255, 255, 0);"
                                    "border-radius:8px;"
                                    "border-style:none;"
                                    "border-width:1px;"
                                    "border-color:rgba(0, 0, 0, 0);"
                                    "font-size:20px;"
                                    "color: rgb(255, 255, 255);"
                                    "}"
                                    "QToolButton:hover{"
                                    "background-color: rgba(103, 188, 234, 0.3);"
                                    "color: rgb(82, 85, 193);"
                                    "}"
                                    "QToolButton:checked {"
                                    "background-color: rgba(245, 139, 89, 0.5);"
                                    "color: rgb(255, 255, 255);"
                                    "}"));

    QButtonGroup *group = new QButtonGroup(this);
    // group->addButton(ui->but_user, 0);
    group->addButton(ui->but_home, 0);
    group->addButton(ui->but_camera, 1);    
    group->addButton(ui->but_data, 2);
    group->addButton(ui->but_set, 3);
    group->setExclusive(true);

    // ui->but_user->setCheckable(true);
    ui->but_home->setCheckable(true);
    ui->but_camera->setCheckable(true);
    ui->but_data->setCheckable(true);
    ui->but_set->setCheckable(true);

    QString sidebar_butStyle = QString("QPushButton{"
                                    // "text-align: left;" // 文字左对齐
                                    // "icon-align: left;" // 图标左对齐
                                    // "padding-left: 10px;" // 增加左侧内边距，为图标留出空间
                                    "background-color: rgba(255, 255, 255, 0);"
                                    "border-radius:8px;"
                                    "border-style:none;"
                                    "border-width:1px;"
                                    "border-color:rgba(0, 0, 0, 0);"
                                    "font-size:20px;"
                                    "font-weight:bold;"
                                    "color: rgb(255, 255, 255);"
                                    "}"
                                    // "QPushButton:icon {"
                                    // "margin-right: 8px;" // 设置图标与文本之间的间距
                                    // "}"
                                    "QPushButton:hover{"
                                    "background-color: rgba(179, 222, 245, 0.5);"
                                    "}"
                                    "QPushButton:checked {"
                                    "background-color: rgba(255, 255, 255, 0.8);"
                                    "color: rgb(82, 85, 193);"
                                    "}");
    
    QIcon homeIcon;
    homeIcon.addFile(":/home.png", QSize(), QIcon::Normal);     // 正常状态
    homeIcon.addFile(":/home_on.png", QSize(), QIcon::Active); // 按下状态
    ui->but_home->setIcon(homeIcon);
    ui->but_home->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_home->setStyleSheet(sidebar_butStyle);

    QIcon cameraIcon;
    cameraIcon.addFile(":/camare.png", QSize(), QIcon::Normal);     // 正常状态
    cameraIcon.addFile(":/camare_on.png", QSize(), QIcon::Active); // 按下状态
    ui->but_camera->setIcon(cameraIcon);
    ui->but_camera->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_camera->setStyleSheet(sidebar_butStyle);

    QIcon dataIcon;
    dataIcon.addFile(":/data.png", QSize(), QIcon::Normal);     // 正常状态
    dataIcon.addFile(":/data_on.png", QSize(), QIcon::Active); // 按下状态
    ui->but_data->setIcon(dataIcon);
    ui->but_data->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_data->setStyleSheet(sidebar_butStyle);

    QIcon setIcon;
    setIcon.addFile(":/set.png", QSize(), QIcon::Normal);     // 正常状态
    setIcon.addFile(":/set_on.png", QSize(), QIcon::Active); // 按下状态
    ui->but_set->setIcon(setIcon);
    ui->but_set->setIconSize(QSize(30, 30)); // 设置图标的大小
    ui->but_set->setStyleSheet(sidebar_butStyle);

    QIcon userIcon;
    userIcon.addFile(":/user.png", QSize(), QIcon::Normal);     // 正常状态
    ui->but_user->setIcon(userIcon);
    ui->but_user->setIconSize(QSize(80, 80)); // 设置图标的大小
    ui->but_user->setStyleSheet(QString("QToolButton{"
                                        "background-color: rgba(255, 255, 255, 0);"
                                        "padding-top: 15px;"
                                        "border-radius:8px;"
                                        "border-style:none;"
                                        "border-width:1px;"
                                        "border-color:rgba(0, 0, 0, 0);"
                                        "font-size:16px;"
                                        "font-weight:bold;"
                                        "color: rgb(255, 255, 255);"
                                        "}"));

    ui->widget->setStyleSheet(QString("QWidget{"
                                        "background-color: rgb(116, 121, 206);"
                                        "border-top-left-radius:0px;"
                                        "border-top-right-radius:0px;"
                                        "border-bottom-right-radius:0px;"
                                        "border-bottom-left-radius:10px;"
                                        "border-style:solid;"
                                        "border-width:1px;"
                                        "border-color:rgb(116, 121, 206);"
                                        "}"));


    // 标题栏
    ui->widget_2->setStyleSheet(QString("QWidget{"
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
                                        "background-color: rgba(103, 188, 234, 0.5);"
                                        "border-radius:10px;"
                                        "}"));
    ui->but_off->setStyleSheet("QPushButton:hover{"
                               "background-color: rgb(255, 89, 89);"
                               "border-radius:10px;"
                               "border-style:solid;"
                               "border-color: rgb(255, 89, 89);"
                               "}");


    // 主页
    QImage image (QString(":/lb_camaretest.png")) ;
    QPixmap pixmap = QPixmap::fromImage(image);
    pixmap = pixmap.scaled(ui->lb_cameramin->size(),Qt::KeepAspectRatio);
    pixmap = getRoundRectPixmap(pixmap,pixmap.size(),20);
    ui->lb_cameramin->setPixmap(pixmap);
    ui->lb_cameramin->setScaledContents(true);
    ui->lb_cameramin->setStyleSheet(QString("QLabel{"
                                            "border-radius:20px;"
                                            "border-style:none;"
                                            "border-width:1px;"
                                            "border-color:rgb(82, 85, 193);"
                                            "}"));

    // 下拉框样式
    QString comboboxStyle = QString("QComboBox{"
                                    "font-size:14px;"
                                    "border-radius:15px;"
                                    "border-style:solid;"
                                    "border-width:2px;"
                                    "border-color:rgb(208, 208, 211);"
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
                                    "font-size:14px;"
                                    "border-radius:0px;"
                                    "border-style:solid;"
                                    "border-width:2px;"
                                    "border-color:rgb(208, 208, 211);"
                                    "background-color: rgb(255, 255, 255);"
                                    "selection-background-color: rgb(82, 85, 193);" // 设置选中项的背景色
                                    "selection-color: rgb(255, 255, 255);" // 设置选中项的文字颜色
                                    "}");

    ui->box_spline->setStyleSheet(comboboxStyle);
    ui->widget_spline->setStyleSheet(QString("QWidget{"
                                             "background-color: rgb(255, 255, 255);"
                                             "border-radius:20px;"
                                             "border-style:solid;"
                                             "border-width:1px;"
                                             "border-color:rgb(255,255,255);"
                                             "}"));


    ui->widget_bar->setStyleSheet(QString("QWidget{"
                                             "background-color: rgb(255, 255, 255);"
                                             "border-radius:20px;"
                                             "border-style:solid;"
                                             "border-width:1px;"
                                             "border-color:rgb(255,255,255);"
                                             "}"));
    ui->widget_bar2->setStyleSheet(QString("QWidget{"
                                             "background-color: rgb(255, 255, 255);"
                                             "border-radius:20px;"
                                             "border-style:solid;"
                                             "border-width:1px;"
                                             "border-color:rgb(255,255,255);"
                                             "}"));

    // 主页按键样式
    QString QPBut_Style = QString("QPushButton{"
                                "background-color: rgb(82, 85, 193);"
                                "border-radius:20px;"
                                "border-style:solid;"
                                "}"
                                "QPushButton:hover{"
                                "background-color: rgba(103, 188, 234, 0.5);"
                                "}");
    ui->pushButton->setStyleSheet(QPBut_Style);
    ui->widget_rgbar->setStyleSheet(QString("QWidget{"
                                            "background-color: rgb(255, 255, 255);"
                                            "border-radius:20px;"
                                            "border-style:solid;"
                                            "border-width:1px;"
                                            "border-color:rgb(255,255,255);"
                                            "}"
                                            ));

    ui->box_pie->setStyleSheet(comboboxStyle);
    ui->widget_pie->setStyleSheet(QString("QWidget{"
                                          "background-color: rgb(255, 255, 255);"
                                          "border-radius:20px;"
                                          "border-style:solid;"
                                          "border-width:1px;"
                                          "border-color:rgb(255,255,255);"
                                          "}"));
    ui->lb_piedata->setStyleSheet(QString("QLabel{"
                                          "background-color: rgba(255, 255, 255, 0);"
                                          "border-style:none;"
                                          "}"));
    ui->widget_calendar->setStyleSheet(QString("QWidget{"
                                               "background-color: rgb(255, 255, 255);"
                                               "border-radius:20px;"
                                               "border-style:solid;"
                                               "border-width:1px;"
                                               "border-color:rgb(255,255,255);"
                                               "}"));
    ui->widget_4->setStyleSheet(QString("QWidget{background-color: rgb(255, 255, 255);"
                                        "border-radius:20px;"
                                        "border-style:solid;"
                                        "border-width:1px;"
                                        "border-color:rgb(255,255,255);"
                                        "}"));


    // 摄像头界面
    QImage camera_img (QString(":/lb_camaretest.png")) ;
    QPixmap camera_pix = QPixmap::fromImage(camera_img);
//    camera_pix = camera_pix.scaled(ui->lb_camera->size(),Qt::KeepAspectRatio);
//    ui->lb_camera->setScaledContents(true);
    ui->lb_camera->setPixmap(camera_pix);
    ui->lb_camera->setStyleSheet(QString("QLabel{"
                                         "background-color: rgb(255, 255, 255);"
                                         "border-radius:20px;"
                                         "border-style:solid;"
                                         "border-width:1px;"
                                         "border-color:rgb(255,255,255);"
                                         "}"));
    ui->lb_cameramessage->setStyleSheet(QString("QLabel{"
                                         "background-color: rgb(255, 255, 255);"
                                         "border-radius:20px;"
                                         "border-style:solid;"
                                         "border-width:1px;"
                                         "border-color:rgb(255,255,255);"
                                         "}"));
    // 数据库界面

    // 设置界面
    // 按钮样式
    QString setPage_butStyle = QString("QPushButton{"
                                "background-color: rgba(255, 255, 255, 0.8);"
                                "border-radius:17px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color:rgb(190, 190, 190);"
                                "font-size:18px;"
                                "color: rgb(0, 0, 0);"
                                "}"
                                "QPushButton:hover{"
                                "border-color:rgb(82, 85, 193);"
                                "color: rgb(82, 85, 193);"
                                "}");                                   
    ui->but_rtsp->setStyleSheet(setPage_butStyle);
    ui->but_onnx->setStyleSheet(setPage_butStyle);
    ui->but_facenetonnx->setStyleSheet(setPage_butStyle);
    ui->but_storagefile->setStyleSheet(setPage_butStyle);
    ui->but_enterface->setStyleSheet(setPage_butStyle);
    ui->but_message->setStyleSheet(setPage_butStyle);

    ui->but_sure->setStyleSheet(QString("QPushButton{"
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
           
    // 输入框样式
    QString setPage_inputStyle = QString("QLineEdit{"
                                "background-color: rgba(255, 255, 255, 0.8);"
                                "border-radius:5px;"
                                "border-style:solid;"
                                "border-width:1px;"
                                "border-color:rgb(190, 190, 190);"
                                "font-size:18px;"
                                "color: rgb(0, 0, 0);"
                                "}");
    ui->le_rtsp->setStyleSheet(setPage_inputStyle);
    ui->le_onnx->setStyleSheet(setPage_inputStyle);
    ui->le_facenetonnx->setStyleSheet(setPage_inputStyle);
    ui->le_storagefile->setStyleSheet(setPage_inputStyle);
    ui->le_message->setStyleSheet(setPage_inputStyle);
    ui->le_facedata->setStyleSheet(setPage_inputStyle);
    
    // QTextEdit样式
    ui->te_onnxmessage->setStyleSheet(QString("QTextEdit{"
                                    "background-color: rgba(255, 255, 255, 0.8);"
                                    "border-radius:5px;"
                                    "border-style:solid;"
                                    "border-width:1px;"
                                    "border-color:rgb(190, 190, 190);"
                                    "font-size:18px;"
                                    "color: rgb(0, 0, 0);"
                                    "}"));
    // 标签样式 
    QString setPage_labelStyle = QString("QLabel{"
                                "font-size:18px;"
                                "color: rgb(0, 0, 0);"
                                "}");
    ui->lb_rtsp->setStyleSheet(setPage_labelStyle);
    ui->lb_onnx->setStyleSheet(setPage_labelStyle);
    ui->lb_facenet->setStyleSheet(setPage_labelStyle);
    ui->lb_storagefile->setStyleSheet(setPage_labelStyle);
    ui->lb_message->setStyleSheet(setPage_labelStyle);
    ui->lb_enterface->setStyleSheet(setPage_labelStyle);


}

// 重绘窗口
void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

// 监控鼠标点击的位置
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x = ui->widget_2->x();
    int y = ui->widget_2->y();
    int w = ui->widget_2->width();
    int h = ui->widget_2->height();
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
void MainWindow::mouseMoveEvent(QMouseEvent *event)
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
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMousePressed = false;//释放鼠标，标志位置为假
}

// 给图片设置圆角
QPixmap MainWindow::getRoundRectPixmap(QPixmap srcPixMap, const QSize & size, int radius)
{
    //不处理空数据或者错误数据
    if (srcPixMap.isNull()) {
        return srcPixMap;
    }

    //获取图片尺寸
    int imageWidth = size.width();
    int imageHeight = size.height();

    //处理大尺寸的图片,保证图片显示区域完整
    QPixmap newPixMap = srcPixMap.scaled(imageWidth, (imageHeight == 0 ? imageWidth : imageHeight),
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap destImage(imageWidth, imageHeight);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);
    // 抗锯齿
    painter.setRenderHints(QPainter::Antialiasing, true);
    // 图片平滑处理
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    // 将图片裁剪为圆角
    QPainterPath path;
    QRect rect(0, 0, imageWidth, imageHeight);
    path.addRoundedRect(rect, radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, imageWidth, imageHeight, newPixMap);
    painter.end();
    return destImage;
}



// 主界面
void MainWindow::on_but_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// 摄像机
void MainWindow::on_but_camera_clicked()
{
    // 打开摄像头
    if(isplay) ui->stackedWidget->setCurrentIndex(1);
    else{
        int ret = callback->play();
        if(ret < 0) QMessageBox::warning(this, tr("No configure"), tr("You have not configured yet."));
        else ui->stackedWidget->setCurrentIndex(1);
        isplay = true;
    }
}

// 后台数据
void MainWindow::on_but_data_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

// 设置
void MainWindow::on_but_set_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief 生成随机点集
 * @param int n    生成点的个数
 * @param int min        范围（最小值）
 * @param int max        范围（最大值）
 * @return QVector<QPointF> data       随机数的点集
 *
 * x值默认int y值默认double
 * x值默认间隔1 y为随机数
 */
QVector<QPointF> MainWindow::randomNumbers(int n, int min, int max)
{
    QVector<QPointF> data;
    for(int i = 0; i < n; i++){
        QPointF p = QPointF(i,QRandomGenerator::global()->bounded(min,max));
        data << p;
    }
    return data;
}

/*
*   横向堆叠柱状图
*   未完成内容
*   1.每个柱子中的4个颜色的排序不同
*   2.柱子的末端为半圆，非直线
*   3.图表和图例的间隔无法调整✔
*/
void MainWindow::setBarChart()
{
    QCustomPlot *customPlot = ui->chart_bar;
    // xy轴 使用yAxis作为柱状图的key轴，xAxis作为value轴
    QCPAxis *keyAxis = customPlot->yAxis; // x轴
    QCPAxis *valueAxis = customPlot->xAxis;
    double size = 0.6; // 柱的宽
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 ;//轴的范围
    labels << "aaa" << "bbb" << "ccc" << "ddd" ;//轴的刻度文字显示
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    keyAxis->setTicker(textTicker);        // 设置为文字轴
    keyAxis->setSubTicks(false);           // 不显示子刻度
    keyAxis->setTickLength(0, 5);          // 轴内外刻度的长度分别是0,5,也就是轴内的刻度线不显示
    keyAxis->setTickPen(QPen(QColor(0,0,0,0)));// 刻度
    keyAxis->setBasePen(QColor(208, 208, 211));// 轴线
    keyAxis->setTickLabelColor(QColor(208, 208, 211));// 刻度文字
    keyAxis->setRange(1-size, 4+size);               // 设置范围
    keyAxis->grid()->setVisible(false);
    valueAxis->setRange(0, 60);
    valueAxis->setVisible(false);
    valueAxis->grid()->setVisible(false);// 网格不可视

    QCPBars *fossil_a = new QCPBars(keyAxis, valueAxis);
    QCPBars *fossil_b = new QCPBars(keyAxis, valueAxis);
    QCPBars *fossil_c = new QCPBars(keyAxis, valueAxis);
    QCPBars *fossil_d = new QCPBars(keyAxis, valueAxis);
    // 随机数据
    QVector<double> fossilData_a, fossilData_b, fossilData_c, fossilData_d;
//    fossilData_a.clear();
//    fossilData_b.clear();
//    fossilData_c.clear();
//    fossilData_d.clear();
    for(int i = 0; i<4; i++){
        QVector<QPointF> data = randomNumbers(4,2,20);
        fossilData_a << data[i].y();
    }
    for(int i = 0; i<4; i++){
        QVector<QPointF> data = randomNumbers(4,2,20);
        fossilData_b << data[i].y();
    }
    for(int i = 0; i<4; i++){
        QVector<QPointF> data = randomNumbers(4,2,20);
        fossilData_c << data[i].y();
    }
    for(int i = 0; i<4; i++){
        QVector<QPointF> data = randomNumbers(4,2,20);
        fossilData_d << data[i].y();
    }
    fossil_a->setData(ticks,fossilData_a);
    fossil_b->setData(ticks,fossilData_b);
    fossil_c->setData(ticks,fossilData_c);
    fossil_d->setData(ticks,fossilData_d);
    // 名字
    fossil_a->setName(QString("Aaaa"));
    fossil_b->setName(QString("Bbbb"));
    fossil_c->setName(QString("Cccc"));
    fossil_d->setName(QString("Dddd"));

    // 颜色
    QColor colors[4] = {QColor(183, 182, 248),QColor(154, 154, 247),
                        QColor(102, 108, 238), QColor(81, 84, 194)};
    fossil_a->setPen(QPen(colors[0]));
    fossil_a->setBrush(QBrush(colors[0]));
    fossil_b->setPen(QPen(colors[1]));
    fossil_b->setBrush(QBrush(colors[1]));
    fossil_c->setPen(QPen(colors[2]));
    fossil_c->setBrush(QBrush(colors[2]));
    fossil_d->setPen(QPen(colors[3]));
    fossil_d->setBrush(QBrush(colors[3]));

    // 设置堆积在其它柱状图上时的间距(像素)
    fossil_b->setStackingGap(-1);
    fossil_c->setStackingGap(-1);
    fossil_d->setStackingGap(-1);
    fossil_b->moveAbove(fossil_a);
    fossil_c->moveAbove(fossil_b);
    fossil_d->moveAbove(fossil_c);


    fossil_a->setWidth(size); // 设置柱状图的宽度大小
    fossil_b->setWidth(size);
    fossil_c->setWidth(size);
    fossil_d->setWidth(size);

    // 图例
    QCPLegend *legend = ui->chart_bar->legend;
    legend->setVisible(true);
    legend->setBrush(QBrush(QColor(0,0,0,0)));
    legend->setBorderPen(QPen(QColor(0,0,0,0)));
    legend->setIconSize(10,10); // 图例图标的大小
    legend->setTextColor(QColor(208, 208, 211));

    // 下述注释方法会导致图表和图例之间有一段距离，无法消除
//    legend->addElement(0,1,legend->item(0));
//    legend->addElement(0,2,legend->item(1));
//    legend->addElement(0,3,legend->item(2));
//    legend->addElement(0,4,legend->item(3));
//    legend->setMargins(QMargins(0,0,100,0));
//    int count = customPlot->plotLayout()->rowCount();
//    customPlot->plotLayout()->addElement(count,0,legend);
//    customPlot->plotLayout()->setRowStretchFactor(count,0.0001);
//    customPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignCenter|Qt::AlignBottom);

    legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
    customPlot->plotLayout()->setMargins(QMargins(0, 0, 0, 20));           // 首先为底部预留出足够的空间
    customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);   // 设置以任意位置放置图例，以对齐边界的方式不行
    customPlot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.0, 1.2, 1.0, 0.1));    // 将y方向的位置设置为超过1以便它的位置超过QCPAxisRect的位置

    customPlot->replot();
}

/*
*   竖向分组柱状图
*   未完成内容
*   1.100%的时候会发现背景的渐变露馅了（可以直接做一个当超过多少的时候直接变成一个颜色
*/
void MainWindow::setBarChart_2()
{
    QCustomPlot *customPlot = ui->chart_bar2;
    // xy轴
    QCPAxis *keyAxis = customPlot->xAxis; // x轴
    QCPAxis *valueAxis = customPlot->yAxis;
    double size = 0.15; // 柱的宽
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7 ;//轴的范围
    labels << "周一" << "周二" << "周三" << "周四" << "周五" << "周六" << "周日" ;//轴的刻度文字显示
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    // x
    textTicker->addTicks(ticks, labels);
    keyAxis->setTicker(textTicker);        // 设置为文字轴
    keyAxis->setSubTicks(false);           // 不显示子刻度
    keyAxis->setTickLength(0, 7);          // 轴内外刻度的长度分别是0,7,也就是轴内的刻度线不显示
    keyAxis->setTickPen(QPen(QColor(0,0,0,0)));// 刻度
    keyAxis->setBasePen(QColor(208, 208, 211));// 轴线
    keyAxis->setTickLabelColor(QColor(208, 208, 211));// 刻度文字
    keyAxis->setRange(1-size*2.5, 7+size*2.5);               // 设置范围
    keyAxis->grid()->setVisible(false);
    // y
    valueAxis->setRange(0,4.5);
    valueAxis->setSubTicks(false);
    valueAxis->setTickLabelColor(QColor(208, 208, 211));// 刻度文字
    valueAxis->setTickPen(QPen(QColor(0,0,0,0)));// 刻度
    valueAxis->setBasePen(QPen(QColor(0,0,0,0)));
    valueAxis->grid()->setPen(QPen(QColor(0, 0, 0,0), 1, Qt::SolidLine));     // 网格线(对应刻度)画笔
    valueAxis->grid()->setPen(QPen(QColor(208, 208, 211), 1, Qt::SolidLine));

    QCPBars *fossil_a = new QCPBars(keyAxis, valueAxis);
    QCPBars *fossil_b = new QCPBars(keyAxis, valueAxis);
    QCPBars *fossil_c = new QCPBars(keyAxis, valueAxis);

    QCPBarsGroup *group = new QCPBarsGroup(customPlot);
    group->setSpacingType(QCPBarsGroup::stAbsolute);  // 设置组内柱状图的间距，按像素
    group->setSpacing(1);     // 设置较小的间距值，这样看起来更紧凑
    fossil_a->setBarsGroup(group);
    fossil_b->setBarsGroup(group);
    fossil_c->setBarsGroup(group);
    // 随机数据
//    fossilData_a.clear();
//    fossilData_b.clear();
//    fossilData_c.clear();
    QVector<double> fossilData_a, fossilData_b, fossilData_c;
    for(int i = 0; i<7; i++){
        QVector<QPointF> data = randomNumbers(7,1,5);
        fossilData_a << data[i].y();
    }
    for(int i = 0; i<7; i++){
        QVector<QPointF> data = randomNumbers(7,1,5);
        fossilData_b << data[i].y();
    }
    for(int i = 0; i<7; i++){
        QVector<QPointF> data = randomNumbers(7,1,5);
        fossilData_c << data[i].y();
    }

    fossil_a->setData(ticks,fossilData_a);
    fossil_b->setData(ticks,fossilData_b);
    fossil_c->setData(ticks,fossilData_c);
    // 名字
    fossil_a->setName(QString("Aaaa"));
    fossil_b->setName(QString("Bbbb"));
    fossil_c->setName(QString("Cccc"));

    // 颜色
    QColor colors[3] = {QColor(245, 139, 89),QColor(244, 180, 144),QColor(244, 203, 193)};
    fossil_a->setPen(QPen(colors[0]));
    fossil_a->setBrush(QBrush(colors[0]));
    fossil_b->setPen(QPen(colors[1]));
    fossil_b->setBrush(QBrush(colors[1]));
    fossil_c->setPen(QPen(colors[2]));
    fossil_c->setBrush(QBrush(colors[2]));

    fossil_a->setWidth(size); // 设置柱状图的宽度大小
    fossil_b->setWidth(size);
    fossil_c->setWidth(size);

    customPlot->replot();
}

// 圆形加载条
void MainWindow::setProgressBar()
{
    double data_1 = QRandomGenerator::global()->bounded(0,50);
    double data_2 = QRandomGenerator::global()->bounded(0,50);
    double percentage = (data_1/(data_1+data_2));//百分比

    int side = qMin(ui->lb_roundprogessbar->width(),ui->lb_roundprogessbar->height());
    QImage background(side,side, QImage::Format_ARGB32_Premultiplied);
    background.fill(Qt::transparent); // 初始化为透明背景
    QPainter painter(&background);
    painter.setRenderHint(QPainter::Antialiasing);
    // 圆的背景
    int roundside = side*0.65;// 背景圆的直径
    int roundpoint = (side-roundside)/2;
    QRect rectbackground(QPoint(roundpoint,roundpoint),QSize(roundside,roundside));// 矩形坐标（x,y,w,h）

    QRadialGradient radialGradient(QPoint(side/2,side/2), roundside/2, QPoint(side/2,side/2));// 中心点，渐变半径，渐变焦点
    radialGradient.setColorAt(0, Qt::black);
    radialGradient.setColorAt(1, Qt::white);
    painter.setBrush(QBrush(radialGradient));
    painter.setPen(QPen(QColor(255, 255, 255,0)));

    painter.drawEllipse(rectbackground);// 圆的背景
    // 圆的中心空心圆
    int roundside_1 = roundside*0.8;// 中心圆的直径
    int roundpoint_1 = (side-roundside_1)/2;// 空心圆的左上点位置
    QRect rectmiddle(QPoint(roundpoint_1,roundpoint_1),QSize(roundside_1,roundside_1));
    painter.setPen(QPen(QColor(255, 255, 255,0)));
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawEllipse(rectmiddle);

    // 圆的进度条
    int r = (roundside/2 - roundside_1/2)/2;// 小圆半径
    int radius = roundside_1/2 + r;// 所在的圆的路经上的半径
    qreal startAngle = 270;// 开始角度
    QPoint startPoint (side/2-r,side/2+radius-r);// 开始左上点
    QRect startRect(startPoint,QSize(r*2,r*2));// 开始的矩形框

    const double Angle =270 - percentage * 360; // 角度
    const double pi = 3.14159265358979323846; // 圆周率π
    double AngleInRadians = Angle * (pi / 180.0);
    double x = cos(AngleInRadians)*radius;
    double y = sin(AngleInRadians)*radius;
    QPoint endPoint = QPoint(side/2+x-r,side/2-y-r);
    QRect endRect(endPoint,QSize(r*2,r*2-1));
    qreal endAngle = startAngle - Angle;// 结束角度

    painter.setPen(QPen(QColor(0, 0, 0,0),1));
    QConicalGradient gradient(QPointF(side/2,side/2),270+7);// 设置渐变颜色
    gradient.setColorAt(0, QColor(80, 85, 193));     // 开始颜色
    gradient.setColorAt(1, QColor(211, 212, 239));      // 结束颜色
    painter.setBrush(QBrush(gradient));

    QPainterPath path;
    path.moveTo(startPoint.x()+r,startPoint.y()+r*2);
    path.arcTo(startRect,270,180);// 开始的半圆
    path.arcTo(rectmiddle,startAngle, -endAngle); // 内侧圆弧
    path.arcTo(endRect,Angle+180,180);// 结束的半圆
    path.arcTo(rectbackground,Angle, 180+90-Angle); // 外侧圆弧
    path.closeSubpath(); // 封闭路径
    painter.drawPath(path);

    // 中间百分比
    painter.setPen(QPen(QColor(0, 0, 0, 150),1));
    painter.setFont(QFont ("Arial", 27));
    painter.drawText(QPointF(side/2-40,side/2+18),QString("%1%").arg(QString::number(percentage*100,'f',0)));
    painter.end(); // 结束绘制
    // 放置
    QPixmap pixmap = QPixmap::fromImage(background);
    ui->lb_roundprogessbar->setPixmap(pixmap);
}

// 饼状图
//
void MainWindow::setPieChart()
{
    QChart* chart = new QChart();
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.7);//空心大小
    series->setPieSize(1);
    series->setPieStartAngle(90+45);
    series->setPieEndAngle(360+90+45);

    double data_1 = QRandomGenerator::global()->bounded(0,50);
    double data_2 = QRandomGenerator::global()->bounded(0,50);
    double percentage = (data_1/(data_1+data_2))*100;//百分比
    QPieSlice *slice2 = series->append("%", data_2);
    slice2->setPen(QPen(QColor(255,255,255),3));
    slice2->setBrush(QColor(245, 139, 89) );

    QPieSlice *slice1 = series->append("%", data_1);
    slice1->setPen(QPen(QColor(255,255,255),3));
    slice1->setBrush(QColor(80, 85, 193));

    // 中间百分比
    int side = qMin(ui->lb_piedata->width(),ui->lb_piedata->height());
    double side_min = sqrt((side/2)*(side/2));
    QImage background(side,side, QImage::Format_ARGB32_Premultiplied);
    background.fill(Qt::transparent); // 初始化为透明背景
    QPainter painter(&background);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont percentage_font = QFont ("Arial",12,QFont::Bold);// 百分比的字体
    QFont label_font = QFont ("Arial",7,QFont::Bold);// 百分比的字体
    QFontMetrics metrics(percentage_font);
    QFontMetrics metrics2(label_font);
    int percentage_textH = metrics.height();
    int label_textH = metrics2.height();

    QRect percentage_rect = QRect(QPoint(side_min/4,side_min/4+10),QSize((side-side_min)/2+side_min,(side-side_min)/2+side_min-20));
    QRect label_min = QRect(QPoint(side_min/4+10,side_min/4+percentage_textH+5),QSize((side-side_min)/2+side_min, percentage_textH*2+label_textH-15));

    // 百分比
    painter.setFont(percentage_font);
    painter.setPen(QPen(QColor(80, 85, 193)));// 紫色
    painter.drawText(percentage_rect, Qt::AlignTop | Qt::AlignRight, QString("%1%").arg(QString::number(percentage,'f',0)));
    painter.setPen(QPen(QColor(245, 139, 89)));// 橙色
    painter.drawText(percentage_rect, Qt::AlignBottom | Qt::AlignLeft, QString("%1%").arg(QString::number(100-percentage,'f',0)));

    painter.setPen(QPen(QColor(208, 208, 211),2));
    painter.setFont(QFont ("Arial",7,QFont::Bold));
    painter.drawText(label_min, Qt::AlignTop | Qt::AlignRight, QString("online"));
    painter.drawText(label_min, Qt::AlignBottom | Qt::AlignLeft, QString("unline"));

    painter.drawLine(QPoint(side/4,side/4),QPoint(side/4*3,side/4*3));// 斜线

    painter.end(); // 结束绘制
    QPixmap pixmap = QPixmap::fromImage(background);
    ui->lb_piedata->setPixmap(pixmap);

    chart->addSeries(series);

    // 图例
    chart->legend()->setVisible(false);
    ui->chart_pie->setRenderHint(QPainter::Antialiasing);
    ui->chart_pie->setChart(chart);
}

// 折线图
void MainWindow::setLineChart()
{

}

// 曲线图
void MainWindow::setSplineChart()
{
    QCustomPlot *customPlot = ui->chart_spline;
    // xy轴
    QCPAxis *keyAxis = customPlot->xAxis;
    QCPAxis *valueAxis = customPlot->yAxis;
    keyAxis->setRange(0, 12);// 设置坐标轴的范围，以看到所有数据
    keyAxis->setVisible(false);// 隐藏x轴
    keyAxis->grid()->setPen(QPen(QColor(0, 0, 0,0), 1, Qt::SolidLine));     // 网格线(对应刻度)画笔
    keyAxis->grid()->setSubGridVisible(false);     // 显示子网格线

    valueAxis->setRange(0, 30);
    valueAxis->ticker()->setTickCount(4); // 设置刻度数量
    valueAxis->setBasePen(QPen(QColor(0, 0, 0, 0), 1));  // 轴线的画笔
    valueAxis->setTickPen(QPen(QColor(0, 0, 0, 0), 1));  // 轴刻度线的画笔
    valueAxis->setSubTickPen(QPen(QColor(0, 0, 0, 0), 1)); // 轴 子刻度线的画笔
    valueAxis->setTickLabelColor(QColor(208, 208, 211));  // 轴刻度文字颜色
    valueAxis->grid()->setPen(QPen(QColor(208, 208, 211), 1, Qt::SolidLine));
    valueAxis->grid()->setSubGridVisible(false);

    // 蓝线
    QVector<QPointF> data0 = randomNumbers(25,10,35);// 生成随机点集
    QVector<QPointF> firstControlPoints, secondControlPoints;// 计算两个控制点
    calculateControlPoints(data0, &firstControlPoints, &secondControlPoints);
    QVector<QPointF> spline_data0;// 生成塞贝尔曲线
    double t_end = 1.0;
    double step = 0.01;
    for(int i = 0; i < data0.size() - 1; i++){
        for(double t = 0; t <= t_end; t = t + step ){
            QPointF p = splineSmooth(data0[i],firstControlPoints[i],secondControlPoints[i],data0[i+1],t);
            spline_data0 << p;
        }
    }
    firstControlPoints.clear();secondControlPoints.clear();
    QVector<double> x_data0(spline_data0.size()), y_data0(spline_data0.size());// 塞贝尔曲线数据点
    for (int i=0; i<spline_data0.size(); ++i){
      x_data0[i] = spline_data0[i].x() ; // x 轴数据点
      y_data0[i] = spline_data0[i].y() ; // y 轴数据点
    }
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom | Qt::AlignRight);
    customPlot->addGraph(keyAxis,valueAxis);
    customPlot->graph(0)->setData(x_data0, y_data0);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setPen(QPen(QColor(82, 85, 193),3));
    // 生成渐变色纹理图
    QImage image(500,500, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent); // 初始化为透明背景
    QPainter painter(&image);
    QLinearGradient gradient(0, 0, 0, 500);// 设置渐变颜色
    gradient.setColorAt(0, QColor(82, 85, 193, 100));     // 开始颜色
    gradient.setColorAt(0.5, QColor(255, 255, 255, 200));      // 结束颜色
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen); // 不绘制边框
    painter.drawRect(0, 0, 500, 500); // 绘制渐变矩形
    painter.end(); // 结束绘制
    customPlot->graph(0)->setBrush(QBrush(QPixmap::fromImage(image)));// 填充纹理

    // 橙线
    QVector<QPointF> data1 = randomNumbers(25,10,35);// 生成随机点集
    QVector<double> x_data1(data1.size()), y_data1(data1.size());
    for (int i=0; i<data1.size(); ++i){
      x_data1[i] = data1[i].x() ; // x 轴数据点
      y_data1[i] = data1[i].y() ; // y 轴数据点
    }
    customPlot->addGraph();
    customPlot->graph(1)->setData(x_data1,y_data1);
    customPlot->graph(1)->setSmooth(true);
    customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(1)->setPen(QPen(QColor(245, 139, 91),3));

    // 点
    QVector<double> oo1_x;
    QVector<double> oo1_y;
    QVector<double> oo2_x;
    QVector<double> oo2_y;

    oo1_x << data0[2].x();
    oo1_x << data0[5].x();
    oo1_y << data0[2].y();
    oo1_y << data0[5].y();
    oo2_x << x_data1[1];
    oo2_x << x_data1[3];
    oo2_y << y_data1[1];
    oo2_y << y_data1[3];
    // 蓝色的点
    customPlot->addGraph();
    customPlot->graph(2)->setData(oo1_x,oo1_y);
    customPlot->graph(2)->setPen(QPen(QColor(82, 85, 193),3));
    customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,8));
    customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
    // 橘色的点
    customPlot->addGraph();
    customPlot->graph(3)->setData(oo2_x,oo2_y);
    customPlot->graph(3)->setPen(QPen(QColor(245, 139, 91),3));
    customPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,8));
    customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);

    // 图例
    QCPLegend *legend = customPlot->legend;
    legend->setVisible(true);
    legend->removeItem(2);
    legend->removeItem(2);
    legend->setBrush(*(new QBrush(QColor(0,0,0,0))));
    legend->setBorderPen(QPen(QColor(0,0,0,0)));
    legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
    customPlot->plotLayout()->setMargins(QMargins(0, 0, 0, 20));           // 首先为底部预留出足够的空间
    customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);   // 设置以任意位置放置图例，以对齐边界的方式不行
    customPlot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.0, 1.2, 1.0, 0.1));    // 将y方向的位置设置为超过1以便它的位置超过QCPAxisRect的位置

    customPlot->replot();// 重画图像
}

// 计算贝塞尔曲线的所有控制点
void MainWindow::calculateControlPoints(const QVector<QPointF> &knots,
                                       QVector<QPointF> *firstControlPoints,
                                       QVector<QPointF> *secondControlPoints)
{
        int n = knots.size() - 1;

        firstControlPoints->reserve(n);
        secondControlPoints->reserve(n);

        for (int i = 0; i < n; ++i) {
            firstControlPoints->append(QPointF());
            secondControlPoints->append(QPointF());
        }

        if (n == 1) {
            // Special case: Bezier curve should be a straight line.
            // P1 = (2P0 + P3) / 3
            (*firstControlPoints)[0].rx() = (2 * knots[0].x() + knots[1].x()) / 3;
            (*firstControlPoints)[0].ry() = (2 * knots[0].y() + knots[1].y()) / 3;

            // P2 = 2P1 – P0
            (*secondControlPoints)[0].rx() = 2 * (*firstControlPoints)[0].x() - knots[0].x();
            (*secondControlPoints)[0].ry() = 2 * (*firstControlPoints)[0].y() - knots[0].y();

            return;
        }

        // Calculate first Bezier control points
        double *xs = nullptr;
        double *ys = nullptr;
        double *rhsx = new double[n]; // Right hand side vector
        double *rhsy = new double[n]; // Right hand side vector

        // Set right hand side values
        for (int i = 1; i < n - 1; ++i) {
            rhsx[i] = 4 * knots[i].x() + 2 * knots[i + 1].x();
            rhsy[i] = 4 * knots[i].y() + 2 * knots[i + 1].y();
        }
        rhsx[0] = knots[0].x() + 2 * knots[1].x();
        rhsx[n - 1] = (8 * knots[n - 1].x() + knots[n].x()) / 2.0;
        rhsy[0] = knots[0].y() + 2 * knots[1].y();
        rhsy[n - 1] = (8 * knots[n - 1].y() + knots[n].y()) / 2.0;

        // Calculate first control points coordinates
        calculateFirstControlPoints(xs, rhsx, n);
        calculateFirstControlPoints(ys, rhsy, n);

        // Fill output control points.
        for (int i = 0; i < n; ++i) {
            (*firstControlPoints)[i].rx() = xs[i];
            (*firstControlPoints)[i].ry() = ys[i];

            if (i < n - 1) {
                (*secondControlPoints)[i].rx() = 2 * knots[i + 1].x() - xs[i + 1];
                (*secondControlPoints)[i].ry() = 2 * knots[i + 1].y() - ys[i + 1];
            } else {
                (*secondControlPoints)[i].rx() = (knots[n].x() + xs[n - 1]) / 2;
                (*secondControlPoints)[i].ry() = (knots[n].y() + ys[n - 1]) / 2;
            }
        }

        delete xs;
        delete ys;
        delete[] rhsx;
        delete[] rhsy;
    }

// 一个静态方法，用于计算贝塞尔曲线的所有控制点。
void MainWindow::calculateFirstControlPoints(double *&result, const double *rhs, int n) {
        result = new double[n];
        double *tmp = new double[n];
        double b = 2.0;
        result[0] = rhs[0] / b;

        // Decomposition and forward substitution.
        for (int i = 1; i < n; i++) {
            tmp[i] = 1 / b;
            b = (i < n - 1 ? 4.0 : 3.5) - tmp[i];
            result[i] = (rhs[i] - result[i - 1]) / b;
        }

        for (int i = 1; i < n; i++) {
            result[n - i - 1] -= tmp[n - i] * result[n - i]; // Backsubstitution.
        }

        delete[] tmp;
    }
//  三次贝塞尔曲线的计算函数
QPointF MainWindow::splineSmooth(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, double t)
{
    double tx = pow(1 - t, 3) * P0.x() + 3 * pow(1 - t, 2) * t * P1.x() + 3 * (1 - t) * pow(t, 2) * P2.x() + pow(t, 3) *P3.x();
    double ty = pow(1 - t, 3) * P0.y() + 3 * pow(1 - t, 2) * t * P1.y() + 3 * (1 - t) * pow(t, 2) * P2.y() + pow(t, 3) *P3.y();
    return QPointF(tx, ty);
}

// 最大化
void MainWindow::on_but_max_clicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

// 最小化
void MainWindow::on_but_min_clicked()
{
    showMinimized();
}

// 关闭窗口
void MainWindow::on_but_off_clicked()
{
    // QApplication::quit();//立即终止应用程序
    // close(); // 关闭窗口
    MainWindow::close();
}

// use to test
// sth bug:
// 1.重复生成数据，导致无限增大
// 2.圆形加载调的头部露馅
// unfinish:
// 自定义日历
// 渐变曲线*2
// 数据封装
// 布局设置
// ↑↑↑↑↑↑ allow just page1 ↑↑↑↑↑↑
void MainWindow::on_pushButton_clicked()
{
    setBarChart();
    setBarChart_2();
    setProgressBar();
    setPieChart();
    setLineChart();
    setSplineChart();
}

// 录入人脸信息
void MainWindow::on_but_enterface_clicked()
{
    ui_enterface->setCallback([this]() {
        if (callback) {
            callback->resume();
        }
    });
    // 这里的yolo就会有isLoaded
    if(!yolo->isLoaded){// 模型未加载
        QMessageBox::warning(this, tr("Model error:"), 
            tr("Model not found or loaded failed. \n Please enter the button to load the model and try again."));
        MYLOG << "Fail to load model";
        return;
    } else {
        callback->pasue();
        ui_enterface->setWindowModality(Qt::ApplicationModal);
        ui_enterface->show();
        MYLOG << "Model loaded successfully";
    }

}

// ---------------- HJJ --------------- //
void MainWindow::on_but_sure_clicked()
{
    try { // 加载facenet模型
        bool ret = facenet->loadModel(ui->le_facenetonnx->text());// 加载facenet模型
        if (!ret)
        {
            std::cerr << "model path is empty!" << std::endl;
            return;
        }
    }
    catch(const Ort::Exception& e)
    {
        QMessageBox::critical(nullptr, "Facenet error", e.what());
        return;
    }
	std::string rtsp_url = ui->le_rtsp->text().toStdString();
	std::string modelPath = ui->le_onnx->text().toStdString(); // yolo 模型路径
	int ret = callback->ensureEnter(modelPath, rtsp_url);
	if(ret < 0){
        QMessageBox::critical(this, tr("Init error:"), tr("Fail to configure"));
        MYLOG << "Slot error: ";
		MYLOG << "Fail to created thread";
	} else {
        QMessageBox::information(this, tr("Init success:"), tr("Successfully configured"));
        isplay = false;
        //ui->stackedWidget->setCurrentWidget(ui->page_2);
    }
}

// 选择YOLO模型文件
void MainWindow::on_but_onnx_clicked()
{
	// 打开文件对话框
	QString fileName = QFileDialog::getOpenFileName(
		this,                           // 父窗口
		tr("Yolo model file"),           // 对话框标题
		// "../../",                             // 默认路径
        ":/",                             // 默认路径
		tr("ONNX Files (*.onnx);;All Files (*)") // 文件过滤器
	);
	// 检查用户是否选择了文件
	if (!fileName.isEmpty()) {
		ui->le_onnx->setText(fileName);
	} else {
		// 如果用户没有选择文件，可以处理这种情况
		QMessageBox::warning(this, tr("No File Selected"), tr("No ONNX file was selected."));
	}
}


// 选择facenet模型文件
void MainWindow::on_but_facenetonnx_clicked()
{
	// 打开文件对话框
	QString fileName = QFileDialog::getOpenFileName(
		this,                           // 父窗口
		tr("Facenet model file"),           // 对话框标题
		":/UI/Resources",                             // 默认路径
		tr("ONNX Files (*.onnx);;All Files (*)") // 文件过滤器
	);
	// 检查用户是否选择了文件
	if (!fileName.isEmpty()) {
		ui->le_facenetonnx->setText(fileName);
	} else {
		// 如果用户没有选择文件，可以处理这种情况
		QMessageBox::warning(this, tr("No File Selected"), tr("No ONNX file was selected."));
	}
}

// 数据库配置
void MainWindow::on_but_storagefile_clicked()
{
    // 打开文件对话框
    Configure conf;
    conf.databaseConfigure();
}



