//
// Created by robbe on 03.06.18.
//

#pragma once

#include <QObject>
#include <QList>

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

#include "QTCity.h"

class QTBackEnd: public QObject {
    Q_OBJECT

public:

    explicit QTBackEnd(QObject *parent = nullptr);
    ~QTBackEnd();

    /// @brief Handler for QML to generate pop. Will do the genpop logic.
    Q_INVOKABLE void genPop();
    Q_PROPERTY(QList<QObject*> cities READ get_citiies)

    QList<QObject*> get_citiies(){return m_cities;}

private:
    void makeCityList();

private:

    ///> Represents our grid
    shared_ptr<stride::GeoGrid> m_grid;

    ///> A Qlist that conains our QTCity models
    QList<QObject*> m_cities;


};

