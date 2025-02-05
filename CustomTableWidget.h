#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QTableWidget>
#include <QContextMenuEvent>
#include "ProcessFilter.h"

class CustomTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit CustomTableWidget(QWidget *parent = nullptr);
    void refreshData(ProcessFilter filter);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void openPropertiesDialog(const QModelIndex &index);
    void openMemoryMapsDialog(const QModelIndex &index);
    void propertiesSelected(int pid);
    void memoryMapSelected(int pid);
    void openFilesSelected(int pid);
};

#endif // CUSTOMTABLEWIDGET_H
