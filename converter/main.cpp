#include "mainwindow.h"
#include <QApplication>
#include <QResource>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("UVC Font Converter");

    MainWindow w;
    w.show();

    return app.exec();
}
