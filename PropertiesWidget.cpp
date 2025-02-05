#include "PropertiesWidget.h"
#include "process.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <iostream>
#include <vector>
#include <string>

// Assuming these are external C functions
extern "C" {
    char* get_process_name(int pid);
    char* get_current_user();
    char* get_status(int pid);
    char* get_memory(int pid);
    char* get_process_vm_size(int pid);
    char* get_shared_memory(int pid);
    char* get_cpu_time(int pid);
    char* get_start_date_time(int pid);
}

PropertiesWidget::PropertiesWidget(QWidget *parent)
    : QWidget(parent), layout(new QVBoxLayout(this)) {
    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &PropertiesWidget::closeRequested);
    layout->addWidget(closeButton);
}

void PropertiesWidget::setProcessPID(int pid) {
    processPid = pid;
    updateProperties(pid); // Update properties whenever PID is set
}

void PropertiesWidget::clearContent() {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if(item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void PropertiesWidget::updateProperties(int pid) {
    clearContent();

    // Fetch and update the process properties
    char *process_name = get_process_name(pid);
    char *user = get_current_user();
    char *status = get_status(pid);
    char *memory = get_memory(pid);
    char *vm = get_process_vm_size(pid);
    char *sm = get_shared_memory(pid);
    char *cpu_time = get_cpu_time(pid);
    char *start_date = get_start_date_time(pid);

    std::vector<std::pair<QString, QString>> properties = {
        {"Process Name", QString::fromUtf8(process_name)},
        {"User", QString::fromUtf8(user)},
        {"State", QString::fromUtf8(status)},
        {"Memory", QString::fromUtf8(memory)},
        {"Virtual Memory", QString::fromUtf8(vm)},
        {"Shared Memory", QString::fromUtf8(sm)},
        {"CPU Time", QString::fromUtf8(cpu_time)},
        {"Date/Time Started", QString::fromUtf8(start_date)}
    };

    for (const auto &property : properties) {
        QLabel *label = new QLabel(property.first + ": " + property.second, this);
        layout->addWidget(label);
    }

    // Free the allocated memory
    free(process_name);
    free(user);
    free(status);
    free(memory);
    free(vm);
    free(sm);
    free(cpu_time);
    free(start_date);
    
    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &PropertiesWidget::closeRequested);
    layout->addWidget(closeButton);

    // Update the layout
    layout->update();
}
