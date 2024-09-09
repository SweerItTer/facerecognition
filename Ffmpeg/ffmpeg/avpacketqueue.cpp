#include "avpacketqueue.h"
#include <iostream>

AVPacketQueue::~AVPacketQueue(){

}

void AVPacketQueue::Abort()
{
	release();
	queue_.Abort();
}

int AVPacketQueue::Push(AVPacket *pkt)
{
	AVPacket *temp_pkt = av_packet_alloc();//分配空间
	av_packet_move_ref(temp_pkt, pkt);//把数据分配到新的空间里

	return queue_.Push(temp_pkt);
}

AVPacket *AVPacketQueue::Pop(const int timeout)
{
	AVPacket *temp_pkt = NULL;
	int ret = queue_.Pop(temp_pkt, timeout);

	if(ret < 0){
		if(ret == -1){
			std::cout << "Already aborted.";
		}
		else if(ret == -2){
			std::cout << "Queue is empty\n";
		}
	}
	return temp_pkt;
}

void AVPacketQueue::release()
{
	while (1){
		AVPacket *temp_pkt = NULL;
		int ret = queue_.Pop(temp_pkt,1);
		if(ret >= 0){
			av_packet_free(&temp_pkt);
			continue;
		}
		else {
			break;
		}
	}
}
