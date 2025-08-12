#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); 

    //Подгрузка стиля в главной функции для универсальности сборки под Linux и Windows
    QString styleString;
    styleString.append("QMainWindow{background-color: rgb(255, 255, 255);}");
    styleString.append("QPushButton#coPushButton{color: rgb(0, 0, 0);background-color:rgb(150, 180, 204);border-radius: 10px;padding: 8px;}");
    styleString.append("QPushButton#bPushButton{color: rgb(0, 0, 0);background-color:rgb(150, 180, 204);border-radius: 10px;padding: 9px;}");
    styleString.append("QPushButton#kPushButton{color: rgb(0, 0, 0);background-color:rgb(150, 180, 204);border-radius: 10px;padding: 8px;}");
    styleString.append("QPushButton#vPushButton{color: rgb(0, 0, 0);background-color:rgb(150, 180, 204);border-radius: 10px;padding: 8px;}");
    styleString.append("QPushButton#connectionPushButton{color: rgb(0, 0, 0);background-color:rgb(150, 180, 204);border-radius: 10px;padding: 8px;}");
    styleString.append("QTextEdit{color: #426887;font-style: solid;}");
    styleString.append("QLabel#electricity1Label{color:#5283aa;}");
    styleString.append("QLabel#electricity2Label{color:#5283aa;}");
    styleString.append("QLabel#temperature1Label{color:#5283aa;}");
    styleString.append("QLabel#temperature2Label{color:#5682a3;}");

    a.setStyleSheet(styleString);

    MainWindow w;
    w.show();

    return a.exec();
}
