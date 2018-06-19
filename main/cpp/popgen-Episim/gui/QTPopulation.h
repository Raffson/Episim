//
// Created by Nishchal Shrestha on 16/06/2018.
//

#ifdef USING_QT

#ifndef STRIDE_QTPOPULATION_H
#define STRIDE_QTPOPULATION_H

#include <QObject>
#include <string>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <map>
#include <string>


class QTPopulation : public QObject
{
    Q_OBJECT
public:
    /// Constructor.
    QTPopulation(std::map <std::string, unsigned int>& popCounter,
        std::map <unsigned int, unsigned int>& householdCounter,
                 std::map <std::string, unsigned int>& workplaceCounter);

    /// Copy constructor.
    QTPopulation(const QTPopulation&);

    /// Assignment operator.
    QTPopulation& operator=(const QTPopulation&)=delete;

    /// Contains the button display all the charts.
    void VisualiseAll();

public slots:
    /// Displays bardiagrams for age distribution.
    void AgeDistributionClicked();

    /// Displays piechart for population density.
    void PopulationDensityClicked();

    /// Notify if workplace distribution is clicked and handles appropriately.
    void WorkplaceClicked();

    /// Notify if household distribution is clicked and handles appropriately.
    void HouseholdClicked();

private:
    /// Displays bardiagram.
    /// @param toManage: If workplace then displays the bardiagram of different types of workplaces,
    /// otherwise of different types of households.
    void Manage(std::string toManage);

private:
    QMainWindow* m_window; ///< The window where charts will appear.
    QWidget* m_wdg; ///< The widget.
    QVBoxLayout *m_vlay; ///< This will align widgets vertically.
    QtCharts::QChartView *m_chartView; ///< The chartview.

    std::map <std::string, unsigned int>m_pop_counter; ///< Holds the counts of different types of population based on age.
    std::map <unsigned int, unsigned int>m_household_counter;    ///< holds the number of households with the key describing
                                                                 ///  the number members in the household.
    std::map <std::string, unsigned int>m_workplace_counter; ///< Counter for different types of workplaces.

};


#endif //STRIDE_QTPOPULATION_H

#endif //USING_QT
