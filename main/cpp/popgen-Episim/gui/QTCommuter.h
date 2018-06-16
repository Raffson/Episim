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

// Modifiers
/***********************************************************************************************************************/
/**
 * @name Modifers
 * All modifiers of QTCommuter
 */
///@{
    /// @brief Sets the in commuter count. Used for double commuter lines
    /// @praram amoun a double the amount of in commuters to set
    void set_in_commuters(double amount);
///@}

// Helpers
/***********************************************************************************************************************/
private:
/**
 * @name helpers
 * helper functions of QTCommuter
 */
 ///@{
    /// @brief  Calculates the Line Width used for visual representation
    /// @return returns the width of the line
    double CalculateLineWidth();

    /// @brief Picks the color based on the line state
    /// @return a QColor with the color representation of the line
    QColor PickColor();
///@}

// DataMembers
/***********************************************************************************************************************/
private:
/**
 * @name Datamembers
 * all datamember
 */
///@{
///< the model of the main city
QTCity* m_city1;

///< the model of the secondary city
QTCity* m_city2;

///< QGeoCoordinate of the center of the main city
QGeoCoordinate m_center_city1;

///< QGeoCoordinate of the center of the cecondary city
QGeoCoordinate m_center_city2;

///< OutCommuter total count
double m_out_count;

///< InCommuter total count
double m_in_count;
///@}

};




