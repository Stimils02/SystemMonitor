#include "resourceTab.h"
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QRandomGenerator>
#include "graphs.h"
#include <string.h>
#include <QTimer>
#include <QDateTime>
#include <unistd.h>
#include <QThread>

double g_cpu_data_points[61];
double g_cpu_one_data[61];
double g_cpu_two_data[61];
double g_cpu_three_data[61];
double g_mem_data_points[61];
double g_swap_data_points[61];
double g_sent_data_points[61];
double g_received_data_points[61];

ResourcesTab::ResourcesTab(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    // Set up the charts
    setupCpuChart();
    setupMemoryChart();
    setupNetworkChart();

    // Add the chart views to the layout
    layout->addWidget(cpuChartView);
    layout->addWidget(memoryChartView);
    layout->addWidget(networkChartView);

    setLayout(layout);

    // Set up the update timer
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &ResourcesTab::updateGraphs);
    updateTimer->start(1000); // Update interval in milliseconds
}

ResourcesTab::~ResourcesTab() {
    // Clean up if necessary
}

void ResourcesTab::setupCpuChart() {
    // Create chart
    QChart *chart = new QChart();
    chart->setTitle("CPU Usage");
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Configure axis
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis;
    yAxis->setRange(0, 100);
    yAxis->setTitleText("Usage(%)");
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis;
    xAxis->setRange(0, 60);
    xAxis->setTitleText("Seconds");
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    // Set up series
    cpuSeries = new QLineSeries();
    cpuSeries->setName("Average CPU usage");
    chart->addSeries(cpuSeries);
    cpuSeries->attachAxis(yAxis);
    cpuSeries->attachAxis(xAxis);

    oneCpuSeries = new QLineSeries();
    oneCpuSeries->setName("CPU1");
    chart->addSeries(oneCpuSeries);
    oneCpuSeries->attachAxis(yAxis);
    oneCpuSeries->attachAxis(xAxis);

    twoCpuSeries = new QLineSeries();
    twoCpuSeries->setName("CPU2");
    chart->addSeries(twoCpuSeries);
    twoCpuSeries->attachAxis(yAxis);
    twoCpuSeries->attachAxis(xAxis);

    threeCpuSeries = new QLineSeries();
    threeCpuSeries->setName("CPU3");
    chart->addSeries(threeCpuSeries);
    threeCpuSeries->attachAxis(yAxis);
    threeCpuSeries->attachAxis(xAxis);

    cpuChartView = new QChartView(chart);
    cpuChartView->setRenderHint(QPainter::Antialiasing);
    //cpuChartView->setMinimumSize(800, 300);
}

void ResourcesTab::setupMemoryChart() {
    QChart *chart = new QChart();
    chart->setTitle("Memory and Swap History");
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Configure axis
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis;
    yAxis->setRange(0, 100);
    yAxis->setTitleText("Usage(%)");
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis;
    xAxis->setRange(0, 60);
    xAxis->setTitleText("Seconds");
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    memoryChartView = new QChartView(chart);
    memoryChartView->setRenderHint(QPainter::Antialiasing);

    memorySeries = new QLineSeries();
    memorySeries->setName("Memory");
    chart->addSeries(memorySeries);
    memorySeries->attachAxis(yAxis);
    memorySeries->attachAxis(xAxis);

    swapSeries = new QLineSeries();
    swapSeries->setName("Swap");
    chart->addSeries(swapSeries);
    swapSeries->attachAxis(yAxis);
    swapSeries->attachAxis(xAxis);
}

void ResourcesTab::setupNetworkChart() {
    QChart *chart = new QChart();
    chart->setTitle("Network History");
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Configure axis
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis;
    yAxis->setRange(0, 100);
    yAxis->setTitleText("KB/s");
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis;
    xAxis->setRange(0, 60);
    xAxis->setTitleText("Seconds");
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    networkChartView = new QChartView(chart);
    networkChartView->setRenderHint(QPainter::Antialiasing);

    receivedSeries = new QLineSeries();
    receivedSeries->setName("Receiving");
    chart->addSeries(receivedSeries);
    receivedSeries->attachAxis(yAxis);
    receivedSeries->attachAxis(xAxis);

    sentSeries = new QLineSeries();
    sentSeries->setName("Sending");
    chart->addSeries(sentSeries);
    sentSeries->attachAxis(yAxis);
    sentSeries->attachAxis(xAxis);
}

