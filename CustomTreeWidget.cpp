#include "CustomTreeWidget.h"
#include "PropertiesWidget.h"
#include "OpenFiles.h"
#include "process.h"
#include <QMenu>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>

extern int g_num_pids;

extern "C" {
    int* get_all_processes();
    int* get_my_processes();
    int* get_active_processes();
    char* get_process_name(int pid);
    char* get_status(int pid);
    char* get_cpu_info(int pid);
    char* get_memory(int pid);
    int kill_process(int pid);
    int* get_children_pids(int parent_pid, int &num_children);
}

CustomTreeWidget::CustomTreeWidget(QWidget *parent)
    : QWidget(parent), treeView(new QTreeView(this)), model(new QStandardItemModel(this)) {
    setupModelAndView();
    populateModelWithProcessData();
}

void CustomTreeWidget::setupModelAndView() {
    QStringList headers = {"Process Name", "Status", "% CPU", "ID", "Memory"};
    model->setHorizontalHeaderLabels(headers);

    treeView->setModel(model);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(treeView);

    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    treeView->header()->setStretchLastSection(true);
    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void CustomTreeWidget::populateModelWithProcessData() {
    int * pids = get_all_processes(); 
    if (pids != NULL) {
        for (int i = 0; i < g_num_pids; ++i) {
            QList<QStandardItem*> rowItems = createRowItems(pids[i]);
            if (rowItems.isEmpty()) {
                continue;
            }
            model->appendRow(rowItems);
            populateChildren(rowItems.first(), pids[i]);
        }
        free(pids);
        pids = NULL;
    }
}

void CustomTreeWidget::populateChildren(QStandardItem *parentItem, int pid) {
    int num_children = 0;
    int *childPids = get_children_pids(pid, num_children);
    if (childPids != NULL) {
        for (int i = 0; i < num_children; ++i) {
            QList<QStandardItem*> childRowItems = createRowItems(childPids[i]);
            if (childRowItems.isEmpty()) {
                continue;
            }
            parentItem->appendRow(childRowItems);
            populateChildren(childRowItems.first(), childPids[i]);
        }
        free(childPids);
    }
}

QList<QStandardItem*> CustomTreeWidget::createRowItems(int pid) {
    char *process_name = get_process_name(pid);
    if (process_name == NULL) {
        free(process_name);
        return QList<QStandardItem*>();
    }
    QString qprocess_name = process_name ? QString::fromUtf8(process_name) : QString();
    free(process_name);

    char *process_status = get_status(pid);
    QString qprocess_status = process_status ? QString::fromUtf8(process_status) : QString();
    free(process_status);

    char *process_cpu_info = get_cpu_info(pid);
    QString qprocess_cpu_info = process_cpu_info ? QString::fromUtf8(process_cpu_info) : QString();
    free(process_cpu_info);

    char *process_memory = get_memory(pid);
    QString qprocess_memory = process_memory ? QString::fromUtf8(process_memory) : QString();
    free(process_memory);

    return {
        new QStandardItem(qprocess_name),
        new QStandardItem(qprocess_status),
        new QStandardItem(qprocess_cpu_info),
        new QStandardItem(QString::number(pid)),
        new QStandardItem(qprocess_memory)
    };
}

void CustomTreeWidget::refreshData(ProcessFilter filter) {
    // Remove all rows from the model
    while (model->rowCount() > 0) {
        model->removeRow(0);
    }

    // Optionally, you can reset the headers here if needed
    QStringList headers = {"Process Name", "Status", "% CPU", "ID", "Memory"};
    model->setHorizontalHeaderLabels(headers);

    int *pids = NULL;
    switch (filter) {
        case ProcessFilter::AllProcesses:
            pids = get_all_processes();
            break;
        case ProcessFilter::MyProcesses:
            pids = get_my_processes();
            break;
        case ProcessFilter::ActiveProcesses:
            pids = get_active_processes();
            break;
        default:
            pids = get_all_processes();
            break;
    }

    if (pids != NULL) {
        for (int i = 0; i < g_num_pids; ++i) {
            QList<QStandardItem*> rowItems = createRowItems(pids[i]);
            if (rowItems.isEmpty()) {
                continue;
            }
            model->appendRow(rowItems);
            populateChildren(rowItems.first(), pids[i]);
        }
        free(pids);
    }
}

void CustomTreeWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu contextMenu(this);
    QAction *stopProcess = contextMenu.addAction("Stop Process");
    QAction *continueProcess = contextMenu.addAction("Continue Process");
    QAction *killProcess = contextMenu.addAction("Kill Process");
    QAction *memoryMaps = contextMenu.addAction("Memory Maps");
    QAction *openFiles = contextMenu.addAction("Open Files");
    QAction *propertiesAction = contextMenu.addAction("See Properties");

    QAction *selectedAction = contextMenu.exec(event->globalPos());
    QModelIndexList selectedIndexes = treeView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        return; // No item is selected
    }
    QModelIndex selectedIndex = selectedIndexes.first();

    // qDebug() << "Selected Index - Row:" << selectedIndex.row() << "Column:" << selectedIndex.column();
    // Assuming PID is stored as an integer
    int pid = selectedIndex.data().toInt();
    // qDebug() << "PID:" << pid;

    QString success;
    QString error;
    // int pid = 0;
    if (selectedAction == propertiesAction) {
        emit propertiesSelected(pid);
    } else if (selectedAction == memoryMaps) {
        emit memoryMapSelected(pid);
    } else if (selectedAction == killProcess) {
        int ret = kill_process(pid);
        success = "Process %1 has been killed.";
        error = "Failed to kill process: %1";
        QMessageBox::information(this, "Process Action", ret ? success.arg(pid) : error.arg(pid));
    } else if (selectedAction == continueProcess) {
        int ret = continue_process(pid);
        success = "Process %1 has been continued.";
        error = "Failed to resume process: %1";
        QMessageBox::information(this, "Process Action", ret ? success.arg(pid) : error.arg(pid));
    } else if (selectedAction == stopProcess) {
        int ret = stop_process(pid);
        success = "Process %1 has been stopped.";
        error = "Failed to stop process: %1";
        QMessageBox::information(this, "Process Action", ret ? success.arg(pid) : error.arg(pid));
    } else if (selectedAction == openFiles) {
        emit openFilesSelected(pid);
    }
}
