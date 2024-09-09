#ifndef AVFRAMEQUEUE_H
#define AVFRAMEQUEUE_H

#include "queue.h"

extern "C"{
#include "libavformat/avformat.h"
}

class AVFrameQueue
{
public:
	AVFrameQueue();
	~AVFrameQueue(){}

	void Abort(){
		release();
		queue_.Abort();
	}

	int Push(AVFrame *val);
	int Size(){
		return queue_.Size();
	}
	AVFrame *Pop(const int timeout);

	AVFrame *Front();
	void release();
private:

	Queue<AVFrame *> queue_;
};

#endif // AVFRAMEQUEUE_H
