#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

//#include <iostream>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("yzh3551");
    w.show();

    QFile file(QString(":/test.qss"));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());

//    QMessageBox(Icon, QString(""), qss);
//    std::cerr << qss.toUtf8().constData() << std::endl;
    a.setStyleSheet(qss);

    return a.exec();
}
