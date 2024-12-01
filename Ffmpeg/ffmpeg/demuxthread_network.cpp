#include "demuxthread_network.h"

DemuxThread_network::DemuxThread_network(AVPacketQueue *audio_queue, AVPacketQueue *video_queue)
	: audio_queue_(audio_queue), video_queue_(video_queue){}

DemuxThread_network::~DemuxThread_network()
{
	if(thread_){
		Stop();
	}
}


int DemuxThread_network::Init(const char *url){
	int ret = 0;
	url_ = url;

	//设置打开参数
	AVDictionary *options = nullptr;
	//如果包含rtsp则设置超时时间
	if (strstr(url, "rtsp") != nullptr) {
		av_log(nullptr, AV_LOG_INFO, "set rtsp timeout\n");
		ret = av_dict_set(&options, "stimeout", "5000000", 0);
		if (ret < 0) {
			av_log(nullptr, AV_LOG_ERROR, "set rtsp timeout failed: %s\n", error);
			return false;
		}
		//设置缓冲大小
		ret = av_dict_set(&options, "buffer_size", "1024000", 0);
		if (ret < 0) {
			av_strerror(ret,error,sizeof(error));
			av_log(nullptr, AV_LOG_ERROR, "set rtsp buffer size failed: %s\n", error);
			return false;
		}
		//设置最大延迟
		ret = av_dict_set(&options, "max_delay", "500000", 0);
		if (ret < 0) {
			av_strerror(ret,error,sizeof(error));
			av_log(nullptr, AV_LOG_ERROR, "set rtsp max delay failed: %s\n", error);
			return false;
		}
		ret = av_dict_set(&options, "rtsp_transport", "tcp", 0);
		if (ret < 0) {
			av_strerror(ret,error,sizeof(error));
			av_log(nullptr, AV_LOG_ERROR, "set rtsp transport failed: %s\n", error);
			return false;
		}
		ret = av_dict_set(&options, "max_analyze_duration", "10", 0);
		if (ret < 0) {
			av_strerror(ret,error,sizeof(error));
			av_log(nullptr, AV_LOG_ERROR,"av_dict_set error %s\n",  error);
			return false;
		}
	}

	//创建上下文
	ifmt_ctx = avformat_alloc_context();
	if (ifmt_ctx == nullptr) {
		av_strerror(ret,error,sizeof(error));
		av_log(nullptr, AV_LOG_ERROR, "alloc format context failed: %s\n", error);
		return false;
	}

	//打开
	ret = avformat_open_input(&ifmt_ctx, url, nullptr, &options);
	if (ret < 0) {
		av_strerror(ret,error,sizeof(error));
		av_log(nullptr, AV_LOG_ERROR, "open url %s failed: %s\n", url, error);
		return false;
	}

	//查找流信息
	ret = avformat_find_stream_info(ifmt_ctx, nullptr);
	if (ret < 0) {
		av_strerror(ret,error,sizeof(error));
		av_log(nullptr, AV_LOG_ERROR, "find stream info failed: %s\n", error);
		return false;
	}

	//寻找包索引
	audio_index = av_find_best_stream(ifmt_ctx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
	video_index = av_find_best_stream(ifmt_ctx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);

	return 0;
}

int DemuxThread_network::Start(){
	thread_ = new std::thread(&DemuxThread_network::Run,this);
	if(!thread_){
		std::cout << "Faile to creat thread.";
		return -1;
	}
	return 0;
}

int DemuxThread_network::Stop(){
	Thread::Stop();
	avformat_close_input(&ifmt_ctx);
	return 0;
}

void DemuxThread_network::Run()
{
	AVPacket *frame_packet = av_packet_alloc();
	while(false == abort_){
		if(video_queue_->Size() > 30){
			frame_packet = video_queue_->Pop(100);
			av_packet_unref(frame_packet);
			continue;
		}

		int ret = av_read_frame(ifmt_ctx, frame_packet);
		if(ret < 0){
			av_strerror(ret,error,sizeof(error));
			std::cout << "error:" << error << "\n";
			end_of_file = true;//文件读完
			break;
		}
		if(frame_packet->stream_index == audio_index){
			continue;
		} else if (frame_packet->stream_index == video_index){
			video_queue_->Push(frame_packet);
//			av_log(NULL,AV_LOG_INFO,"video_queue size : %d \n",(video_queue_->Size()));
		}
		else{
		}
		av_packet_unref(frame_packet);
	}
	av_packet_free(&frame_packet);

	std::cout << "Finish\n";
}

AVCodecParameters *DemuxThread_network::VideoCodecParameters()
{
	if(video_queue_ ){//!= -1
		return ifmt_ctx->streams[video_index]->codecpar;
	} else {
		return NULL;
	}
}
