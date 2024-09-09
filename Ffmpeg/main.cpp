#include "player.h"

#include <QApplication>
//rtsp://admin:15935746jk@Liu@172.18.107.147:554/h264/ch1/main/av_stream
//D:/Program/project/C++/QT/FFmpeg_player/01.mp4
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Player w;
	w.show();
	return a.exec();
}