void ResourcesTab::updateGraphs() {
    // Update CPU usage data
    char *total_usage = get_cpu_usage();
    char *temp1 = total_usage;
    double specific_usage = 0;
    double average_usage = 0;
    int cpu_counter = 1;
    while (sscanf(temp1, "%lf", &specific_usage) == 1) {
        if (cpu_counter == 0) {
            oneCpuSeries->clear();
            for (int i = 60; i > 0; i--) {
                g_cpu_one_data[i] = g_cpu_one_data[i - 1];
            }
            g_cpu_one_data[0] = specific_usage;

            for (int i = 0; i < 61; i++) {
                oneCpuSeries->append(i, g_cpu_one_data[i]);
            }
        }
        if (cpu_counter == 1) {
            twoCpuSeries->clear();
            for (int i = 60; i > 0; i--) {
                g_cpu_two_data[i] = g_cpu_two_data[i - 1];
            }
            g_cpu_two_data[0] = specific_usage;

            for (int i = 0; i < 61; i++) {
                twoCpuSeries->append(i, g_cpu_two_data[i]);
            }
        }
        if (cpu_counter == 2) {
            threeCpuSeries->clear();
            for (int i = 60; i > 0; i--) {
                g_cpu_three_data[i] = g_cpu_three_data[i - 1];
            }
            g_cpu_three_data[0] = specific_usage;

            for (int i = 0; i < 61; i++) {
                threeCpuSeries->append(i, g_cpu_three_data[i]);
            }
        }
        average_usage += specific_usage;
        temp1 = strchr(temp1, ',');
        if (temp1 == NULL) {
            break;
        }
        temp1++;
        cpu_counter++;
        if (cpu_counter == 208) {
            cpu_counter = 0;
        }
    }
    average_usage /= 208;

    cpuSeries->clear();

    for (int i = 60; i > 0; i--) {
        g_cpu_data_points[i] = g_cpu_data_points[i - 1];
    }
    g_cpu_data_points[0] = average_usage;

    for (int i = 0; i < 61; i++) {
        cpuSeries->append(i, g_cpu_data_points[i]);
    }
    free(total_usage);
    total_usage = NULL;

    // Update Memory and Swap usage data
    char *mem_swp_info = get_memory_usage();
    char *temp2 = mem_swp_info;
    double mem_used;
    double mem_total;
    double swap_used;
    double swap_total;
    sscanf(temp2, "%lf,%lf", &mem_used, &mem_total);
    temp2 = strchr(temp2, '\n');
    if (temp2 == NULL)  {
        perror("temp is null");
    }
    temp2++;
    sscanf(temp2, "%lf,%lf", &swap_used, &swap_total);
    double mem_percentage = mem_used / mem_total * 100;
    double swap_percentage = swap_used / swap_total * 100;

    memorySeries->clear();

    for (int i = 60; i > 0; i--) {
        g_mem_data_points[i] = g_mem_data_points[i - 1];
    }
    g_mem_data_points[0] = mem_percentage;

    for (int i = 0; i < 61; i++) {
        memorySeries->append(i, g_mem_data_points[i]);
    }

    swapSeries->clear();

    for (int i = 60; i > 0; i--)  {
        g_swap_data_points[i] = g_swap_data_points[i - 1];
    }
    g_swap_data_points[0] = swap_percentage;

    for (int i = 0; i < 61; i++) {
        swapSeries->append(i, g_swap_data_points[i]);
    }

    free(mem_swp_info);
    mem_swp_info = NULL;

    // Update Network usage data
    char *net_info = get_network_usage();
    char *temp3 = net_info;
    double receive_speed;
    double send_speed;
    sscanf(temp3, "%lf,%lf", &receive_speed, &send_speed);

    receivedSeries->clear();

    for (int i = 60; i > 0; i--) {
        g_received_data_points[i] = g_received_data_points[i - 1];
    }
    g_received_data_points[0] = receive_speed;

    for (int i = 0; i < 61; i++) {
        receivedSeries->append(i, g_received_data_points[i]);
    }

    sentSeries->clear();

    for (int i = 60; i > 0; i--) {
        g_sent_data_points[i] = g_sent_data_points[i - 1];
    }
    g_sent_data_points[0] = send_speed;

    for (int i = 0; i < 61; i++) {
        sentSeries->append(i, g_sent_data_points[i]);
    }

    free(net_info);
    net_info = NULL;
}
