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

	if(video_decode_thread) {
		video_decode_thread->Stop();
		cout << "video_decode_thread stop\n";
		delete video_decode_thread;
	}
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
	
	while(!abord_){
		while(paused) {
			msleep(1);  // 避免空循环占用CPU
		}
		
		AVFrame *frame = video_frame_queue.Pop(100);
		if (frame) {
			cv::Mat mat_;
			AVFrameToCVMat(frame, mat_);
			
			if(!mat_.empty()) {
				emit cvframesSignal(mat_.clone());
			}
			
			mat_.release();
			av_frame_unref(frame);
			av_frame_free(&frame);
			frame = nullptr;  // 确保指针置空
		} else {
			msleep(1);  // 如果没有帧，短暂休眠避免CPU占用过高
		}
	}
	cout << "PlayThread loop break.\n";

	// 先清理队列
	audio_packet_queue.release();
	video_packet_queue.release();
	video_frame_queue.release();

	// 然后停止所有线程
	if(isStream_){
		if(demux_thread_network) {  // 添加空指针检查
			demux_thread_network->Stop();
			cout << "demux_thread_network stop\n";
		}
	} else {
		if(demux_thread) {  // 添加空指针检查
			demux_thread->Stop();
			cout << "demux_thread stop\n";
		}
	}

	if(video_decode_thread) {  // 添加空指针检查
		video_decode_thread->Stop();
		cout << "video_decode_thread stop\n";
	}
}

bool PlayThread::isStream(const std::string& url) {
	avformat_network_init();
	AVFormatContext* formatContext = nullptr;
	bool is_stream = false;  // 添加返回值变量

	// 添加错误处理
	if (avformat_open_input(&formatContext, url.c_str(), nullptr, nullptr) < 0) {
		std::cerr << "Could not open input URL" << std::endl;
		return false;
	}

	if (formatContext && formatContext->iformat) {  // 添加空指针检查
		std::string formatName = formatContext->iformat->name;
		is_stream = (formatName.find("hls") != std::string::npos ||
					formatName.find("rtsp") != std::string::npos ||
					formatName.find("udp") != std::string::npos ||
					formatName.find("http") != std::string::npos);
	}

	avformat_close_input(&formatContext);
	return is_stream;
}

void PlayThread::AVFrameToCVMat(AVFrame *frame, cv::Mat &mat)
{
	if (frame == nullptr || frame->data[0] == nullptr) {
		qDebug() << "Invalid frame or frame data";
		return;
	}

	try {
		// 创建新的Mat对象
		cv::Mat temp(frame->height, frame->width, CV_8UC3, frame->data[0], frame->linesize[0]);
		
		if(!temp.empty()) {
			cv::resize(temp, mat, cv::Size(), 0.75, 0.75, cv::INTER_LINEAR);
		}
		
		// 确保temp被释放
		temp.release();
	} catch (const cv::Exception& e) {
		qDebug() << "OpenCV error:" << e.what();
		//mat.release();  // 确保出错时mat被释放
	}
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

