#ifndef SCRIPT_H
#define SCRIPT_H
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

#include "Ffmpeg/playthread.h"
#include "./imageprocessor.h"
#include "./Yolov8/YOLO/yolo.h"

class MainWindow;
class Yolo;

class Script: public QObject {
	Q_OBJECT  // 确保 Script 也可以使用 Qt 的信号槽机制
public:
	explicit Script(MainWindow *window = nullptr)
		: mw(window)
	{
		Configurate();
	}

	~Script();
	int ensureEnter(std::string rtsp_url, std::string modelPath);

private:
	PlayThread *p_thread = nullptr;

	void Configurate();
	bool string_compare(const std::string& s, const std::string& prefix){
		return (s.compare(0, prefix.size(), prefix) == 0);
	}
//	bool fileExists(const std::string& fileName);
	void prossPixSignal(QPixmap image);
	void prossCVSignal(cv::Mat image);
	void updateUI(const QPixmap& image);

	std::string modelPath;

	MainWindow *mw = nullptr;
	Yolo *yolo = nullptr;
	Ort::Session *session = nullptr;
	ImageProcessor *imageProcessor = nullptr;
};

#endif // SCRIPT_H
