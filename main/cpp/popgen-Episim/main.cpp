//
// Created by robbe on 5/03/18.
//

#include <iostream>

#ifdef USING_QT
    #include <QtCore/QTextStream>
    #include <QtGui/QGuiApplication>
    #include <QtQml/QQmlApplicationEngine>
    #include <QtQuick/QQuickItem>
#endif

#include "popgen-Episim/GeoGen/GeoGrid.h"

using namespace std;

//currently i just added stuff from mapviewer's main.cpp
//we need to figure out what exactly is required an what not..

#ifdef USING_QT

int startMap()
{
#if QT_CONFIG(library)
    const QByteArray additionalLibraryPaths = qgetenv("QTLOCATION_EXTRA_LIBRARY_PATH");
    for (const QByteArray &p : additionalLibraryPaths.split(':'))
        QCoreApplication::addLibraryPath(QString(p));
#endif
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    int dummyargc = 0;
    QGuiApplication application(dummyargc, 0);

    QVariantMap parameters;

    // Fetch tokens from the environment, if present
    const QByteArray mapboxMapID = qgetenv("MAPBOX_MAP_ID");
    const QByteArray mapboxAccessToken = qgetenv("MAPBOX_ACCESS_TOKEN");
    const QByteArray hereAppID = qgetenv("HERE_APP_ID");
    const QByteArray hereToken = qgetenv("HERE_TOKEN");
    const QByteArray esriToken = qgetenv("ESRI_TOKEN");

    if (!mapboxMapID.isEmpty())
        parameters["mapbox.map_id"] = QString::fromLocal8Bit(mapboxMapID);
    if (!mapboxAccessToken.isEmpty()) {
        parameters["mapbox.access_token"] = QString::fromLocal8Bit(mapboxAccessToken);
        parameters["mapboxgl.access_token"] = QString::fromLocal8Bit(mapboxAccessToken);
    }
    if (!hereAppID.isEmpty())
        parameters["here.app_id"] = QString::fromLocal8Bit(hereAppID);
    if (!hereToken.isEmpty())
        parameters["here.token"] = QString::fromLocal8Bit(hereToken);
    if (!esriToken.isEmpty())
        parameters["esri.token"] = QString::fromLocal8Bit(esriToken);


    QQmlApplicationEngine engine;
    engine.load(QUrl::fromLocalFile("mapviewer/mapviewer.qml"));
    QObject::connect(&engine, SIGNAL(quit()), qApp, SLOT(quit()));

    QObject *item = engine.rootObjects().first();
    Q_ASSERT(item);

    QMetaObject::invokeMethod(item, "initializeProviders",
                              Q_ARG(QVariant, QVariant::fromValue(parameters)));

    return application.exec();
}

#endif

int main(int argc, char** argv)
{
    geogen::GeoGrid grid("config/geogen_default.xml");
    grid.GenerateAll();
#ifdef USING_QT
    //startMap(argc, argv);
    startMap();
#endif
}