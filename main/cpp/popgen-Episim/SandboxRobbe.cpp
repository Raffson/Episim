//
// Created by robbe on 03.06.18.
//

#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QGeoCoordinate>

#include "RobbeGui/QTBackEnd.h"

int main(int argc, char** argv){

    QGuiApplication app(argc, argv); // main app

    qmlRegisterType<QTBackEnd>("episim.backend",1,0,"BackEnd"); // let the qml find the BackEnd class
    //qmlRegisterType<QTCity>("episim.city",1,0,"City"); // let the qml find the City class


    QQmlApplicationEngine engine;
    engine.load(QStringLiteral("mapviewer/Gui.qml"));

    return app.exec();
}