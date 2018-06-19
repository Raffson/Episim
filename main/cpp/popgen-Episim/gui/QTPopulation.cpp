//
// Created by Nishchal Shrestha on 16/06/2018.
//

#include "QTPopulation.h"

#include <QApplication>
#include <QPushButton>
#include <iostream>

#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QPushButton>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

QTPopulation::QTPopulation(unsigned int toddlers, unsigned int schooled,  unsigned int colleged, unsigned int youngsters,
                           unsigned int middleAged, unsigned int oldies, unsigned int actives, unsigned int unemployed)
{
    m_counter["toddlers"] = toddlers;
    m_counter["schooled"] = schooled;
    m_counter["colleged"] = colleged;
    m_counter["youngsters"] = youngsters;
    m_counter["middleAged"] = middleAged;
    m_counter["oldies"] = oldies;
    m_counter["actives"] = actives;
    m_counter["unemployed"] = unemployed;
    m_total_pop = toddlers + schooled + youngsters + middleAged + oldies;

}

void QTPopulation::VisualiseAll()
{

    QPushButton *populationDistributionButton = new QPushButton("Age distribution");
    QPushButton *populationDensityButton = new QPushButton("Population density");


    m_wdg = new QWidget();

    //all buttons are sorted vertically
    m_vlay = new QVBoxLayout(m_wdg);
    m_vlay->addWidget(populationDistributionButton);
    m_vlay->addWidget(populationDensityButton);


    m_wdg->setLayout(m_vlay);

    m_window = new QMainWindow;
    m_window->setCentralWidget(m_wdg);
    m_window->resize(250, 400);
    m_window->setWindowTitle(tr("Important characteristics of generated Population"));
    m_window->show();

    QObject::connect(populationDistributionButton, SIGNAL(clicked()), this, SLOT(ageDistributionClicked()));
    //QObject::connect(householdButton, SIGNAL(clicked()), this, SLOT(householdClicked()));
    //QObject::connect(workplaceButton, SIGNAL(clicked()), this, SLOT(workplaceClicked()));
    QObject::connect(populationDensityButton, SIGNAL(clicked()), this, SLOT(populationDensityClicked()));
}

void QTPopulation::ageDistributionClicked()
{

    if(m_vlay->indexOf(m_chartView) != -1){
        m_vlay->removeWidget(m_chartView);
        delete m_chartView;
    }

    QBarSet* set0 = new QBarSet("Toddlers(0-2)");
    QBarSet* set1 = new QBarSet("School students(3-17)");
    QBarSet* set2 = new QBarSet("Youngsters(18-25)");
    QBarSet* set3 = new QBarSet("Middle aged(26-65)");
    QBarSet* set4 = new QBarSet("Old aged(65+)");


    *set0 << m_counter["toddlers"];
    *set1 << m_counter["schooled"];
    *set2 << m_counter["youngsters"];
    *set3 << m_counter["middleAged"];
    *set4 << m_counter["oldies"];

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Age Distribution");
    chart->setAnimationOptions(QChart::SeriesAnimations);


    QBarCategoryAxis *axis = new QBarCategoryAxis();
    QStringList categories;
    categories << "";
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartView = new QChartView(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_vlay->addWidget(m_chartView);

    m_window->resize(800, 900);
    m_window->show();
}

void QTPopulation::populationDensityClicked()
{
    //remove the old chart if it exists
    if(m_vlay->indexOf(m_chartView) != -1){
        m_vlay->removeWidget(m_chartView);
        delete m_chartView;
    }

    double actives = (double) m_counter["actives"] / m_total_pop;
    double non_actives = (double) (m_counter["toddlers"] + m_counter["oldies"])/ m_total_pop;
    double students = (double) (m_counter["schooled"] + m_counter["colleged"]) / m_total_pop;
    double unemployed = (double) m_counter["unemployed"] / m_total_pop;

    QPieSeries *series = new QPieSeries();
    series->append("active(" + QString::number(actives*100.0) + " %)", 1);
    series->append("non-actives("+ QString::number(non_actives*100.0) + "%)", 2);
    series->append("students(" + QString::number(students*100.0) + "%)", 3);
    series->append("unemployed(" + QString::number(unemployed*100.0) + "%)", 4);

    series->slices().at(0)->setValue(actives);
    series->slices().at(1)->setValue(non_actives);
    series->slices().at(2)->setValue(students);
    series->slices().at(3)->setValue(unemployed);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Population density");

    m_chartView = new QChartView(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_vlay->addWidget(m_chartView);

    m_window->resize(800, 800);
    m_window->show();

}