#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <iostream>
#include <playthread.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Player; }
QT_END_NAMESPACE

class Player : public QWidget
{
	Q_OBJECT

public:
	Player(QWidget *parent = nullptr);
	~Player();

private:
	PlayThread *p_thread = nullptr;
	Ui::Player *ui;
	// 获取 label 的大小
	QSize labelSize;

	void Signal();

	bool string_compare(const std::string& s, const std::string& prefix){
		return (s.compare(0, prefix.size(), prefix) == 0);
	}

protected:
	void resizeEvent(QResizeEvent *event);
};
#endif // PLAYER_H
