#include "script.h"
#include "./UI/mainwindow.h"
#include "./UI/ui_mainwindow.h"

#include <QCoreApplication>
#include <QObject>
#include <QPixmap>
#include <QSize>

Script::~Script()
{
	if (p_thread) {
		p_thread->Stop();
		delete p_thread; // 释放线程
	}
	delete session;
	std::cout << "player close." << std::endl;
	delete yolo;
	std::cout << "deleted yolo and mainwindow." << std::endl;

}

void Script::Configurate(){
	yolo = new Yolo(nullptr);
	facenet = new FaceNet(L"D:/Program/project/gitee/facerecognition/Facenet/ONNX_model/full_model.onnx");
	database = new FaceDatabase("localhost", "root", "Youisshit1", "FaceDB");
	std::cout << "Created yolo." << std::endl;

	qRegisterMetaType<cv::Mat>("cv::Mat");

	imageProcessor = new ImageProcessor(yolo, facenet, database);

	imageProcessor->setCallback([this](const QPixmap& img) {
		// 在主线程中更新 UI
		QMetaObject::invokeMethod(this, [this, img]() {
			updateUI(img);
		}, Qt::QueuedConnection);
	});
	std::cout << "Processor set callback." << std::endl;
	startProcessingTimer();
}

void Script::startProcessingTimer() {
	processingTimer = new QTimer(this);
	connect(processingTimer, &QTimer::timeout, this, &Script::processNextFrame);
	processingTimer->start(5); // 每50ms处理一帧，可以根据需要调整
}

void Script::processNextFrame() {
	cv::Mat frame;
	{// 取出帧
		QMutexLocker locker(&queueMutex);
		if (!frameQueue.isEmpty()) {
			frame = frameQueue.dequeue();
		}
	}
	// 处理帧
	if (!frame.empty()) {
		imageProcessor->setImage(frame);
	}
}

int Script::ensureEnter(std::string rtsp_url, std::string modelPath){
	bool is_rtspurl = string_compare(rtsp_url, "rtsp://");
	QObject::connect(yolo, &Yolo::signal_str, this,
					 [this] (QString str) {
						mw->ui->te_onnxmessage->append(str);
					 });
	if(!yolo->loadModel(QString::fromStdString(modelPath))){
		std::cerr << "model do not exist." << std::endl;
		return -1;
	}
	if ( is_rtspurl || 1) {
		if (p_thread) {
			p_thread->Stop();
			while(!p_thread->isFinished()){
				std::cout << "Waitting...." << std::endl;
			}
			delete p_thread; // 释放线程
		}

		p_thread = new PlayThread(rtsp_url);
		std::cout << "PlayThread created." << std::endl;
		p_thread->Start();

		QObject::connect(p_thread, &PlayThread::pixframesSignal, this,
						 &Script::prossPixSignal);
		QObject::connect(p_thread, &PlayThread::cvframesSignal, this,
						 &Script::prossCVSignal);
		return 0;
	} else return -1;
	return 0;
}

void Script::prossPixSignal(QPixmap image){
	// 按 label 大小等比例缩放图像
	QSize labelSize = mw->ui->lb_camera->size();
	// 设置缩放后的图像
	mw->ui->lb_camera->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Script::prossCVSignal(cv::Mat image) {
	QMutexLocker locker(&queueMutex);
	if (frameQueue.size() < 144) { // 限制队列大小，防止内存溢出
		frameQueue.enqueue(std::move(image));
		p_thread->Pause(0);
	} else {
		p_thread->Pause(1);
	}
}

void Script::updateUI(const QPixmap& image) {
	QPixmap scaledPixmap = image.scaled(mw->ui->lb_camera->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mw->ui->lb_camera->setPixmap(scaledPixmap);
//	mw->ui->lb_camera->setPixmap(image);

}


