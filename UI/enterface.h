#ifndef ENTERFACE_H
#define ENTERFACE_H

#include <QWidget>

namespace Ui {
class enterface;
}

class enterface : public QWidget
{
    Q_OBJECT

public:
    explicit enterface(QWidget *parent = nullptr);
    ~enterface();

private:
    Ui::enterface *ui;
    int page = 0;

private slots:
    void on_but_data_clicked(); 
    void on_but_face_clicked(); 
    void on_but_finish_clicked(); 
    void on_but_verify_clicked(); 

    void on_but_back_clicked(); // back
    void on_but_next_clicked(); // next

    void StackedWInit(); //堆叠容器初始化
};

#endif // ENTERFACE_H
