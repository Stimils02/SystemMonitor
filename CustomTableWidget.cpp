#include "CustomTableWidget.h"
#include "PropertiesWidget.h"
#include "OpenFiles.h"
#include "process.h"
#include <QMenu>
#include <QMessageBox>
#include <QHeaderView>
#include <iostream>
#include <string.h>

extern "C" {
    int* get_all_processes();
    int* get_my_processes();
    int* get_active_processes();
    char* get_process_name(int pid);
    char* get_status(int pid);
    char* get_cpu_info(int pid);
    char* get_memory(int pid);
    int kill_process(int pid);
}

extern int g_num_pids;

CustomTableWidget::CustomTableWidget(QWidget *parent)
    : QTableWidget(parent) {
    setColumnCount(5);

    // Set the headers for the columns
    QStringList headers;
    headers << "Process Name" << "Status" << "% CPU" << "ID" << "Memory";
    setHorizontalHeaderLabels(headers);

    int *pids = get_all_processes();
    if (pids == NULL) {
        std::cout << "Failed to get PIDs" << std::endl;
    }

    if (pids != NULL) {
        int rowCount = 0;
        setRowCount(g_num_pids); // set this back to normal
        printf("Pid amt: %d\n", 5);
        for (int i = 0; i < g_num_pids; ++i) {
            char *process_name = get_process_name(pids[i]);
            if (process_name == NULL) {
                free(process_name);
                continue;
            }
            QString qprocess_name = QString(process_name);

            char *process_status = get_status(pids[i]);
            QString qprocess_status = QString(process_status);

            char *process_cpu_info = get_cpu_info(pids[i]);
            QString qprocess_cpu_info = QString(process_cpu_info);

            int pid = pids[i];
            QString process_id = QString::number(pid);
            char *process_memory = get_memory(pids[i]);
            QString qprocess_memory;
            qprocess_memory = QString(process_memory);

            // if (process_memory[0] == 'k' || strcmp(process_status, "Zombie") == 0 || strcmp(process_status, "Stopped") == 0) {
            //     qprocess_memory = "0 kB";
            // } else {
            //     qprocess_memory = QString(process_memory);
            //     qprocess_memory += " kB";
            // }

            setItem(rowCount, 0, new QTableWidgetItem(qprocess_name));
            setItem(rowCount, 1, new QTableWidgetItem(process_status));
            setItem(rowCount, 2, new QTableWidgetItem(qprocess_cpu_info));
            setItem(rowCount, 3, new QTableWidgetItem(process_id));
            setItem(rowCount, 4, new QTableWidgetItem(qprocess_memory));
            free(process_name);
            free(process_status);
            free(process_cpu_info);
            free(process_memory);

            rowCount++;
        }
        free(pids);
    }

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setShowGrid(false);
    verticalHeader()->setVisible(false);
}

void CustomTableWidget::refreshData(ProcessFilter filter) {
    setRowCount(0);

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

    // int *pids = get_all_processes();
    if (pids == NULL) {
        std::cout << "Failed to get PIDs" << std::endl;
        return;
    }

    int rowCount = 0;
    setRowCount(g_num_pids); // set this back to normal
    printf("Pid amt: %d\n", 5);
    for (int i = 0; i < g_num_pids; ++i) {
        char *process_name = get_process_name(pids[i]);
        if (process_name == NULL) {
            free(process_name);
            continue;
        }
        QString qprocess_name = QString(process_name);

        char *process_status = get_status(pids[i]);
        QString qprocess_status = QString(process_status);

        char *process_cpu_info = get_cpu_info(pids[i]);
        QString qprocess_cpu_info = QString(process_cpu_info);

        int pid = pids[i];
        QString process_id = QString::number(pid);
        char *process_memory = get_memory(pids[i]);
        QString qprocess_memory;
        qprocess_memory = QString(process_memory);

        setItem(rowCount, 0, new QTableWidgetItem(qprocess_name));
        setItem(rowCount, 1, new QTableWidgetItem(process_status));
        setItem(rowCount, 2, new QTableWidgetItem(qprocess_cpu_info));
        setItem(rowCount, 3, new QTableWidgetItem(process_id));
        setItem(rowCount, 4, new QTableWidgetItem(qprocess_memory));
        free(process_name);
        free(process_status);
        free(process_cpu_info);
        free(process_memory);
        rowCount++;
    }


    free(pids);
    pids = NULL;

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setShowGrid(false);
    verticalHeader()->setVisible(false);
}

void CustomTableWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu contextMenu(this);
    QAction *stopProcess = contextMenu.addAction("Stop Process");
    QAction *continueProcess = contextMenu.addAction("Continue Process");
    QAction *killProcess = contextMenu.addAction("Kill Process");
    QAction *memoryMaps = contextMenu.addAction("Memory Maps");
    QAction *openFiles = contextMenu.addAction("Open Files");
    QAction *propertiesAction = contextMenu.addAction("See Properties");
    QAction *selectedAction = contextMenu.exec(event->globalPos());

    QTableWidgetItem *clickedItem = this->itemAt(event->pos());
    if (!clickedItem) {
        return;
    }

    QPoint pos = event->pos();
    int visualRow = this->rowAt(pos.y());
    if (visualRow == -1) {
        return;
    }

    // Retrieve the model index for the clicked row
    QModelIndex modelIndex = this->model()->index(visualRow, 3);
    int pid = this->model()->data(modelIndex).toInt();

    QString success;
    QString error;

    if (selectedAction == propertiesAction) {
        // emit openPropertiesDialog(indexAt(event->pos()));
        emit propertiesSelected(pid);
    } else if (selectedAction == memoryMaps) {
        emit memoryMapSelected(pid);
        // emit openMemoryMapsDialog(indexAt(event->pos()));
    } else if (selectedAction == killProcess) {
        int ret = kill_process(pid);
        success  = "Process %1 has been killed.";
        error = "Failed to kill process: %1";
        if (ret) {
            QMessageBox::information(this, "Process Action", success.arg(pid));
        } else {
            QMessageBox::information(this, "Process Action", error.arg(pid)); 
        }
    } else if (selectedAction == continueProcess) {
        int ret = continue_process(pid);
        success = "Process %1 has been continued.";
        error = "Failed to resume process: %1";
        if (ret) {
            QMessageBox::information(this, "Process Action", success.arg(pid));
        } else {
            QMessageBox::information(this, "Process Action", error.arg(pid)); 
        }
    } else if (selectedAction == stopProcess) {
        int ret = stop_process(pid);
        success = "Process %1 has been stopped.";
        error = "Failed to stop process: %1";
        if (ret) {
            QMessageBox::information(this, "Process Action", success.arg(pid));
        } else {
            QMessageBox::information(this, "Process Action", error.arg(pid)); 
        }
    } else if (selectedAction == openFiles) {
        emit openFilesSelected(pid);
    }
}

