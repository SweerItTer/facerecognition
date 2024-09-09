#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H
#include <iostream>
#include "thread.h"
#include "avpacketqueue.h"

extern "C"{
#include <libavformat/avformat.h>
}


class DemuxThread : public Thread
{
public:
	DemuxThread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue);
	~DemuxThread();

	//url : file path or link of Stream
	int Init(const char* url);

	int Start();
	int Stop();

	void Run();
	bool end_of_file;

	AVCodecParameters *AudioCodecParameters();//音频解码格式
	AVCodecParameters *VideoCodecParameters();//视频解码格式

private:
	//复用器上下文
	AVFormatContext *ifmt_ctx = NULL;
	char error[256] = {0};
	std::string url_;

	AVPacketQueue *audio_queue_;
	AVPacketQueue *video_queue_;

	//when find index number of type,it's begin from 0,which means -1 is don't find the index number
	int audio_index = -1, video_index = -1;
};

#endif // DEMUXTHREAD_H
