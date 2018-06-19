//
// Created by Nishchal Shrestha on 16/06/2018.
//

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
    QTPopulation(std::map <std::string, unsigned int>& popCounter,
        std::map <unsigned int, unsigned int>& householdCounter,
                 std::map <std::string, unsigned int>& workplaceCounter);
    void VisualiseAll();

public slots:
    void ageDistributionClicked();
    void populationDensityClicked();
    void workplaceClicked();
    void householdClicked();

private:
    void manage(std::string toManage);

private:
    QMainWindow* m_window;
    QWidget* m_wdg;
    QVBoxLayout *m_vlay;
    QtCharts::QChartView *m_chartView;
    std::map <std::string, unsigned int>m_pop_counter;
    std::map <unsigned int, unsigned int>m_household_counter;
    std::map <std::string, unsigned int>m_workplace_counter;



};


#endif //STRIDE_QTPOPULATION_H
