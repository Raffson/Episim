//
// Created by robbe on 03.06.18.
//

#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QGeoCoordinate>

int main(int argc, char** argv){

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QStringLiteral("mapviewer/Gui.qml"));
    return app.exec();
}