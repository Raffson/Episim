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
{    unsigned int schooled = 0;
    unsigned int youngsters = 0;
    unsigned int middleAged = 0;
    unsigned int toddlers = 0;
    unsigned int oldies = 0;
    unsigned int active = 0;
    Q_OBJECT
public:
    QTPopulation(unsigned int toddlers, unsigned int schooled, unsigned int youngsters,
                 unsigned int middleAged, unsigned int oldies, unsigned int actives);
    void VisualiseAll();

public slots:
    void ageDistributionClicked();

private:
    QMainWindow* m_window;
    QWidget* m_wdg;
    QVBoxLayout *m_vlay;
    QtCharts::QChartView *m_chartView;
    std::map <std::string, unsigned int>m_counter;


};


#endif //STRIDE_QTPOPULATION_H
