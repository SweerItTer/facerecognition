#include "avframequeue.h"
#include <iostream>

AVFrameQueue::AVFrameQueue()
{

}

int AVFrameQueue::Push(AVFrame *val)
{
	AVFrame *temp_frame = av_frame_alloc();//分配空间
	av_frame_move_ref(temp_frame, val);//把数据分配到新的空间里

	return queue_.Push(temp_frame);
}

AVFrame *AVFrameQueue::Pop(const int timeout)
{
	AVFrame *temp_frame = NULL;
	int ret = queue_.Pop(temp_frame, timeout);

	if(ret < 0){
		if(ret == -1){
			std::cout << "Already aborted.";
		}
		else if(ret == -2){
			std::cout << "Frame Queue is empty\n";
		}
	}
	return temp_frame;
}

AVFrame *AVFrameQueue::Front()
{
	AVFrame *temp_frame = NULL;
	int ret = queue_.Front(temp_frame);
	if(ret < 0){
		if(ret == -1){
			std::cout << "queue is already aborted.\n";//队列跳出状态
		}
		else if(ret == -2){
			//如果依然为空
			std::cout << "queue is empty.\n";
		}
	}
	return temp_frame;
}

void AVFrameQueue::release()
{
	while (1){
		AVFrame *temp_frame = NULL;
		int ret = queue_.Pop(temp_frame,1);
		if(ret >= 0){
			av_frame_free(&temp_frame);
			continue;
		}
		else {
			break;
		}
	}
}
