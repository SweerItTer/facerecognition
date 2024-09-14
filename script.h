#ifndef SCRIPT_H
#define SCRIPT_H
#include <iostream>
#include "Ffmpeg/playthread.h"
<<<<<<< HEAD
//#include "./UI/mainwindow.h"

class MainWindow;

class Script: public QObject {
	Q_OBJECT  // 确保 Script 也可以使用 Qt 的信号槽机制
public:
	explicit Script(MainWindow *window = nullptr) : mw(window) {}
//	explicit Script(){}
=======


class Script
{
public:
	Script(){}
>>>>>>> 9f4521b430aaffd1280eafd7cda72292bff90fc8
	~Script();
	int ensureEnter(std::string rtsp_url);

private:
	PlayThread *p_thread = nullptr;

	bool string_compare(const std::string& s, const std::string& prefix){
		return (s.compare(0, prefix.size(), prefix) == 0);
	}
<<<<<<< HEAD

	MainWindow *mw = nullptr;
=======
>>>>>>> 9f4521b430aaffd1280eafd7cda72292bff90fc8
};

#endif // SCRIPT_H
