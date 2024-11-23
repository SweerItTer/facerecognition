#include "login.h"
#include "ui_login.h"
#pragma execution_character_set("UTF-8")
#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

Login::Login(QWidget *parent,FaceDatabase *database) 
    : QWidget(parent)
    , ui(new Ui::Login)
    , database_(database)
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
    ui->but_off->setIconSize(QSize(20, 20)); // 设置图标的大小

    // 主界面样式
    // 主体框
    ui->widget_2->setStyleSheet(QString("QWidget{"
                                        "background-color: rgb(236, 237, 255);"
                                        "border-top-left-radius:0px;"
                                        "border-top-right-radius:0px;"
                                        "border-bottom-right-radius:10px;"
                                        "border-bottom-left-radius:10px;"
                                        "border-style:solid;"
                                        "border-width:1px;"
                                        "border-color:rgb(82, 85, 193);"
                                        "}"));
    ui->page_2->setStyleSheet(QString("QWidget{"
                                        "background-color: rgba(236, 237, 255, 0);"
                                        "border-radius:0px;"
                                        "border-style:none;"
                                        "border-width:0px;"
                                        "border-color:rgba(82, 85, 193, 0);"
                                        "}"));
    // 头像
    ui->lb_head->setStyleSheet(QString("QLabel{"
                                    "background-color: rgba(0, 0, 0, 0);" 
                                    "border-radius:0px;"
                                    "border-style:none;" 
                                    "border-width:0px;"
                                    "border-color:rgba(0, 0, 0, 0);"
                                    "color: rgba(0, 0, 0, 1);"
                                    "}"));
    QPixmap headPix = QPixmap(":/user1.png");
    headPix = headPix.scaled(ui->lb_head->size(), Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->lb_head->setPixmap(headPix);
    // 欢迎注册
    ui->lb_welcome->setStyleSheet(QString("QLabel{"
                                        "background-color: rgba(0, 0, 0, 0);"
                                        "border-radius:0px;"
                                        "border-style:none;"
                                        "border-width:0px;"
                                        "border-color:rgba(0, 0, 0, 0);"
                                        "font-size:40px;"
                                        "color: rgb(82, 85, 193);"
                                        "}"));
    // 输入框
    QString QLineEditStyle =QString("QLineEdit{"        
                                    "min-height: 35px;"
                                    "background-color: rgba(255, 255, 255, 0.8);"
                                    "border-radius:10px;"
                                    "border-style:solid;"
                                    "border-width:1px;"
                                    "border-color:rgb(190, 190, 190);"
                                    "font-size:18px;"
                                    "color: rgb(0, 0, 0);"
                                    "}");
    ui->le_loginID->setStyleSheet(QLineEditStyle);
    ui->le_loginPW->setStyleSheet(QLineEditStyle);
    ui->le_registerID->setStyleSheet(QLineEditStyle);
    ui->le_registerPW->setStyleSheet(QLineEditStyle);

    ui->le_loginID->setPlaceholderText(tr("Enter account"));
    ui->le_loginPW->setPlaceholderText(tr("Enter password"));
    ui->le_registerID->setPlaceholderText(tr("Enter account"));
    ui->le_registerPW->setPlaceholderText(tr("Enter password"));

    ui->le_loginID->setAlignment(Qt::AlignCenter); // 居中显示
    ui->le_loginPW->setAlignment(Qt::AlignCenter); // 居中显示
    ui->le_loginPW->setEchoMode(QLineEdit::Password); //密码输入模式
    ui->le_registerID->setAlignment(Qt::AlignCenter); // 居中显示
    ui->le_registerPW->setAlignment(Qt::AlignCenter); // 居中显示
    
    // 登录按钮
    QString QPushButtonStyle = QString( "QPushButton{"    //按钮样式
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
                                        "}");
    ui->but_login->setStyleSheet(QPushButtonStyle);
    ui->but_register->setStyleSheet(QPushButtonStyle);

    // 界面切换按钮
    ui->but_toRegister->setStyleSheet(QString("QPushButton{"
                                            "background-color: rgba(0, 0, 0, 0);"
                                            "border-style:none;"
                                            "font-size:18px;"
                                            "color: rgb(0, 102, 204);"
                                            "}" ));
    ui->but_toLogin->setStyleSheet(QString("QPushButton{"
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

// 登录
void Login::on_but_login_clicked()
{
    std::string account = ui->le_loginID->text().toStdString();
    std::string password = ui->le_loginPW->text().toStdString();
    if(account.empty() || password.empty()){   
        MYLOG << "account or password cannot be empty!";
        QMessageBox::warning(this, tr("Warning"), tr("Account or password cannot be empty!"));
        return;
    }
    if(database_->checkUserAccount(account) == true){
        MYLOG << "Account does not exist!";
        QMessageBox::warning(this, tr("Warning"), tr("Account does not exist!"));
        return;
    }
    if(database_->checkUserPassword(account, password) == false){
        MYLOG << "Login failed!";
        QMessageBox::information(this, tr("Warning"), tr("Login failed!"));
        return;
    }else{
        MYLOG << "Login successful!";
        QMessageBox::information(this, tr("Information"), tr("Login successful!"));
        emit loginSignal(account);
        MYLOG << "account";
        on_but_off_clicked();
    }
}
// 注册
void Login::on_but_toRegister_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// 注册账号
void Login::on_but_register_clicked()
{
    std::string account = ui->le_registerID->text().toStdString();
    std::string password = ui->le_registerPW->text().toStdString();
    if(account.empty() || password.empty()){   
        MYLOG << "account or password cannot be empty!";
        QMessageBox::warning(this, tr("Warning"), tr("Account or password cannot be empty!"));
        return;
    }
    if(database_->checkUserAccount(account) == false){
        MYLOG << "Account already exists!";
        QMessageBox::warning(this, tr("Warning"), tr("Account already exists!"));
        return;
    }
    int ret = database_->insertUserAccount(account, password);
    if(ret != 0){
        MYLOG << "Registration successful!";
        QMessageBox::information(this, tr("Information"), tr("Registration successful!"));
        ui->stackedWidget->setCurrentIndex(0);
    }else{
        MYLOG << "Registration failed!";
        QMessageBox::warning(this, tr("Warning"), tr("Registration failed!"));
    }
    ui->stackedWidget->setCurrentIndex(0);
}

// 去登录
void Login::on_but_toLogin_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
