#include "script.h"
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
	delete yolo;
	std::cout << "deleted yolo and mainwindow." << std::endl;
	delete imageProcessor;

	delete mw;
	delete database;
	delete yolo;
	delete facenet;
	delete session;

}

void Script::Configurate(){
	int ret = loadConfig();// 读取配置文件
	if (ret < 0) {
		return;
	}
	yolo = new Yolo(nullptr);
	facenet = new FaceNet(L"D:/Program/project/gitee/facerecognition/Facenet/ONNX_model/full_model.onnx");
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
	isinite = true;
}

int Script::loadConfig() {
	// 获取用户目录
	QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QDir().mkpath(configPath);
	QFile configFile(configPath + "/db_config.json");
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
		database = new FaceDatabase(host, userName, password, "FaceDB", port);
		return 0;
// ---------------------- 顺序问题: 当用户未设置数据库配置,将会出现数据库报错,直接退出程序 ---
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
	if (isinite) { //若数据库连接初始化出现问题,将会直接退出函数,因此需要判断是否已经初始化
		std::cout << "Script is not inited." << std::endl;
		Configurate();
	}
	bool is_rtspurl = string_compare(rtsp_url, "rtsp://");
	QObject::connect(yolo, &Yolo::signal_str, this,
					 [this] (QString str) {
						mw->ui->te_onnxmessage->append(str);
					 });
	if(!yolo->loadModel(QString::fromStdString(modelPath))){
		std::cerr << "Session load model failed." << std::endl;
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
	if (frameQueue.size() < 30) { // 限制队列大小，防止内存溢出
		frameQueue.enqueue(std::move(image));
		p_thread->Pause(0);
	} else {
		p_thread->Pause(1);
	}
}

// 更新UI显示图像
void Script::updateUI(const QPixmap& image) {
	QPixmap scaledPixmap = image.scaled(mw->ui->lb_camera->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mw->ui->lb_camera->setPixmap(scaledPixmap);

}


