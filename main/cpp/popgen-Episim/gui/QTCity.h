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

    Q_PROPERTY(QGeoCoordinate crd READ GetCoordinates CONSTANT)
    Q_PROPERTY(int popCount MEMBER m_pop CONSTANT)
    Q_PROPERTY(QString name READ GetName CONSTANT)
    Q_PROPERTY(int id READ GetId CONSTANT)

    Q_PROPERTY(int total_in_commuters READ GetTotalCommutersIn CONSTANT)
    Q_PROPERTY(int total_out_commuters READ GetTotalCommutersOut CONSTANT)
    Q_PROPERTY(int infected READ GetInfected NOTIFY InfectedChanged)
    Q_PROPERTY(bool clicked READ IsClicked WRITE SetClicked NOTIFY ClickedChanged)


signals:
    void InfectedChanged();
    void ClickedChanged();

public:
    stride::City* GetModelCity()const {return m_city;}
    QTBackEnd* GetBackEnd() const {return m_back_end;}
    int GetInfected() const {return (int) m_city->GetInfectedCount();}
    bool IsClicked(){return m_is_clicked;}
    int GetId() const;
    void CreateCommutingList(int amount);
    QGeoCoordinate GetCoordinates() const;

    QList<QTCommuter*> GetCommuters()  const{return m_commuting_lst;}

    void Flip();

private:
    QString GetName() const;

    int GetTotalCommutersOut(){return(int) m_city->GetTotalOutCommutersCount();}
    int GetTotalCommutersIn(){return (int) m_city->GetTotalInCommutersCount();}


    void FillInOutCommuters();
    void FillInInCommuters();
    void SetClicked(bool val);


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

