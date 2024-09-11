#include "script.h"

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
	if ( is_rtspurl ) {//|| !is_rtspurl) {
		/*if (p_thread) {
			p_thread->Stop();
			while(!p_thread->isFinished()){
				std::cout << "Waitting....";
			}
			delete p_thread; // 释放线程
		}

	//	p_thread = new PlayThread(rtsp_url);
		std::cout << "PlayThread created.";
		connect(p_thread, &PlayThread::framesSignal, this, [this]( QPixmap image ) {
			// 按 label 大小等比例缩放图像
			labelSize = ui->label->size();
			// 设置缩放后的图像
			ui->label->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		});*/
		std::cout << rtsp_url <<std::endl;
		return 0;
	} else {
		return -1;
	}
	return 0;
}
