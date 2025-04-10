﻿#include "script.h"
#include "./UI/mainwindow.h"
#include "./UI/ui_mainwindow.h"

#include <QCoreApplication>
#include <QObject>
#include <QPixmap>
#include <QSize>

// json数据处理(数据库配置)
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

Script::~Script()
{
	if (p_thread) {
		p_thread->Stop();
		delete p_thread; // 释放线程
	}
	delete session;
	std::cout << "player close." << std::endl;

	std::cout << "deleted yolo and mainwindow." << std::endl;
	delete imageProcessor;

	delete *database_ptr;
	delete database_ptr;
}

void Script::Configurate(){
	// 数据库配置
	QFile configFile;
	int ret = loadConfig(configFile);// 读取配置文件
	if (ret < 0) {
		// 配置出现问题将删除错误的配置文件
		configFile.remove();
		return;
	}

	// yolo = new Yolo(nullptr);
	// facenet配置
	// try {
	// 	QString modelPath = mw->ui->le_facenetonnx->text();
	// 	facenet = new FaceNet(modelPath);
	// }
	// catch(const Ort::Exception& e)
	// {
	// 	QMessageBox::critical(nullptr, "Facenet error", e.what());
	// 	return;
	// }

	// 模型初始化放在主窗口里（构造函数）
	if(!yolo->isLoaded || !facenet->isLoaded){
		QMessageBox::critical(nullptr, "Init error", "Yolo or Facenet is nullptr.");
		return;
	}
	std::cout << "Created yolo." << std::endl;

	qRegisterMetaType<cv::Mat>("cv::Mat");

    imageProcessor = new ImageProcessor(yolo, facenet, getCurrentDatabase());

	imageProcessor->setCallback([this](const QPixmap& img) {
		// 在主线程中更新 UI
		QMetaObject::invokeMethod(this, [this, img]() {
			updateUI(img);
		}, Qt::QueuedConnection);
	});
	std::cout << "Processor set callback." << std::endl;
	startProcessingTimer();
	isinite = true;
}

int Script::loadConfig(QFile &configFile) {
	// 获取用户目录
	QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QDir().mkpath(configPath);
	configFile.setFileName(configPath + "/db_config.json");// 配置文件路径
	qDebug() << configPath;
	// 读取配置文件
	if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray jsonData = configFile.readAll();
		configFile.close();
		// 解析json数据
		QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
		if (jsonDoc.isNull()) {
			std::cerr << "Error : Failed to parse the configuration file." << std::endl;
			perror("Error message");
			return -1;
		}
		// 转换为json对象
		QJsonObject jsonObj = jsonDoc.object();
	
		// 读取数据库配置
		string host = jsonObj.value("host").toString("localhost").toStdString();
		unsigned int port = jsonObj.value("port").toString("3306").toUInt();
		string userName = jsonObj.value("userName").toString("root").toStdString();
		string password = jsonObj.value("password").toString().toStdString();
		// 设置数据库连接名称 在UI中显示
		mw->ui->le_storagefile->setText(jsonObj.value("connectionName").toString(""));
		// 初始化数据库连接
		try {
       		*database_ptr = new FaceDatabase(host, userName, password, "FaceDB", port);
			std::cout << "Database connected." << std::endl;
		}
		catch (const std::runtime_error& e){
			std::cerr << e.what() << std::endl;
			QMessageBox::critical(nullptr, "Database error", e.what());
			return -1;
		}
		return 0;
	}
	else {
		std::cerr << "Error : Failed to open the configuration file." << std::endl;
		perror("Error message");
		return -1;
	}
	return 0;
}

void Script::startProcessingTimer() {
	processingTimer = new QTimer(this);
	connect(processingTimer, &QTimer::timeout, this, &Script::processNextFrame);
	processingTimer->start(10); // 每50ms处理一帧，可以根据需要调整
}

void Script::processNextFrame() {
	cv::Mat frame;
	{
		QMutexLocker locker(&queueMutex);
		if (!frameQueue.isEmpty()) {
			frame = frameQueue.dequeue();
			// 主动清理队列
			if (frameQueue.size() > 25) {
				while (frameQueue.size() > 25) {
					cv::Mat old_frame = frameQueue.dequeue();
					old_frame.release();
				}
			}
		}
	}
	
	if (!frame.empty()) {
		imageProcessor->setImage(frame);
		frame.release();
	}
}


int Script::ensureEnter(std::string modelPath, std::string url)// 初始化检测线程
{
	// 判断初始化
	if (!isinite) { //若数据库连接初始化出现问题,将会直接退出函数,因此需要判断是否已经初始化
		std::cout << "Script is not inited." << std::endl;
		
		if(!yolo->loadModel(QString::fromStdString(modelPath))){
			std::cerr << "Session load model failed." << std::endl;
			return -1;
		}
		Configurate();
		if(isinite){
			rtsp_url = url;
			std::cout << "Script is inited." << std::endl;
			return 0;
		}
		return -1;
	}
	rtsp_url = url;
	std::cout << "Script is inited." << std::endl;
	imageProcessor->resetHSWN();
	std::cout << "Reset HSWN." << std::endl;

	return 0;
}

int Script::play() {
	if(!isinite)	return -1;
	// 确认拉流链接的合法性
	bool is_rtspurl = string_compare(rtsp_url, "rtsp://");
	QObject::connect(yolo, &Yolo::signal_str, this,
					 [this] (QString str) {
						mw->ui->te_onnxmessage->append(str);
					 });
	
	if ( is_rtspurl || 1 ) {
		// 创建拉流线程
		if (p_thread) {
			p_thread->Stop();
			while(!p_thread->isFinished()){
				std::cout << "Waitting...." << std::endl;
			}
			delete p_thread; // 释放线程
			imageProcessor->paused = true; // 停止处理线程
		}

		p_thread = new PlayThread(rtsp_url);
		std::cout << "PlayThread created." << std::endl;
		p_thread->Start();
		imageProcessor->paused = false; // 开始处理线程
		// 拉流信号
		QObject::connect(p_thread, &PlayThread::pixframesSignal, this,
						 &Script::prossPixSignal);
		QObject::connect(p_thread, &PlayThread::cvframesSignal, this,
						 &Script::prossCVSignal);
		return 0;
	} else return -1;
	return 0;
}

void Script::pasue()
{
	if (p_thread) {
		p_thread->Pause(1);
	}
	isPause = true;
}

void Script::resume()
{	
	if (p_thread) {
		p_thread->Pause(0);
	}
	isPause = false;
}

void Script::prossPixSignal(QPixmap image){
	// 按 label 大小等比例缩放图像
	QSize labelSize = mw->ui->lb_camera->size();
	// 设置缩放后的图像
	mw->ui->lb_camera->setPixmap(image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Script::prossCVSignal(cv::Mat image) {
	if (isPause) {
		image.release();
		return; // 若处于暂停状态,则不处理图像
	}
	QMutexLocker locker(&queueMutex);
	if (frameQueue.size() < 30) { // 限制队列大小，防止内存溢出
		frameQueue.enqueue(std::move(image));
		p_thread->Pause(0);
	} else {
		p_thread->Pause(1);
		image.release();
	}
}

// 更新UI显示图像
void Script::updateUI(QPixmap image) {
	QPixmap scaledPixmap = image.scaled(mw->ui->lb_camera->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mw->ui->lb_camera->setPixmap(scaledPixmap);
	scaledPixmap = QPixmap();
	image = QPixmap();
}

