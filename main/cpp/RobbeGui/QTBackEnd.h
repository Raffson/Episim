//
// Created by robbe on 03.06.18.
//

#pragma once

#include <QObject>

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

class QTBackEnd: public QObject {
    Q_OBJECT

public:

    QTBackEnd(QObject *parent = nullptr);

    /// Handler for QML to generate pop
    void GenPop();



};

