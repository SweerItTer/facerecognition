#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QStyleOption>
#include <QMessageBox>

#include <vector>

#include "./Sql/facedatabase.h"

class FaceDatabase;

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr,FaceDatabase *database = nullptr); // 修正构造函数名称
    ~Login(); // 修正析构函数名称

    void InitStyle();

    void setCallback(std::function<void()> callback_){
        callback = callback_;
    }



private slots:
    // 重绘窗口
    virtual void paintEvent(QPaintEvent *event) override; 
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    // 关闭窗口
    void on_but_off_clicked(); // 声明槽函数
    // 登录&注册
    void on_but_login_clicked(); // 登录
    void on_but_toRegister_clicked(); // 去注册
    void on_but_register_clicked(); // 注册账号
    void on_but_toLogin_clicked(); // 去登录
    
private:
    Ui::Login *ui; // 修正UI类的指针类型
    bool m_leftMousePressed = false; //鼠标是否点中标题栏，是true，否false
    QPoint m_StartPoint ; // 窗口的全局位置

    FaceDatabase *database_ = nullptr;

    // 关闭事件
    void closeEvent(QCloseEvent *event){
        callback(); // 调用回调函数(继续线程)
        event->accept();
    }
    std::function<void()> callback; // 无返回,无参数

signals:
    void loginSignal(std::string account); // 登录信号

};

#endif // LOGIN_H
