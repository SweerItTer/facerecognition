#ifndef AVPACKETQUEUE_H
#define AVPACKETQUEUE_H

#include "queue.h"

extern "C"{
#include <libavformat/avformat.h>
}

class AVPacketQueue
{
public:
	AVPacketQueue(){}
	~AVPacketQueue();

	void Abort();
	int Push(AVPacket *pkt);
	int Size(){
		return queue_.Size();
	}
	AVPacket *Pop(const int timeout);
	void release();
private:

	Queue<AVPacket *> queue_;

};

#endif // AVPacketQueue_H
