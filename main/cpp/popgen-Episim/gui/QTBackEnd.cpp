//
// Created by robbe on 03.06.18.
//



#include "QTBackEnd.h"

#include <QtQuick/QQuickView>

#include <fstream>
#include <cstddef>

#include "sim/SimRunner.h"
#include "util/FileSys.h"
#include "util/InstallDirs.h"

#include "boost/filesystem/path.hpp"


using namespace std;
namespace stride {
namespace gui{
QTBackEnd::QTBackEnd(QQmlApplicationEngine &engine, ptree &pt, CliController *clicontrol, QObject *parent)
        : QObject(parent), m_grid(nullptr), m_cities(), m_commuters(),
          m_pt(pt), m_geo_pt(), m_engine(engine),m_cntrller(clicontrol)
{

    string file(m_pt.get<string>("run.geopop_file"));
    string path = "config/" + file;
    fstream filestr;
    filestr.open(path);
    using namespace boost::property_tree::xml_parser;
    read_xml(path, m_geo_pt, trim_whitespace | no_comments);
    filestr.close();
}


void QTBackEnd::genPop() {


    string file(m_pt.get<string>("run.geopop_file"));
    string path = "config/" + file;
    fstream filestr;
    filestr.open(path);

    boost::property_tree::xml_writer_settings<string> w( ' ', 2 );
    write_xml( path, m_geo_pt, std::locale(), w );
    filestr.close();

    m_grid = stride::GeoGridGenerator().Generate(m_pt);
    stride::PopulationGenerator(*m_grid).Generate();

    m_pop_generated = true;

    m_total_pop = m_grid->GetTotalPop();
    emit PopChanged();

    MakeCityList();
}

void QTBackEnd::MakeCityList() {


    m_cities.clear();
    m_commuters.clear();
    for (auto &it: m_grid->GetCities()) {
        m_cities.append(new QTCity(&it.second, this));
    }
    for (auto &it: m_cities) { // Needs to happen when the full list is initialized
        auto cty = dynamic_cast<QTCity *>(it);

        cty->CreateCommutingList(10);
    }
    emit CitiesChanged();
}

QGeoCoordinate QTBackEnd::GetCenter() const {

    stride::Coordinate crd = m_grid->GetCenterOfGrid();
    return QGeoCoordinate(crd.GetLatitude(), crd.GetLongitude());
}


QObject *QTBackEnd::getCity(unsigned int id) const {

    for (auto &it: m_cities) {
        if (id == (unsigned int) dynamic_cast<QTCity *>(it)->GetId()) {
            return it;
        }
    }
    cout << "Could not find city with id: " << id << endl;
    return nullptr;

}

void QTBackEnd::runSimulator() {

    if (!m_pop_generated) {
        genPop();
    }

    if (m_grid == nullptr) {
        cout << "run popgen first" << endl;
        return;
    }

    auto view_ptr = make_shared<SimRunner>(m_pt, m_grid->GetPopulation(), m_grid);
    m_cntrller->RegisterViewers(view_ptr);
    view_ptr->Run();
    m_pop_generated = false;

    for (auto &it: m_cities) {
        emit dynamic_cast<QTCity *>(it)->InfectedChanged();
    }
    emit SelectedInfectedChanged();
    emit TotalInfectedChanged();
}

//Config setters and getters
/***********************************************************************************************************************/

QString QTBackEnd::getConfig(const QString &xml_tag, bool geo) const {

    if(geo){
        return QString((m_geo_pt.get<string>(xml_tag.toStdString())).c_str());
    }

    return QString((m_pt.get<string>(xml_tag.toStdString())).c_str());
}

void QTBackEnd::setConfig(const QString &xml_tag, const QString &val, bool geo) {

    if(geo){
        m_geo_pt.put(xml_tag.toStdString(), val.toStdString());
    }

    else{
        m_pt.put(xml_tag.toStdString(), val.toStdString());
    }
}

QString QTBackEnd::readPath(const QString &tag, bool geo_grid, bool data) const {

    boost::filesystem::path basePath = "";
    if(m_pt.get<bool>("run.use_install_dirs", true)){

        if(data){
            basePath  = util::FileSys::GetDataDir();

        }

        else{
            basePath = util::FileSys::GetConfigDir();
        }
    }

    string file;
    if(geo_grid){
        file = m_geo_pt.get<string>(tag.toStdString());
    }

    else{
        file = m_pt.get<string>(tag.toStdString());
    }

    basePath /= file;
    return QString(basePath.c_str());

}

void QTBackEnd::setPath(const QString &tag, const QString &path, bool geoGrid) {

    string placer = "";

    if(m_pt.get<bool>("run.use_install_dirs", true)){
        placer = path.split("/").back().toStdString();
        cout<< placer << endl;
        cout<< tag.toStdString();
    }

    else{
        placer = path.toStdString();
    }
    if(geoGrid){
        m_geo_pt.put(tag.toStdString(), placer);
    }

    else{
        m_pt.put(tag.toStdString(), placer);
    }



}

bool QTBackEnd::getBoolConfig(const QString &xml_tag) const {

    return m_geo_pt.get<bool>(xml_tag.toStdString());
}

bool QTBackEnd::setBoolConfig(const QString &xml_tag, const bool &value) {

    m_geo_pt.put(xml_tag.toStdString(), value);
    return !value;
}

QUrl QTBackEnd::getDataPath() const {

    return QUrl(util::FileSys::GetDataDir().c_str());
}

QUrl QTBackEnd::getConfigPath() const{
            return QUrl(util::FileSys::GetConfigDir().c_str());
        }

int QTBackEnd::countConfigChildren(const QString &xml_tag, bool geo) const{


    if(geo){
        return (int) m_geo_pt.get_child(xml_tag.toStdString()).size();
    }

    else{
        return (int) m_pt.get_child(xml_tag.toStdString()).size();
    }


}

double QTBackEnd::getChildAtIndex(const QString &xml_tag, int index, bool geo){

    if(geo){

        while(countConfigChildren(xml_tag, geo) - 1 <= index){
            m_geo_pt.add(xml_tag.toStdString() + ".value", 0);
        }
        auto child = m_geo_pt.get_child(xml_tag.toStdString());
        return std::next(child.find("value"),index)->second.get_value<double>();
    }

    else{
        while(countConfigChildren(xml_tag, geo) - 1 <= index){
          m_pt.add(xml_tag.toStdString() + ".value", 0);
        }
        auto child = m_pt.get_child(xml_tag.toStdString());
        return std::next(child.find("value"),index)->second.get_value<double>();
    }
}

void QTBackEnd::setChildAtIndex(const QString &xml_tag, int index, double value, bool geo) {

    if(geo) {
        auto &child = m_geo_pt.get_child(xml_tag.toStdString());
        auto forwrd = std::next(child.find("value"), index);
        forwrd->second.put("", value);
    }
    else{
        auto &child = m_pt.get_child(xml_tag.toStdString());
        auto forwrd = std::next(child.find("value"), index);
        forwrd->second.put("", value);
    }
}

void QTBackEnd::cleanChildren (const QString& xml_tag, int size, bool geo){

    boost::property_tree::ptree* child;
    if(geo) {
        child = &m_geo_pt.get_child(xml_tag.toStdString());


    }
    else{
        child = &m_pt.get_child(xml_tag.toStdString());
    }

    while(child->size() > (unsigned int)  size){
        child->pop_back();
    }

}

/***********************************************************************************************************************/


int QTBackEnd::CountSelectedInfected() const {

    int counter = 0;
    for (auto &it : m_cities) {
        auto *cty = dynamic_cast<QTCity *>(it);
        if (cty->IsClicked()) {
            counter += cty->GetInfected();
        }
    }
    return counter;
}

int QTBackEnd::GetTotalInfected() const {
    if (m_grid == nullptr) {
        return 0;
    }
    int counter = 0;
    for (auto &it : m_grid->GetCities()) {
        counter += it.second.GetInfectedCount();
    }
    return counter;
}

void QTBackEnd::AddSelectedPop(int amount) {

    m_selected_pop += amount;
    emit SelectedPopChanged();
}

void QTBackEnd::AddCommuteLines(const QList<QTCommuter *> &lst) {

    for (auto &it: lst) {
        m_commuters.append(it);
    }
    emit CommutersChanged();
}

void QTBackEnd::AddCommuteLineNoEmit(const QList<QTCommuter *> &lst) {
    for (auto &it: lst) {
        m_commuters.append(it);
    }
}

void QTBackEnd::RemoveCommuteLines(const QList<QTCommuter *> &lst) {

    for (auto &it: lst) {
        m_commuters.removeOne(it);
    }
    emit CommutersChanged();
}

void QTBackEnd::RemoveCommuteLinesNoEmit(const QList<QTCommuter *> &lst) {

    for (auto &it: lst) {
        m_commuters.removeOne(it);
    }
}


void QTBackEnd::flipItems(QList<QObject *> cities) {
    for (auto &it: cities) {
        auto cty = dynamic_cast<QTCity *>(it);
        if (cty != nullptr) {
            if (cty->IsClicked()) {
                RemoveCommuteLinesNoEmit(cty->GetCommuters());
            } else {
                AddCommuteLineNoEmit(cty->GetCommuters());
            }
            cty->Flip();
        } else {
            cout << "Some Items are in Flip items not cities" << endl;
        }

    }
    emit CommutersChanged();
    emit SelectedPopChanged();
    emit SelectedInfectedChanged();
}


}
}


