#include "enterface.h"
#include "ui_enterface.h"

enterface::enterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::enterface)
{
    ui->setupUi(this);

    page = ui->stackedWidget->currentIndex();
    StackedWInit();
    
}

enterface::~enterface()
{
    delete ui;
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