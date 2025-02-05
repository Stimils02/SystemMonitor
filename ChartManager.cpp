// ChartManager.cpp
#include "ChartManager.h"

ChartManager::ChartManager(QObject *parent) : QObject(parent) {
    // Initialization if needed
}

void ChartManager::setupCpuChart(QChart *chart) {
    // Create chart
    QChart *chart = new QChart();
    chart->setTitle("CPU Usage");

    // Configure axis
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis;
    yAxis->setRange(0, 100);
    yAxis->setTitleText("Usage (%)");
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis;
    xAxis->setRange(0, 60);
    xAxis->setTitleText("Seconds");
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    //QLineSeries *series = new QLineSeries();
    //chart->addSeries(series);

    cpuChartView = new QChartView(chart);
    cpuChartView->setRenderHint(QPainter::Antialiasing);

    // Parse CPU Usage and update graph every second
    int sec_counter = 0;
    double specific_usage = 0;
    cpuSeries = new QLineSeries();
    chart->addSeries(cpuSeries);
    cpuSeries->attachAxis(yAxis);
    cpuSeries->attachAxis(xAxis);
    while(1) {
        sleep(1);
        char *total_usage = get_cpu_usage();
        char *temp = total_usage;
        sscanf(total_usage, "%lf", &specific_usage);
        //while(sscanf(temp, "%lf", &specific_usage) == 1) {
            if (sec_counter > 60) {
                sec_counter %= 60;
            }

            cpuSeries->append(sec_counter, specific_usage);

            temp = strchr(temp, ',');
            if (temp == NULL) {
                break;
            }
            temp++;
        //}
        sec_counter++;
    }
}
