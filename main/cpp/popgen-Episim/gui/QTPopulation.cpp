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

QTPopulation::QTPopulation(unsigned int toddlers, unsigned int schooled, unsigned int youngsters,
                           unsigned int middleAged, unsigned int oldies, unsigned int actives)
{
    m_counter["toddlers"] = toddlers;
    m_counter["schooled"] = schooled;
    m_counter["youngsters"] = youngsters;
    m_counter["middleAged"] = middleAged;
    m_counter["oldies"] = oldies;
    m_counter["actives"] = actives;

}

void QTPopulation::VisualiseAll()
{

    int argc;
    char *argv[] = {};
    QApplication app(argc, argv);

    QPushButton *populationDistributionButton = new QPushButton("Age distribution");


    m_wdg = new QWidget();

    //all buttons are sorted vertically
    m_vlay = new QVBoxLayout(m_wdg);
    m_vlay->addWidget(populationDistributionButton);


    m_wdg->setLayout(m_vlay);

    m_window = new QMainWindow;
    m_window->setCentralWidget(m_wdg);
    m_window->resize(250, 300);
    m_window->setWindowTitle(tr("Important characteristics of generated Population"));
    m_window->show();

    QObject::connect(populationDistributionButton, SIGNAL(clicked()), this, SLOT(ageDistributionClicked()));
    //QObject::connect(householdButton, SIGNAL(clicked()), this, SLOT(householdClicked()));
    //QObject::connect(workplaceButton, SIGNAL(clicked()), this, SLOT(workplaceClicked()));
    //QObject::connect(populationDensityButton, SIGNAL(clicked()), this, SLOT(populationDensityClicked()));

    app.exec();

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

    m_window->resize(800, 600);
    m_window->show();
}