#include "MemoryMapsWidget.h"
#include <QStringList>
#include <QHeaderView>

// Include headers for functions that provide process data
extern "C" {
    char* get_memory_maps(int pid);
    char** parse_memory_maps(char* big_map, int& num_lines);
    char* get_file_name(char* line);
    char* get_vm_start(char* line);
    char* get_vm_end(char* line);
    char* get_vm_size(char* line);
    char* get_flags(char* line);
    char* get_vm_offset(char* line);
    void free_and_nullify(void** ptr);
}

MemoryMapsWidget::MemoryMapsWidget(QWidget *parent)
    : QWidget(parent), table(new QTableWidget(this)), layout(new QVBoxLayout(this)), processPid(-1) {
    // Set up the table with headers but no rows
    QStringList headers = {"Filename", "VM Start", "VM End", "VM Size", "Flags", "VM Offset"};
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setShowGrid(false);
    table->verticalHeader()->setVisible(false);

    // Add table and close button to layout
    layout->addWidget(table);
    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &MemoryMapsWidget::closeRequested);
    layout->addWidget(closeButton);
}

int MemoryMapsWidget::updateTable(int pid) {
    processPid = pid;
    // table->setRowCount(10);

    // Fetch the memory map for the process
    char *big_map = get_memory_maps(processPid);
    if (big_map == NULL) {
        return 1;
    }
    int num_lines = 0;
    char **parsed_map = parse_memory_maps(big_map, num_lines);

    // Clear existing data in the table
    table->setRowCount(0);
    table->setRowCount(num_lines);

    for (int row = 0; row < num_lines; ++row) {
        // Fetch data for each column from the parsed map
        char *file_name = get_file_name(parsed_map[row]);
        char *vm_start = get_vm_start(parsed_map[row]);
        char *vm_end = get_vm_end(parsed_map[row]);
        char *vm_size = get_vm_size(parsed_map[row]);
        char *flags = get_flags(parsed_map[row]);
        char *vm_offset = get_vm_offset(parsed_map[row]);
        
        // Create and set QTableWidgetItem for each column
        table->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(file_name)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(vm_start)));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(vm_end)));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(vm_size)));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(flags)));
        table->setItem(row, 5, new QTableWidgetItem(QString::fromUtf8(vm_offset)));
        
        // Free memory for each line after use
        free_and_nullify((void **)&file_name);
        free_and_nullify((void **)&vm_start);
        free_and_nullify((void **)&vm_end);
        free_and_nullify((void **)&vm_size);
        free_and_nullify((void **)&flags);
        free_and_nullify((void **)&vm_offset);
    }

    // Free the big map and parsed map memory
    free_and_nullify((void **)&big_map);
    for (int i = 0; i < num_lines; ++i) {
        free_and_nullify((void **)&parsed_map[i]);
    }
    free_and_nullify((void **)&parsed_map);
    return 0;
}
