#include "script.h"
#include "./UI/mainwindow.h"
#include "./UI/ui_mainwindow.h"

#include <QObject>
#include <QPixmap>
#include <QSize>

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
	if ( is_rtspurl ) {
		if (p_thread) {
			p_thread->Stop();
			while(!p_thread->isFinished()){
				std::cout << "Waitting...." << std::endl;
			}
			delete p_thread; // 释放线程
		}

		p_thread = new PlayThread(rtsp_url);
		std::cout << "PlayThread created." << std::endl;
		p_thread->Start();
		QObject::connect(p_thread, &PlayThread::framesSignal, this, [this]( QPixmap image ) {
			// 按 label 大小等比例缩放图像
			QSize labelSize = mw->ui->lb_camera->size();
			// 设置缩放后的图像
			mw->ui->lb_camera->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		});

		return 0;
	} else {
		return -1;
	}
	return 0;
}
