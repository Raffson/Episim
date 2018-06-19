//
// Created by robbe on 13.06.18.
// File contains the QTCommuter class.
//

#pragma once

#include <QObject>
#include <QtGui/QColor>
#include <QtPositioning/QGeoCoordinate>

#include "QTCity.h"

namespace stride {
namespace gui {
class QTCity;

/**
* @brief: Represents a visual commuter line in the GUI. We will handle the logic at the C++ side.
*
*/
class QTCommuter : public QObject {
Q_OBJECT

public:
// Constructors, Destructor
/***********************************************************************************************************************/
/**
* @name Constructors, Destructors
* All constructors and destructors.
*
*/
    ///@{
    /**
     * @brief Constructor of QTCommuter can also used as a default to use QTCommuter as a metatype in QML.
     * @param city1: Main city, the city where we draw the commute line from.
     * @param city2: City where the commute line goes to.
     * @param outcommuting: Total outcommuters on the line.
     * @param incommuting: Total incommuters on the line.
     * @param parent: Parent used for QML purposes.
     */
    explicit QTCommuter(QTCity *city1 = nullptr,QTCity *city2 = nullptr,double outcommuting = 0,double incommuting = 0);

    QTCommuter(const QTCommuter&)=delete;
    QTCommuter& operator=(const QTCommuter&)=delete;

    /// Default destructor.
    ~QTCommuter() override = default;
///@}

// Q Properties
/***********************************************************************************************************************/
/**
* @name Q Properties
* Q Properties of a Commuting line.
*/
    ///@{
    ///< The Coordinates of the center of city1 in latitude and longitude.
    Q_PROPERTY(QGeoCoordinate center_city1
                       MEMBER
                               m_center_city1
                       CONSTANT)

    ///< The Coordinate of the center of city2 in latitude and longitude.
    Q_PROPERTY(QGeoCoordinate center_city2
                       MEMBER
                               m_center_city2
                       CONSTANT)

    ///< The width of our line in the Gui Representation.
    Q_PROPERTY(double width
                       READ
                               CalculateLineWidth
                       CONSTANT)

    ///< Color the line should have in the Gui Representation.
    Q_PROPERTY(QColor color
                       READ
                               PickColor
                       CONSTANT)
///@}

// Modifiers
/***********************************************************************************************************************/
/**
* @name Modifers
* All modifiers of QTCommuter.
*/
///@{
    /// @brief Sets the in commuter count. Used for double commuter lines.
    /// @praram amount: The amount of in commuters to set.
    void set_in_commuters(double amount);
///@}

// Helpers
/***********************************************************************************************************************/
private:
/**
* @name helpers
* Helper functions of QTCommuter.
*/
    ///@{
    /// @brief  Calculates the Line Width used for visual representation.
    /// @return Returns the width of the line.
    double CalculateLineWidth() const;

    /// @brief Picks the color based on the line state.
    /// @return A QColor with the color representation of the line.
    QColor PickColor() const;
///@}

// DataMembers
/***********************************************************************************************************************/
private:
/**
* @name Datamembers
* All datamember.
*/
///@{
///< The model of the main city.
    QTCity *m_city1;

///< The model of the secondary city.
    QTCity *m_city2;

///< QGeoCoordinate of the center of the main city.
    QGeoCoordinate m_center_city1;

///< QGeoCoordinate of the center of the cecondary city.
    QGeoCoordinate m_center_city2;

///< OutCommuter total count.
    double m_out_count;

///< InCommuter total count.
    double m_in_count;
///@}

};
}
}




