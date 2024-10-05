#include "ui/mainwindow.h"

// rtsp://admin:15935746jk@Liu@172.18.107.147:554/h264/ch1/main/av_stream
// rtmp://8.134.146.151/live/camera
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
