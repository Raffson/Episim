//
// Created by robbe on 13.06.18.
// File contains the QTCommuter class.
//

#pragma once

#include <QObject>
#include <QtGui/QColor>
#include <QtPositioning/QGeoCoordinate>

#include "QTCity.h"

class QTCity;
/**
 * @brief: Represents a visual commuter line in the GUI. We will handle the logic at the C++ side
 *
 */
class QTCommuter : public QObject {
    Q_OBJECT

public:
// Constructors, Destructor
/***********************************************************************************************************************/
/**
 * @name Constructors, Destructors
 * All constructors and destructors
 *
 */
 ///@{
    /**
     * @brief Constructor of QTCommuter can also used as a default to use QTCommuter as a metatype in QML
     * @param city1 The main city, city where we draw the commute line from.
     * @param city2 City where the commute line goes to
     * @param outcommuting total outcommuters on the line
     * @param incommuting  total incommuters on the line
     * @param parent  parent used for QML purposes
     */
    explicit QTCommuter(QTCity* city1 = nullptr, QTCity* city2 = nullptr, double outcommuting = 0, double incommuting = 0, QObject* parent = nullptr);

    /// Default destructor
    ~QTCommuter() override = default;
///@}

// Q Properties
/***********************************************************************************************************************/
/**
 * @name Q Properties
 * Q Properties of a Commuting line.
 */
 ///@{
    ///< The Coordinates of the center of city1 in latitude and longitude
    Q_PROPERTY(QGeoCoordinate center_city1 MEMBER m_center_city1 CONSTANT)

    ///< The Coordinate of the center of city2 in latitude and longitude
    Q_PROPERTY(QGeoCoordinate center_city2 MEMBER m_center_city2 CONSTANT)

    ///< The width of our line in the Gui Representation
    Q_PROPERTY(double width READ CalculateLineWidth CONSTANT)

    ///< Color the line should have in the gui Representation
    Q_PROPERTY(QColor color READ PickColor CONSTANT)
///@}

/***********************************************************************************************************************/
    void set_in_commuters(double amount);
    QTCity* GetMainCity();
    QTCity* GetSecondCity();

private:

    double CalculateLineWidth();
    QColor PickColor();
private:

QTCity* m_city1;
QTCity* m_city2;

QGeoCoordinate m_center_city1;
QGeoCoordinate m_center_city2;

double m_out_count;
double m_in_count;

double m_total_out;
double m_total_in;

};




