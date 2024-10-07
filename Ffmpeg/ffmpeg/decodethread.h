#ifndef DECODETHREAD_H
#define DECODETHREAD_H
#include "thread.h"
#include "avpacketqueue.h"
#include "avframequeue.h"

extern "C"{
	#include "libavcodec/avcodec.h"
	#include <libswscale/swscale.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/opt.h>
}

class DecodeThread : public Thread
{
public:
	DecodeThread(AVPacketQueue *packet_queue, AVFrameQueue *frame_queue);
	~DecodeThread();

	int Init(AVCodecParameters *par);

	int Start();
	int Stop();

	void Run();

private:
	AVCodecContext *codec_ctx = NULL;
	AVPacketQueue *packet_queue_ = NULL;
	AVFrameQueue *frame_queue_ = NULL;
	SwsContext *sws_ctx = NULL;

	AVFrame *frameRGB = nullptr;

	AVPixelFormat renderPixelFormat = AV_PIX_FMT_BGR24;

	char error[256] = {0};
};

#endif // DECODETHREAD_H
