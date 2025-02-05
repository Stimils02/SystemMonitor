#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include "ProcessFilter.h"

// You need to define the ProcessInfo structure or include its definition
struct ProcessInfo {
    QString name;
    QString status;
    double cpuUsage;
    int pid;
    int memoryUsage;
};

class CustomTreeWidget : public QWidget {
    Q_OBJECT

public:
    explicit CustomTreeWidget(QWidget *parent = nullptr);
    void refreshData(ProcessFilter filter);

private:
    QTreeView *treeView;
    QStandardItemModel *model;

    void setupModelAndView();
    void populateModelWithProcessData();
    void populateChildren(QStandardItem *parentItem, int pid);
    QList<QStandardItem*> createRowItems(int pid);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void openPropertiesDialog(const QModelIndex &index);
    void openMemoryMapsDialog(const QModelIndex &index);
    void propertiesSelected(int pid);
    void memoryMapSelected(int pid);
    void openFilesSelected(int pid);
};

#endif // CUSTOMTREEWIDGET_H
