//
// Created by robbe on 5/03/18.
//

#include <iostream>
#include <sstream>

#ifdef USING_QT
#include <QString>
#include <QPair>
#include <QtCore/QTextStream>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickItem>
#endif

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/PopGen/PopulationGenerator.h"

using namespace std;

#ifdef USING_QT

int startMap(geogen::GeoGrid& grid)
{
#if QT_CONFIG(library)
        const QByteArray additionalLibraryPaths = qgetenv("QTLOCATION_EXTRA_LIBRARY_PATH");
        for (const QByteArray& p : additionalLibraryPaths.split(':'))
                QCoreApplication::addLibraryPath(QString(p));
#endif
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        int             dummyargc = 0;
        QGuiApplication application(dummyargc, 0);
        application.mouseButtons();

        QVariantMap parameters;

        // Fetch tokens from the environment, if present
        const QByteArray mapboxMapID       = qgetenv("MAPBOX_MAP_ID");
        const QByteArray mapboxAccessToken = qgetenv("MAPBOX_ACCESS_TOKEN");
        const QByteArray hereAppID         = qgetenv("HERE_APP_ID");
        const QByteArray hereToken         = qgetenv("HERE_TOKEN");
        const QByteArray esriToken         = qgetenv("ESRI_TOKEN");

        if (!mapboxMapID.isEmpty())
                parameters["mapbox.map_id"] = QString::fromLocal8Bit(mapboxMapID);
        if (!mapboxAccessToken.isEmpty()) {
                parameters["mapbox.access_token"]   = QString::fromLocal8Bit(mapboxAccessToken);
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

        QObject* item = engine.rootObjects().first();
        Q_ASSERT(item);

        /// Call a function from a qml file.
        QMetaObject::invokeMethod(item, "initializeProviders", Q_ARG(QVariant, QVariant::fromValue(parameters)));

        /// To center the map on a specific location: use following code.
        QVariantList       coords;
        geogen::Coordinate c = grid.GetCenterOfGrid();
        coords.push_back(c.latitude);
        coords.push_back(c.longitude);
        QMetaObject::invokeMethod(item, "setCentre", Q_ARG(QVariant, QVariant::fromValue(coords)));

        /// To add cities on the map: use following.
        auto cities = grid.GetCities();
        for (map<unsigned int, geogen::City>::iterator c_it = cities.begin(); c_it != cities.end(); c_it++) {
                std::stringstream ss;
                string            s;
                string            temp;
                /// c_it.first is the ID of the city, c_it.second is a pointer to the city itself.
                geogen::City& city = (*c_it).second;
                QVariantMap              vals;
                QList<QPair<int, int>> in_commuting;
                QList<QPair<int, int>> out_commuting;
                /// ID of the city, used for the commuting details
                vals["id"] = city.GetId();
                /// Latitude
                vals["latitude"] = city.GetCoordinates().latitude;
                /// Longitude
                vals["longitude"] = city.GetCoordinates().longitude;
                /// X coordinate
                vals["x"] = city.GetCoordinates().x;
                /// Y coordinate
                vals["y"] = city.GetCoordinates().y;
                /// Radius
                vals["radius"] = city.GetPopulation() / (2 * M_PI);
                /// Percentage
                vals["perc"] = city.GetPopulation() / (double)grid.GetTotalPop();
                // cout << 50000 * (city.GetPopulation() / (double)grid.GetTotalPop()) << endl;
                /// Population
                vals["population"] = city.GetPopulation();
                /// Info
                ss << city.GetPopulation();
                ss >> s;
                s += "\n";
                s.append(city.GetName());
                s += "\n";
                QString qs   = QString(s.c_str());
                vals["info"] = qs;
                /// Commuting information
                auto com_in = city.GetInCommuting();
                for (auto it : com_in){
                    // it.first: id of other city
                    // it.second: number of commuters
                    in_commuting.push_back(qMakePair(it.first, it.second));
                }
                auto com_out = city.GetOutCommuting();
                for (auto it : com_out){
                    // it.first: id of other city
                    // it.second: number of commuters
                    out_commuting.push_back(qMakePair(it.first, it.second));
                }


                QMetaObject::invokeMethod(item, "placeCity", Q_ARG(QVariant, QVariant::fromValue(vals)),
                                          Q_ARG(QVariant, QVariant::fromValue(in_commuting)),
                                          Q_ARG(QVariant, QVariant::fromValue(out_commuting)) );
        }

        return application.exec();
}

#endif

int main(int argc, char** argv)
{
        geogen::GeoGrid grid("config/geogen_default.xml");
        grid.GenerateAll();

        popgen::PopulationGenerator pop_generator(grid);
        pop_generator.AssignAll();

#ifdef USING_QT
        // startMap(argc, argv);
        startMap(grid);
#endif
}
