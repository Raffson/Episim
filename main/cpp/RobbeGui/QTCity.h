//
// Created by robbe on 04.06.18.
//

#pragma once

#include <QObject>
#include <QPoint>
#include <QtPositioning/QGeoCoordinate>
#include <QString>
#include <QList>

#include "popgen-Episim/model/City.h"
#include "QTBackEnd.h"
#include "QTCommuter.h"
//#include "RobbeGui/QTBackEnd.h"

#include <iostream>
using namespace std;

class QTBackEnd;
class QTCommuter;

class QTCity : public QObject {
    Q_OBJECT

public:
    explicit  QTCity(stride::City* m_city = nullptr, QTBackEnd* back_end = nullptr,  QObject* = nullptr);
    ~QTCity() override = default;
    QTCity(const QTCity&);

    Q_INVOKABLE void ctyTest() {cout << "Hello from" << m_city->GetName() << endl;}

    Q_PROPERTY(QGeoCoordinate crd READ get_coordinates CONSTANT)
    Q_PROPERTY(int popCount MEMBER m_pop CONSTANT)
    Q_PROPERTY(QString name READ get_name CONSTANT)
    Q_PROPERTY(int id READ get_id CONSTANT)

    Q_PROPERTY(int total_in_commuters READ get_total_commuters_in CONSTANT)
    Q_PROPERTY(int total_out_commuters READ get_total_commuters_out CONSTANT)
    Q_PROPERTY(int infected READ get_infected NOTIFY infectedChanged)
    Q_PROPERTY(bool clicked READ get_clicked WRITE set_clicked NOTIFY clickedChanged)


signals:
    void infectedChanged();
    void clickedChanged();

public:
    stride::City* get_m_city()const {return m_city;}
    QTBackEnd* get_back_end() const {return m_back_end;}
    int get_infected() const {return (int) m_city->GetInfectedCount();}
    bool get_clicked(){return m_is_clicked;}
    int get_id() const;
    void create_commuting_lst(int amount);
    QGeoCoordinate get_coordinates() const;

    QList<QTCommuter*> get_commuters ()  const{return m_commuting_lst;}

    void flip();

private:
    QString get_name() const;

    int get_total_commuters_out(){return(int) m_city->GetTotalOutCommutersCount();}
    int get_total_commuters_in(){return (int) m_city->GetTotalInCommutersCount();}


    void fill_in_out_commuters();
    void fill_in_in_commuters();
    void set_clicked(bool val);


private:

    stride::City* m_city;
    QList<int> m_sorted_out_commuters; ///> Keeps a list sorted high low to the commuters
    QList<int> m_commuter_out_count; ///> count ot amount of commuters
    QList<int> m_sorted_in_commuters; ///> Keeps a list sorted high low to the commuters
    QList<int> m_commuter_in_count; ///> count ot amount of commuters

    QList<QTCommuter*> m_commuting_lst;
    bool m_is_clicked{false};
    QTBackEnd* m_back_end;

    const int m_id;
    const int m_pop;

};

//Q_DECLARE_METATYPE(QTCity)
//Q_DECLARE_METATYPE(QTCity*)
//Q_DECLARE_OPAQUE_POINTER(QTCity*);*/

