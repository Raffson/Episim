//
// Created by beau on 5/8/18.
//
#ifdef USING_QT

#include "MapViewer.h"

#include <iostream>
#include <sstream>

#include <QPair>
#include <QString>
#include <QtCore/QTextStream>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickItem>
#include <QMetaObject>

using namespace std;
using namespace std::chrono;
using namespace stride::sim_event;
namespace stride {
namespace viewers {

void MapViewer::Initialize() {
#if QT_CONFIG(library)
    const QByteArray additionalLibraryPaths = qgetenv("QTLOCATION_EXTRA_LIBRARY_PATH");
    for (const QByteArray &p : additionalLibraryPaths.split(':'))
        QCoreApplication::addLibraryPath(QString(p));
#endif

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    int             dummyargc = 0;
    QGuiApplication application(dummyargc, 0);
    application.mouseButtons();

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

    engine = make_shared<QQmlApplicationEngine>();
    engine->load(QUrl::fromLocalFile("mapviewer/mapviewer.qml"));
    QObject::connect(&(*engine), SIGNAL(quit()), qApp, SLOT(quit()));

    QObject *item = engine->rootObjects().first();
    Q_ASSERT(item);

    /// Call a function from a qml file.
    QMetaObject::invokeMethod(item, "initializeProviders", Q_ARG(QVariant, QVariant::fromValue(parameters)));

}


void MapViewer::Update(const sim_event::Id id)
{
    switch (id) {
        case Id::AtStart: {
            cout<<"Starting MapViewer..."<<endl;
            loadCities();
            break;
        }
        case Id::Stepped: {
            loadCities();
            break;
        }
        case Id::Finished: {
            break;
        }
        default: break;
    }
}


void MapViewer::loadCities() {
    cout<<"Loading cities..."<<endl;
    QObject *item = engine->rootObjects().first();
    Q_ASSERT(item);
    cout<<"\t Assertion passed"<<endl;
    /// To center the map on a specific location: use following code.
    QVariantList coords;
    cout<<"\t Created coordinates..."<<endl;
    stride::Coordinate c = m_grid->GetCenterOfGrid();
    cout<<"\t Center of map: "<<c.GetX()<<", "<<c.GetY()<<endl;
    coords.push_back(c.GetLatitude());
    coords.push_back(c.GetLongitude());
    cout<<"\t Setting center of map..."<<endl;
    QMetaObject::invokeMethod(item, "setCentre", Q_ARG(QVariant, QVariant::fromValue(coords)));
    cout<<"\t Done initializing Qt stuff..."<<endl;
    /// To add cities on the map: use following.
    auto cities = m_grid->GetCities();
    cout<<"\t got cities from grid"<<endl;
    map<unsigned int, vector<stride::City>> sorted;
    for (auto &city : cities)
        sorted[-city.second.GetPopulation()].emplace_back(city.second);
    for (auto c_it = sorted.begin(); c_it != sorted.end(); c_it++) {
        for (const auto& city : (*c_it).second) {
            std::stringstream ss;
            string s;
            string temp;
            /// c_it.first is the ID of the city, c_it.second is a pointer to the city itself.
//            stride::City &city = (*c_it).second;
            QVariantMap vals;
            QList<int> in_commuting_id;
            QList<int> in_commuting_size;
            QList<int> out_commuting_id;
            QList<int> out_commuting_size;
            /// ID of the city, used for the commuting details
            vals["id"] = city.GetId();
            /// Latitude
            vals["latitude"] = city.GetCoordinates().GetLatitude();
            /// Longitude
            vals["longitude"] = city.GetCoordinates().GetLongitude();
            /// X coordinate
            vals["x"] = city.GetCoordinates().GetX();
            /// Y coordinate
            vals["y"] = city.GetCoordinates().GetY();
            /// Radius
            vals["radius"] = city.GetPopulation() / (2 * M_PI);
            /// Percentage
            vals["perc"] = city.GetPopulation() / (double) m_grid->GetTotalPop();
            // cout << 50000 * (city.GetPopulation() / (double)grid.GetTotalPop()) << endl;
            /// Population
            vals["population"] = city.GetPopulation();
            /// Info
            ss << city.GetPopulation();
            ss >> s;
            s += "\n";
            s.append(city.GetName());
            s += "\n";
            QString qs = QString(s.c_str());
            vals["info"] = qs;
            /// Commuting information
            auto com_in = city.GetInCommuting();
            for (auto it : com_in) {
                // it.first: id of other city
                // it.second: number of commuters
                in_commuting_id.push_back(it.first);
                in_commuting_size.push_back(it.second);
            }
            auto com_out = city.GetOutCommuting();
            for (auto it : com_out) {
                // it.first: id of other city
                // it.second: number of commuters
                out_commuting_id.push_back(it.first);
                out_commuting_size.push_back(it.second);
            }


            QMetaObject::invokeMethod(item, "placeCity", Q_ARG(QVariant, QVariant::fromValue(vals)),
                                      Q_ARG(QVariant, QVariant::fromValue(in_commuting_id)),
                                      Q_ARG(QVariant, QVariant::fromValue(in_commuting_size)),
                                      Q_ARG(QVariant, QVariant::fromValue(out_commuting_id)),
                                      Q_ARG(QVariant, QVariant::fromValue(out_commuting_size)));
        }
    }
}

}
}

#endif //USING_QT
