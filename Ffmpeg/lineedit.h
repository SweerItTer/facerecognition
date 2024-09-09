#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class LineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit LineEdit(QWidget *parent = nullptr);

signals:
	void Confirm(std::string str);

private:
	void keyPressEvent(QKeyEvent *e);
};

#endif // LINEEDIT_H
