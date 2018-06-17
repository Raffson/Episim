//
// Created by beau on 5/8/18.
//
#ifdef USING_QT

#include "MapViewer.h"
#include "popgen-Episim/util/DesignByContract.h"
#include "util/FileSys.h"

#include <iostream>
#include <sstream>

#include <QPair>
#include <QString>
#include <QtCore/QTextStream>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickItem>
#include <QMetaObject>
#include <QtGui/QPainter>

using namespace std;
using namespace std::chrono;
using namespace stride::sim_event;
namespace stride {
namespace viewers {

MapViewer::MapViewer(shared_ptr<GeoGrid> grid, const std::string &outputPrefix)
    : m_grid(grid), m_output_prefix(outputPrefix), m_step(0)
{
    m_output_prefix += util::FileSys::IsDirectoryString(m_output_prefix) ? "png/" : "/png/";
    boost::filesystem::path dir(m_output_prefix.c_str());
    if (!(boost::filesystem::exists(dir))) {
        boost::filesystem::create_directory(dir);
    }
    m_map_option = grid ? grid->GetConfigPtree().get("run.map_option", "none") : "none";
    m_png_option = grid ? grid->GetConfigPtree().get("run.png_option", "none") : "none";
}

void MapViewer::LoadMap(bool showMap) {
    REQUIRE(m_grid, "GeoGrid must be intialized in order to load a map for it.");
    cout << "Loading map..." << endl;
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

    m_item = engine.rootObjects().first();
    Q_ASSERT(m_item);

    /// Call a function from a qml file.
    QMetaObject::invokeMethod(m_item, "initializeProviders", Q_ARG(QVariant, QVariant::fromValue(parameters)));

    /// To center the map on a specific location: use following code.
    QVariantList       coords;
    stride::Coordinate c = m_grid->GetCenterOfGrid();
    coords.push_back(c.GetLatitude());
    coords.push_back(c.GetLongitude());
    QMetaObject::invokeMethod(m_item, "setCentre", Q_ARG(QVariant, QVariant::fromValue(coords)));

    /// To add cities on the map: use following.
    auto cities = m_grid->GetCities();
    map<unsigned int, vector<stride::City>> sorted;
    for( auto& city : cities)
        sorted[-city.second.GetPopulation()].emplace_back(city.second);
    for (auto c_it = sorted.begin(); c_it != sorted.end(); c_it++) {
        for (auto city : (*c_it).second) {
            std::stringstream ss;
            string s;
            string temp;
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
            vals["radius"] = city.GetEffectivePopulation() / (2 * M_PI);
            /// Percentage
            vals["perc"] = city.GetEffectivePopulation() / (double) m_grid->GetTotalPop();
            // cout << 50000 * (city.GetPopulation() / (double)m_grid->GetTotalPop()) << endl;
            /// Population
            vals["population"] = city.GetEffectivePopulation();
            /// Infected
            vals["infected"] = city.GetInfectedCount();
            /// Info
            ss << city.GetEffectivePopulation() << "<i>(" << city.GetInfectedCount() << ")</i><br>";
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
                in_commuting_size.push_back((int)it.second);
            }
            auto com_out = city.GetOutCommuting();
            for (auto it : com_out) {
                // it.first: id of other city
                // it.second: number of commuters
                out_commuting_id.push_back(it.first);
                out_commuting_size.push_back((int)it.second);
            }
            QMetaObject::invokeMethod(m_item, "placeCity", Q_ARG(QVariant, QVariant::fromValue(vals)),
                                      Q_ARG(QVariant, QVariant::fromValue(in_commuting_id)),
                                      Q_ARG(QVariant, QVariant::fromValue(in_commuting_size)),
                                      Q_ARG(QVariant, QVariant::fromValue(out_commuting_id)),
                                      Q_ARG(QVariant, QVariant::fromValue(out_commuting_size)));
        }
    }
    stringstream s;
    s << m_output_prefix << m_step << ".png";
    QString fname(s.str().c_str());
    QMetaObject::invokeMethod(m_item, "saveToImage", Q_ARG(QVariant, QVariant::fromValue(fname)));
    if(showMap)
        application.exec();
}

void MapViewer::ToPng(){
    stringstream s;
    s << m_output_prefix << m_step << ".png";
    QString fname(s.str().c_str());
    QMetaObject::invokeMethod(m_item, "saveToImage", Q_ARG(QVariant, QVariant::fromValue(fname)));
}

void MapViewer::Update(const sim_event::Id id) {
    switch (id) {
        case Id::AtStart: {
            bool showMap = (m_map_option == "step");
            if( m_png_option == "step" or showMap )
                LoadMap(showMap);
//            if( m_png_option == "step" )
//                ToPng();
            break;
        }
        case Id::Stepped: {
            m_step++;
            bool showMap = (m_map_option == "step");
            if( m_png_option == "step" or showMap )
                LoadMap(showMap);
//            if( m_png_option == "step" )
//                ToPng();
            break;
        }
        case Id::Finished: {
            m_step++;
            bool showMap = (m_map_option != "none");
            if( m_png_option != "none" or showMap )
                LoadMap(showMap);
//            if( m_png_option != "none" )
//                ToPng();
            break;
        }
        default:
            break;
    }
}

} // namespace viewers
} // namespace stride

#endif //USING_QT
