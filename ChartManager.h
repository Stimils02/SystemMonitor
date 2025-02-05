// ChartManager.h
#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>
#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

class ChartManager : public QObject {
    Q_OBJECT
public:
    explicit ChartManager(QObject *parent = nullptr);

    void setupCpuChart(QChart *chart);

private:
    // Additional members and functions as needed
};

#endif // CHARTMANAGER_H
