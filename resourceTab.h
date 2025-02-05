#ifndef RESOURCESTAB_H
#define RESOURCESTAB_H

#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class ResourcesTab : public QWidget {
    Q_OBJECT

public:
    explicit ResourcesTab(QWidget *parent = nullptr);
    ~ResourcesTab() override;

private slots:
    void updateGraphs();

private:
    QChartView *cpuChartView;
    QLineSeries *cpuSeries;
    QLineSeries *oneCpuSeries;
    QLineSeries *twoCpuSeries;
    QLineSeries *threeCpuSeries;
    QChartView *memoryChartView;
    QLineSeries *memorySeries;
    QLineSeries *swapSeries;
    QChartView *networkChartView;
    QLineSeries *receivedSeries;
    QLineSeries *sentSeries;
    QTimer *updateTimer;

    void setupCpuChart();
    void setupMemoryChart();
    void setupNetworkChart();
};

#endif // RESOURCESTAB_H
