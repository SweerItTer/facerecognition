#include "script.h"
<<<<<<< HEAD
#include "./UI/mainwindow.h"
#include "./UI/ui_mainwindow.h"

#include <QObject>
#include <QPixmap>
#include <QSize>
=======
>>>>>>> 9f4521b430aaffd1280eafd7cda72292bff90fc8

Script::~Script()
{
	if (p_thread) {
		p_thread->Stop();
		delete p_thread; // 释放线程
	}
	std::cout << "player close.\n";
}

int Script::ensureEnter(std::string rtsp_url){
	bool is_rtspurl = string_compare(rtsp_url, "rtsp://");
<<<<<<< HEAD
	if ( is_rtspurl ) {
		if (p_thread) {
			p_thread->Stop();
			while(!p_thread->isFinished()){
				std::cout << "Waitting...." << std::endl;
=======
	if ( is_rtspurl ) {//|| !is_rtspurl) {
		/*if (p_thread) {
			p_thread->Stop();
			while(!p_thread->isFinished()){
				std::cout << "Waitting....";
>>>>>>> 9f4521b430aaffd1280eafd7cda72292bff90fc8
			}
			delete p_thread; // 释放线程
		}

<<<<<<< HEAD
		p_thread = new PlayThread(rtsp_url);
		std::cout << "PlayThread created." << std::endl;
		p_thread->Start();
		QObject::connect(p_thread, &PlayThread::framesSignal, this, [this]( QPixmap image ) {
			// 按 label 大小等比例缩放图像
			QSize labelSize = mw->ui->lb_camera->size();
			// 设置缩放后的图像
			mw->ui->lb_camera->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		});

=======
	//	p_thread = new PlayThread(rtsp_url);
		std::cout << "PlayThread created.";
		connect(p_thread, &PlayThread::framesSignal, this, [this]( QPixmap image ) {
			// 按 label 大小等比例缩放图像
			labelSize = ui->label->size();
			// 设置缩放后的图像
			ui->label->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		});*/
		std::cout << rtsp_url <<std::endl;
>>>>>>> 9f4521b430aaffd1280eafd7cda72292bff90fc8
		return 0;
	} else {
		return -1;
	}
	return 0;
}
