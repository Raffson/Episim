//
// Created by robbe on 03.06.18.
//

#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QGeoCoordinate>

#include "RobbeGui/QTBackEnd.h"

int main(int argc, char** argv){

    QGuiApplication app(argc, argv);

    qmlRegisterType<QTBackEnd>("episim.backend",1,0,"BackEnd");

    QQmlApplicationEngine engine;
    engine.load(QStringLiteral("mapviewer/Gui.qml"));
    return app.exec();
}