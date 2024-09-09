#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>
#include <QImage>
#include <QPixmap>
#include <iostream>

#include "./ffmpeg/avpacketqueue.h"
#include "./ffmpeg/avframequeue.h"
#include "./ffmpeg/demuxthread.h"
#include "./ffmpeg/decodethread.h"
#include "./ffmpeg/demuxthread_network.h"

#include <opencv2/opencv.hpp>
extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

using namespace std;

class PlayThread : public QThread
{
	Q_OBJECT
public:
	explicit PlayThread(QObject *parent = nullptr);
	PlayThread(std::string str);
	~PlayThread();

	void run() override;

	void Stop();

	void Start();

signals:
	void framesSignal(QPixmap image);
private:
	std::string url_;
	bool abord_;

	DemuxThread_network *demux_thread_network = nullptr;
	DemuxThread *demux_thread = nullptr;
	DecodeThread *video_decode_thread = nullptr;

	bool isStream(const std::string& url);

	QPixmap CVMatToQImage(cv::Mat raw_img);
	void AVFrameToCVMat(AVFrame *frame, cv::Mat &mat);
};

#endif // PLAYTHREAD_H
