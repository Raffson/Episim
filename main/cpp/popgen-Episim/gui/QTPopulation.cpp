//
// Created by Nishchal Shrestha on 16/06/2018.
//

#include "QTPopulation.h"

#include <QApplication>
#include <QPushButton>
#include <iostream>
#include <string>

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

QTPopulation::QTPopulation(std::map <std::string, unsigned int>& popCounter,
   std::map <unsigned int, unsigned int>& householdCounter, std::map <std::string, unsigned int>& workplaceCounter)
        : m_pop_counter(popCounter), m_household_counter(householdCounter), m_workplace_counter(workplaceCounter)
{

}

void QTPopulation::VisualiseAll()
{

    QPushButton *populationDistributionButton = new QPushButton("Age distribution");
    QPushButton *populationDensityButton = new QPushButton("Population density");
    QPushButton *householdButton = new QPushButton("Household distribution");
    QPushButton *workplaceButton = new QPushButton("Workplace distribution");


    m_wdg = new QWidget();

    //all buttons are sorted vertically
    m_vlay = new QVBoxLayout(m_wdg);
    m_vlay->addWidget(populationDistributionButton);
    m_vlay->addWidget(populationDensityButton);
    m_vlay->addWidget(householdButton);
    m_vlay->addWidget(workplaceButton);


    m_wdg->setLayout(m_vlay);

    m_window = new QMainWindow;
    m_window->setCentralWidget(m_wdg);
    m_window->resize(250, 350);
    m_window->setWindowTitle(tr("Important characteristics of generated Population"));
    m_window->show();

    QObject::connect(populationDistributionButton, SIGNAL(clicked()), this, SLOT(ageDistributionClicked()));
    QObject::connect(householdButton, SIGNAL(clicked()), this, SLOT(householdClicked()));
    QObject::connect(workplaceButton, SIGNAL(clicked()), this, SLOT(workplaceClicked()));
    QObject::connect(populationDensityButton, SIGNAL(clicked()), this, SLOT(populationDensityClicked()));
}

void QTPopulation::householdClicked()
{
    manage("household");
}


void QTPopulation::workplaceClicked()
{
    manage("workplace");
}

void QTPopulation::manage(std::string toManage)
{
    //remove the old chart
    if(m_vlay->indexOf(m_chartView) != -1){
        m_vlay->removeWidget(m_chartView);
        delete m_chartView;
    }
    QStringList categories;
    QBarSeries *series = new QBarSeries();

    if(toManage == "household"){
        categories << "Number of members";
        for(auto& hh:m_household_counter){
            std::string keyvalue = std::to_string(hh.first);
            QString name = QString::fromStdString(keyvalue);
            QBarSet *setX = new QBarSet(name);
            *setX << hh.second;
            series->append(setX);
        }
    }

    else if(toManage == "workplace"){
        categories << "Type of workplace";
        for(auto& wp: m_workplace_counter){
            QString name = QString::fromStdString(wp.first);
            QBarSet *setX = new QBarSet(name);
            *setX << wp.second;
            series->append(setX);
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Effective size of " + QString::fromStdString(toManage));
    chart->setAnimationOptions(QChart::SeriesAnimations);


    QBarCategoryAxis *axis = new QBarCategoryAxis();
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


    *set0 << m_pop_counter["toddlers"];
    *set1 << m_pop_counter["schooled"];
    *set2 << m_pop_counter["youngsters"];
    *set3 << m_pop_counter["middleAged"];
    *set4 << m_pop_counter["oldies"];

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

    double total = (double )m_pop_counter["actives"] + m_pop_counter["toddlers"] + m_pop_counter["oldies"] +
            m_pop_counter["schooled"] + m_pop_counter["colleged"] + m_pop_counter["unemployed"];
    double actives = (double) m_pop_counter["actives"] /  total;
    double non_actives = (double) (m_pop_counter["toddlers"] + m_pop_counter["oldies"])/ total;
    double students = (double) (m_pop_counter["schooled"] + m_pop_counter["colleged"]) / total;
    double unemployed = (double) m_pop_counter["unemployed"] / total;

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