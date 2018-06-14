//
// Created by robbe on 14.06.18.
//
#pragma once

#include <QAbstractListModel>
#include <QObject>


class QTCommuterModel: public QAbstractListModel {
public:
    int rowCount();


private:
    QList<QObject*> m_data;
};



