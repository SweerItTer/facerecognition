#include "player.h"
#include "./ui_player.h"
#include <QDebug>


Player::Player(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::Player)
{
	ui->setupUi(this);
	labelSize = ui->label->size();
	Signal();
}

Player::~Player()
{
	delete ui;
	if (p_thread) {
		p_thread->Stop();
		delete p_thread; // 释放线程
	}
	qDebug() << "player close.\n";
}

void Player::Signal(){
	connect(ui->lineEdit, &LineEdit::Confirm, [this](std::string str) {
		bool is_rtspurl = string_compare(str, "rtsp://");
		if ( is_rtspurl ) {//|| !is_rtspurl) {
			if (p_thread) {
				p_thread->Stop();
				while(!p_thread->isFinished()){
					qDebug() << "Waitting....";
				}
				delete p_thread; // 释放线程
			}

			p_thread = new PlayThread(str);
			qDebug() << "PlayThread created.";
			connect(p_thread, &PlayThread::framesSignal, this, [this]( QPixmap image ) {
				// 按 label 大小等比例缩放图像
				labelSize = ui->label->size();
				// 设置缩放后的图像
				ui->label->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
			});

			p_thread->Start(); // 启动线程
		}
	});
}

void Player::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
}
