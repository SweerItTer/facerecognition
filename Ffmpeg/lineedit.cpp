#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{

}

void LineEdit::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return){
		QString str_q = this->text();
		std::string str = str_q.toStdString();
		emit Confirm(str);
	} else {
		QLineEdit::keyPressEvent(e);
	}
}


