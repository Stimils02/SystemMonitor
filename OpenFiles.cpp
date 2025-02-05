#include "OpenFiles.h"
#include <QPushButton>
#include <QHeaderView>


// Assuming this is an external C function
extern "C" {
    char* list_open_file(int pid);
}
extern char ** g_open_fds;
extern char ** g_open_types;
extern char ** g_open_paths;
extern int g_num_open_files;

OpenFilesWidget::OpenFilesWidget(QWidget *parent)
    : QWidget(parent), layout(new QVBoxLayout(this)), table(new QTableWidget(this)) {
    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &OpenFilesWidget::closeRequested);

    // Call these methods on the 'table' object
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);

    layout->addWidget(table);
    layout->addWidget(closeButton);
}

void OpenFilesWidget::setProcessPID(int pid) {
    processPid = pid;
    updateOpenFiles();
}

void OpenFilesWidget::clearContent() {
    table->clearContents();
    table->setRowCount(0);
}

void OpenFilesWidget::updateOpenFiles() {
    clearContent();

    table->setColumnCount(3);
    QStringList headers;
    headers << "FD" << "Type" << "Object";
    table->setHorizontalHeaderLabels(headers);

    char *lst = list_open_file(processPid);
    int num_open = parse_list(lst); // Assuming this function populates the global arrays

    for (int i = 0; i < num_open; i++) {
        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(g_open_fds[i])));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(g_open_types[i])));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(g_open_paths[i])));

        // Free each string in the arrays
        free(g_open_fds[i]);
        free(g_open_types[i]);
        free(g_open_paths[i]);
    }

    // Free the arrays themselves
    free(g_open_fds);
    free(g_open_types);
    free(g_open_paths);

    // Free the memory allocated by list_open_file
    free(lst);
}
