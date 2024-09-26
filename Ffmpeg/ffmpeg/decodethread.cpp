#include "decodethread.h"
#include <iostream>

DecodeThread::DecodeThread(AVPacketQueue *packet_queue, AVFrameQueue *frame_queue)
	: packet_queue_(packet_queue), frame_queue_(frame_queue){}

DecodeThread::~DecodeThread()
{
	if(thread_) Stop();

	if(codec_ctx) avcodec_free_context(&codec_ctx);
}

int DecodeThread::Init(AVCodecParameters *par)
{
//	AVCodecParameters 结构体
//	用于保存音视频流的基本参数信息
//	如编解码器类型、ID、格式、比特率、帧率等
	if(!par){
		std::cout << "Got empty CodecParameters\n";
		return -1;
	}
//分配解码器上下文空间
	codec_ctx = avcodec_alloc_context3(NULL);
//将AVCodecParameters 结构体信息复制给AVCodecContext 结构体
	int ret = avcodec_parameters_to_context(codec_ctx, par);
	if(ret < 0){
		av_strerror(ret,error,sizeof(error));
		std::cout << "error:" << error << "\n";
		return -1;
	}
//查找解码器
	const AVCodec *codec = avcodec_find_decoder(codec_ctx->codec_id);
	if(!codec){
		av_strerror(ret,error,sizeof(error));
		std::cout << "error:" << error << "\n";
		return -1;
	}

//用于根据给定的AVCodecContext和AVCodec实例初始化编解码器
	ret = avcodec_open2(codec_ctx,codec,NULL);
	if(ret < 0){
		av_strerror(ret,error,sizeof(error));
		std::cout << "error:" << error << "\n";
		return -1;
	}

	//设置格式转换

	sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
							 codec_ctx->width, codec_ctx->height, renderPixelFormat,
							 SWS_BICUBIC, nullptr, nullptr, nullptr);

	if (sws_ctx == nullptr) {
			av_log(nullptr, AV_LOG_ERROR, "create sws context failed\n");
		}

	//av_opt_set(codec_ctx->priv_data, "tune", "zerolatency", 0);
	return 0;
}

int DecodeThread::Start()
{
	//需要执行的函数,执行的函数所属对象
	thread_ = new std::thread(&DecodeThread::Run, this);
	if(!thread_){
		std::cout << "Faile to creat thread.";
		return -1;
	}
	return 0;
}

int DecodeThread::Stop()
{
	return Thread::Stop();
}

void DecodeThread::Run()
{
	int ret = 0;
	AVFrame *frame = nullptr; // 分配空间
	AVPacket *pkt = nullptr;

	while (!abort_) {
		if (frame_queue_->Size() > 60) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		pkt = packet_queue_->Pop(100);
		if (pkt) {
			ret = avcodec_send_packet(codec_ctx, pkt);
			av_packet_unref(pkt);
			av_packet_free(&pkt);

			if (ret < 0) {
				if (ret == AVERROR(EAGAIN)) {
					continue;
				} else {
					av_strerror(ret, error, sizeof(error));
					std::cout << "Error sending packet: " << error << "\n";
					break;
				}
			}

			//#################
			// 在循环外部，确保 frameRGB 已经分配和初始化
			frameRGB = av_frame_alloc();
			if (!frameRGB) {
				std::cerr << "Could not allocate frameRGB\n";
				return;
			}

			// 为 frameRGB 分配缓冲区
			int numBytes = av_image_get_buffer_size(renderPixelFormat, codec_ctx->width, codec_ctx->height, 1);
			uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

			av_image_fill_arrays(frameRGB->data, frameRGB->linesize, buffer, renderPixelFormat,
								 codec_ctx->width, codec_ctx->height, 1);

			frameRGB->width = codec_ctx->width;
			frameRGB->height = codec_ctx->height;
			frameRGB->format = renderPixelFormat;

			ret = av_frame_get_buffer(frameRGB, 0);
			if (ret < 0) {
				av_strerror(ret, error, sizeof(error));
				av_log(nullptr, AV_LOG_ERROR, "alloc frame buffer failed: %s\n", error);
				return;
			}
			//#################

			frame = av_frame_alloc();
			while (1) {
				ret = avcodec_receive_frame(codec_ctx, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				} else if (ret < 0) {
					av_strerror(ret, error, sizeof(error));
					std::cout << "Error receiving frame: " << error << "\n";
					break;
				}

				//报错

				ret = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height,
								frameRGB->data, frameRGB->linesize);

				if (ret < 0) {
					av_strerror(ret, error, sizeof(error));
					av_log(nullptr, AV_LOG_ERROR, "sws scale failed: %s\n", error);
					break;
				}

				//########

				//转换后的格式
//				av_log(nullptr, AV_LOG_INFO, "frame info width %d , height %d , pix_fmt %s\n",
//					   frame->width, frame->height, av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format)));
//				av_log(nullptr, AV_LOG_INFO, "frameRGB info width %d , height %d , pix_fmt %s\n",
//					   frameRGB->width, frameRGB->height, av_get_pix_fmt_name(static_cast<AVPixelFormat>(frameRGB->format)));

				frame_queue_->Push(frameRGB);

//				av_log(nullptr, AV_LOG_INFO,
//					   "%s frame queue size : %d \n", codec_ctx->codec->name, frame_queue_->Size());
				av_frame_unref(frame);
				av_frame_unref(frameRGB);
				continue;
			}
			av_frame_free(&frame);
			frame = nullptr;
		} else {
			av_log(nullptr, AV_LOG_WARNING, "Do not get packet or queue is empty.\n...Exit...\n");
			break;
		}
	}

	av_frame_free(&frame);
	av_frame_free(&frameRGB);
	frame = nullptr;
	frameRGB = nullptr;
	sws_freeContext(sws_ctx);
	avcodec_free_context(&codec_ctx);
	av_log(nullptr, AV_LOG_INFO, "Decoder thread finished.\n");

}
