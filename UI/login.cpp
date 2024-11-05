#include "login.h"
#include "ui_login.h"
#pragma execution_character_set("UTF-8")
#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

Login::Login(QWidget *parent) 
    : QWidget(parent)
    , ui(new Ui::Login)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    InitStyle();
}

Login::~Login()
{
    delete ui;
}


// 样式初始化
void Login::InitStyle()
{
    // 窗口
    this->setWindowFlags(Qt::FramelessWindowHint);      //隐藏最大最小化等按键
    this->setAttribute(Qt::WA_TranslucentBackground);         //设置窗口透明化
    this->setStyleSheet(QString("QWidget{"
                                 "background-color:rgb(236, 237, 255);"
                                 "border-radius: 10px;"
                                 "border-style:solid;"
                                 "border-width:2px;"
                                 "border-color:rgba(0, 0, 0, 1);"
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

    // 主界面样式
    ui->widget_2->setStyleSheet(QString("QLineEdit{"        //输入框样式
                                        "min-height: 35px;"
                                        "background-color: rgba(255, 255, 255, 0.8);"
                                        "border-radius:10px;"
                                        "border-style:solid;"
                                        "border-width:1px;"
                                        "border-color:rgb(190, 190, 190);"
                                        "font-size:18px;"
                                        "color: rgb(0, 0, 0);"
                                        "}"
                                        "QPushButton{"    //按钮样式
                                        "min-height: 35px;"
                                        "background-color: rgb(82, 85, 193);"
                                        "border-radius:10px;"
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
                                        "}" 
                                        "QPushButton:disabled{"
                                        "background-color: rgba(82, 85, 193, 0.5);"
                                        "color: rgba(255, 255, 255, 0.5);"
                                        "}"));
    // 输入框
    ui->le_ID->setPlaceholderText(tr("Enter account"));
    ui->le_ID->setAlignment(Qt::AlignCenter); // 居中显示

    ui->le_password->setPlaceholderText(tr("Enter password"));
    ui->le_password->setAlignment(Qt::AlignCenter); // 居中显示
    ui->le_password->setEchoMode(QLineEdit::Password); //密码输入模式

    ui->but_register->setStyleSheet(QString("QPushButton{"
                                            "background-color: rgba(0, 0, 0, 0);"
                                            "border-style:none;"
                                            "font-size:18px;"
                                            "color: rgb(0, 102, 204);"
                                            "}" ));
}

// 重绘窗口
void Login::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

// 监控鼠标点击的位置
void Login::mousePressEvent(QMouseEvent *event)
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
void Login::mouseMoveEvent(QMouseEvent *event)
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
void Login::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMousePressed = false;//释放鼠标，标志位置为假
}

void Login::on_but_off_clicked()
{
    Login::close();
}

