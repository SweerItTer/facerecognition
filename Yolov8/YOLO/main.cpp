#include "mainwindow.h"
#include "yolo.h"

#include <QApplication>
#include <QTextCodec>

#pragma execution_character_set("UTF-8")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}


