#ifndef SCRIPT_H
#define SCRIPT_H
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

#include "Ffmpeg/playthread.h"
#include "./imageprocessor.h"

#include "./Yolov8/YOLO/yolo.h"
#include "./Sql/facedatabase.h"
#include "./Facenet/facenet.h"

class MainWindow;
class Yolo;

class Script: public QObject {
	Q_OBJECT  // 确保 Script 也可以使用 Qt 的信号槽机制
public:
	explicit Script(MainWindow *window = nullptr, Yolo *yolo_ = nullptr, FaceNet *facenet_ = nullptr, FaceDatabase **database_ = nullptr)
		: mw(window)
		, yolo(yolo_)
		, facenet(facenet_) // 正逆向传递, 对象是公用的(主窗口，脚本，录入界面)
        , database_ptr(database_)  // 存储数据库指针的指针
	{
		Configurate();
	}

	~Script();
	int ensureEnter(std::string modelPath, std::string url);// 初始化检测线程
	int play();// 播放视频

	void pasue();
	void resume();

private:
	PlayThread *p_thread = nullptr;
	std::string modelPath;

	MainWindow *mw = nullptr;
    FaceDatabase **database_ptr;  // 存储指针的指针
	Yolo *yolo;
	FaceNet *facenet;
	
	Ort::Session *session = nullptr;
	ImageProcessor *imageProcessor = nullptr;

	void Configurate();
	bool string_compare(const std::string& s, const std::string& prefix){
		return (s.compare(0, prefix.size(), prefix) == 0);
	}
	// 添加一个辅助函数来获取当前数据库指针
    FaceDatabase* getCurrentDatabase() const { return *database_ptr; }
	int loadConfig(QFile &configFile);

	void prossPixSignal(QPixmap image);
	void prossCVSignal(cv::Mat image);
	void updateUI(QPixmap image);


	void startProcessingTimer();
	void processNextFrame();

	bool isinite = false;
	bool isPause = false;
	std::string rtsp_url;

	QQueue<cv::Mat> frameQueue;
	QMutex queueMutex;
	QTimer *processingTimer;
};

#endif // SCRIPT_H
