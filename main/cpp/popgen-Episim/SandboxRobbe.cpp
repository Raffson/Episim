//
// Created by robbe on 03.06.18.
//
#include "stdlib.h"
#ifdef USING_QT
#include "RobbeGui/QTBackEnd.h"
#endif

#include <boost/property_tree/xml_parser.hpp>
#include "boost/property_tree/ptree.hpp"
using namespace boost::property_tree;


int main(int argc, char** argv){
    ptree pt;
    read_xml("config/run_short.xml",pt);
    system("rm -r *_*");

#ifdef USING_QT
    QGuiApplication app(argc, argv); // main app
    QQmlApplicationEngine engine;

    //qRegisterMetaType<QTCity>();


    QScopedPointer<QTBackEnd> backend(new QTBackEnd(engine, pt));
    //qmlRegisterType<QTCity>("episim.city", 1, 0 ,"City");
    engine.rootContext()->setContextProperty("backend", backend.data());
    engine.load(QStringLiteral("mapviewer/Gui.qml"));


    return app.exec();
#endif
}