#include "playthread.h"
#include <QDebug>

PlayThread::PlayThread(QObject *parent) : QThread(parent){}
PlayThread::PlayThread(std::string str) : url_(str){}

PlayThread::~PlayThread(){
	Stop();
	if(demux_thread_network){//退出线程
		demux_thread_network->Stop();
		cout << "demux_thread_network stop\n";
		delete demux_thread_network;
	} else if(demux_thread){
		demux_thread->Stop();
		cout << "demux_thread stop\n";
		delete demux_thread;
	} else {
		delete demux_thread_network;
		delete demux_thread;
	}

	video_decode_thread->Stop();
	cout << "video_decode_thread stop\n";
	delete video_decode_thread;
}

void PlayThread::Start()
{
	abord_ = false;
	paused = false;
	QThread::start();
}

void PlayThread::Stop()
{
	if (this->isRunning()) {
		paused = false;
		abord_ = true;
		QThread::quit(); // 请求线程退出
		QThread::wait(); // 等待线程退出
		qDebug() << "Player thread stop.\n";
	}
}

void PlayThread::Pause(int i){
	paused = (i == 1);
}

void PlayThread::run(){
	qDebug() << "PlayThread started.\n" << QString::fromStdString(url_);
	int ret = 0;
	bool isStream_;

	//读取文件获取数据包
	AVPacketQueue audio_packet_queue;
	AVPacketQueue video_packet_queue;

//解复用
	isStream_ = isStream(url_);
	if(isStream_){//流媒体
		demux_thread_network = new DemuxThread_network(&audio_packet_queue,
																	&video_packet_queue);
		ret = demux_thread_network->Init(url_.c_str());
		if(ret < 0){
			cout << "demux_thread init falied\n";
			return;
		}
		ret = demux_thread_network->Start();//启动!!!!!!
		if(ret < 0){
			cout << "demux_thread start falied\n";
			return ;
		}
	} else {//非流媒体
		demux_thread = new DemuxThread(&audio_packet_queue, &video_packet_queue);
		ret = demux_thread->Init(url_.c_str());
		if(ret < 0){
			cout << "demux_thread init falied\n";
			return;
		}
		ret = demux_thread->Start();
		if(ret < 0){
			cout << "demux_thread start falied\n";
			return ;
		}
	}
//解码
	AVFrameQueue video_frame_queue;
	video_decode_thread = new DecodeThread(&video_packet_queue, &video_frame_queue);
	if(isStream_){//流媒体
		ret = video_decode_thread->Init(demux_thread_network->VideoCodecParameters());
	} else{
		ret = video_decode_thread->Init(demux_thread->VideoCodecParameters());
	}
	if(ret < 0){
		cout << "video_decode_thread init falied\n";
		return ;
	}
	ret = video_decode_thread->Start();
	if(ret < 0){
		cout << "video_decode_thread start falied\n";
		return ;
	}

	// 声明分析用的mat和显示用的img
	cv::Mat mat_;
	//QPixmap img;
	while(!abord_){
		// while(paused) std::cout << "pause status: " << paused << std::endl; // 等待播放
		AVFrame *frame = video_frame_queue.Pop(100);//取出帧

		if (frame){
//			qDebug() << frame->width << "x" << frame->height << "\n";
			AVFrameToCVMat(frame, mat_);//转换为cv::Mat
			//img = CVMatToQImage(mat_);//转换为qt支持的图像
			//emit pixframesSignal(img);
			emit cvframesSignal(mat_); // 发射信号，传递图像
		}
		av_frame_unref(frame);
		av_frame_free(&frame); // 使用完毕后释放frame
	}
	cout << "PlayThread loop break.\n";

	audio_packet_queue.release();//清空帧队列
	video_packet_queue.release();
	video_frame_queue.release();

	if(isStream_){//退出线程
		demux_thread_network->Stop();
		cout << "demux_thread_network stop\n";
//		delete demux_thread_network;
	} else{
		demux_thread->Stop();
		cout << "demux_thread stop\n";
//		delete demux_thread;
	}

	video_decode_thread->Stop();
	cout << "video_decode_thread stop\n";
//	delete video_decode_thread;
}

bool PlayThread::isStream(const std::string& url) {
	avformat_network_init();

	AVFormatContext* formatContext = nullptr;

	// Open the input URL
	if (avformat_open_input(&formatContext, url.c_str(), nullptr, nullptr) != 0) {
		std::cerr << "Could not open input URL" << std::endl;
		return false;
	}

	// Retrieve the format name
	std::string formatName = formatContext->iformat->name;

	// Close the input and free the format context
	avformat_close_input(&formatContext);

	// Check if the format name indicates a stream
	return (formatName.find("hls") != std::string::npos ||
			formatName.find("rtsp") != std::string::npos ||
			formatName.find("udp") != std::string::npos ||
			formatName.find("http") != std::string::npos);
}

void PlayThread::AVFrameToCVMat(AVFrame *frame, cv::Mat &mat)
{
	if (frame == nullptr) {
		qDebug() << "frame is empty" << endl;
		return;
	}
	if (frame->data[0] == nullptr) {
		qDebug() << "frame data is empty" << endl;
		return;
	}

	// 直接使用BGR24格式的数据
	mat = cv::Mat(frame->height, frame->width, CV_8UC3, frame->data[0], frame->linesize[0]).clone();

	// 使用缩放因子 0.5 来将图像的宽度和高度缩小到原来的一半
	cv::resize(mat, mat, cv::Size(), 0.75, 0.75, cv::INTER_LINEAR);

}

QPixmap PlayThread::CVMatToQImage(cv::Mat raw_img){
	cv::Mat pro_mat;

	if (raw_img.empty()) {
		std::cout << "No more frames. Exiting.\n";
		return QPixmap();
	}

	cv::cvtColor(raw_img,pro_mat,cv::COLOR_BGR2RGB);
	// 显示检测结果
	QImage qimg((uchar*)(pro_mat.data), static_cast<int>(pro_mat.cols), static_cast<int>(pro_mat.rows), static_cast<int>(pro_mat.step), QImage::Format_RGB888);

	return QPixmap::fromImage(qimg.copy()); 
}

