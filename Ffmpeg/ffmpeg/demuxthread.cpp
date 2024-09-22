#include "demuxthread.h"

DemuxThread::DemuxThread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue)
	: audio_queue_(audio_queue), video_queue_(video_queue)
{}

DemuxThread::~DemuxThread()
{
	//锟斤拷锟斤拷锟斤拷什么锟斤拷锟斤拷锟斤拷没锟斤拷停止锟竭程撅拷锟剿筹拷锟斤拷,锟斤拷锟斤拷锟斤拷锟斤拷尾
	if(thread_){
		Stop();
	}
}


int DemuxThread::Init(const char *url){
	int ret = 0;
	url_ = url;

	ifmt_ctx = avformat_alloc_context();//锟斤拷始锟斤拷锟斤拷锟斤拷锟斤拷
	ret = avformat_open_input(&ifmt_ctx, url_.c_str(), NULL, NULL);//锟斤拷锟斤拷锟斤拷锟斤拷频
	std::cout << "Reading...\n";
	if(ret < 0){//锟斤拷示锟斤拷锟斤拷
		av_strerror(ret,error,sizeof(error));
		std::cout << "error:" << error << "\n";
		return -1;
	}

	ret = avformat_find_stream_info(ifmt_ctx,NULL);//锟斤拷取锟斤拷锟斤拷频锟斤拷息(锟斤拷锟斤拷锟斤拷式之锟斤拷锟斤拷)
	if(ret < 0){
		av_strerror(ret,error,sizeof(error));
		std::cout << "error:" << error << "\n";
		return -1;
	}
//	av_dump_format(ifmt_ctx, 0, url_.c_str(), 0);//锟斤拷示锟斤拷息(锟斤拷印锟斤拷锟秸讹拷)

	//锟斤拷取锟斤拷锟斤拷频锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	audio_index = av_find_best_stream(ifmt_ctx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
	video_index = av_find_best_stream(ifmt_ctx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);

	return 0;
}

int DemuxThread::Start(){
	//锟斤拷锟斤拷锟竭筹拷
	thread_ = new std::thread(&DemuxThread::Run,this);
	if(!thread_){
		std::cout << "Faile to creat thread.";
		return -1;
	}
	return 0;
}

int DemuxThread::Stop(){
	//停止
	Thread::Stop();
	avformat_close_input(&ifmt_ctx);
	return 0;
}

void DemuxThread::Run()
{
	//锟斤拷始锟斤拷锟斤拷
	AVPacket *frame_packet = av_packet_alloc();
	while(false == abort_){
		if(audio_queue_->Size() > 100 || video_queue_->Size() > 120){
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		//锟斤拷取锟斤拷锟斤拷锟斤拷
		int ret = av_read_frame(ifmt_ctx, frame_packet);
		if(ret < 0){
			av_strerror(ret,error,sizeof(error));
			std::cout << "error:" << error << "\n";
			end_of_file = true;//鏂囦欢璇诲畬
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
	av_packet_unref(frame_packet);
	av_packet_free(&frame_packet);

	std::cout << "Finish\n";
}

AVCodecParameters *DemuxThread::VideoCodecParameters()
{
	if(video_queue_ ){//!= -1
		return ifmt_ctx->streams[video_index]->codecpar;
	} else {
		return NULL;
	}
}
