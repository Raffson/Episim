//
// Created by robbe on 03.06.18.
//
#include "stdlib.h"
#ifdef USING_QT

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "RobbeGui/QTBackEnd.h"
#endif

#include <boost/property_tree/xml_parser.hpp>
#include "boost/property_tree/ptree.hpp"
using namespace boost::property_tree;


int main(int argc, char** argv){
    ptree pt;
    read_xml("config/run_short.xml",pt);
    system("cd bin; rm -r *_*; cd ..");

#ifdef USING_QT
    QGuiApplication app(argc, argv); // main app
    QQmlApplicationEngine engine;

    QScopedPointer<QTBackEnd> backend(new QTBackEnd(engine, pt));
    qmlRegisterType<QTCity>("episim.city",1,0,"City"); // let the qml find the City class
    engine.load(QStringLiteral("mapviewer/Gui.qml"));
    engine.rootContext()->setContextProperty("backend", backend.data());

    return app.exec();
#endif
}