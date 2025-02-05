#include "fileSystemTab.h"
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTableWidget>
#include <QStringList>
#include <QHeaderView>
#include <QtWidgets>

int g_num_mounts = 0;
char ** g_device_path = NULL;
char ** g_directory_path = NULL;
char ** g_type = NULL;
char ** g_total = NULL;
char ** g_available = NULL;
char ** g_used = NULL;
char ** g_used_percent = NULL;


void free_file_system_info() {
    for (int i = 0; i < g_num_mounts; ++i) {
        free(g_device_path[i]);
        free(g_directory_path[i]);
        free(g_type[i]);
        free(g_total[i]);
        free(g_available[i]);
        free(g_used[i]);
        free(g_used_percent[i]);
    }
    free(g_device_path);
    free(g_directory_path);
    free(g_type);
    free(g_total);
    free(g_available);
    free(g_used);
    free(g_used_percent);
}

QWidget* createFileSystemTab() {
    auto *tab = new QWidget;
    auto *layout = new QVBoxLayout(tab);

    char *ret = get_all_file_system_info();
    free(ret);

    // Create a table widget
    auto *table = new QTableWidget();

    // Set the number of columns (for headers)
    table->setColumnCount(7);
    // Set the number of rows
    table->setRowCount(g_num_mounts);

    // Set headers
    QStringList headers;
    headers << "Device" << "Directory" << "Type" << "Total" << "Available" << "Used" << "Used%";
    table->setHorizontalHeaderLabels(headers);

    // Populate the table with data
    for (int row = 0; row < g_num_mounts; ++row) {
        table->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(g_device_path[row])));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(g_directory_path[row])));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(g_type[row])));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(g_total[row])));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(g_available[row])));
        table->setItem(row, 5, new QTableWidgetItem(QString::fromUtf8(g_used[row])));

        // Make graphic representation of used percent
        int percent = atoi(g_used_percent[row]);
        QProgressBar *progressBar = new QProgressBar();
        progressBar->setRange(0, 100);
        progressBar->setValue(percent);

        // Set up a custom widget with a QProgressBar
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(widget);
        layout->addWidget(progressBar);
        layout->setContentsMargins(0, 0, 0, 0);
        widget->setLayout(layout);
        
        // Set the custom widget as the item widget for the table
        QTableWidgetItem *itemWithProgressBar = new QTableWidgetItem;
        table->setItem(row, 6, itemWithProgressBar);
        table->setCellWidget(row, 6, widget);
    }

    table->setShowGrid(false);
    table->verticalHeader()->setVisible(false);
    table->resizeColumnsToContents();
    free_file_system_info();
    
    // Add the table to the layout
    layout->addWidget(table);

    tab->setLayout(layout);
    return tab;
}

/*
 * Helper function that parses components of file_system
 * Device(filesystem/path) Directory(path) Type Total(GiB) Available(GiB) Used(GiB) Used%
 * return 1 on success, 0 if no g_num_mounts
 */

int parse_components(char * file_system_buf) {

    // malloc according to g_num_mounts
    
    if (g_num_mounts == 0) {
        perror("no g_num_mounts");
        return 0;
    }

    g_device_path = (char **)malloc(sizeof(char*) * g_num_mounts);
    g_directory_path = (char **)malloc(sizeof(char*) * g_num_mounts);
    g_type = (char **)malloc(sizeof(char*) * g_num_mounts);
    g_total = (char **)malloc(sizeof(char*) * g_num_mounts);
    g_available = (char **)malloc(sizeof(char*) * g_num_mounts);
    g_used = (char **)malloc(sizeof(char*) * g_num_mounts);
    g_used_percent = (char **)malloc(sizeof(char*) * g_num_mounts);


    int buf_index = 0;

        // iterate for each of the mount points

        int i = 0;

        while ((file_system_buf[buf_index] != '\n') &&
               (file_system_buf[buf_index] != '\0')) {
            // parsing the file_system string

            int index = 0;
            g_device_path[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != ' ') {
                // device

                g_device_path[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_device_path[i][index] = '\0';
            buf_index++;

            index = 0;
            g_directory_path[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != ' ') {
                // directory_path

                g_directory_path[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_directory_path[i][index] = '\0';
            buf_index++;


            index = 0;
            g_type[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != ' ') {
                // type

                g_type[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_type[i][index] = '\0';
            buf_index++;


            index = 0;
            g_total[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != ' ') {
                // total

                g_total[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_total[i][index] = '\0';
            buf_index++;


            index = 0;
            g_available[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != ' ') {
                // total

                g_available[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_available[i][index] = '\0';
            buf_index++;


            index = 0;
            g_used[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != ' ') {
                // total

                g_used[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_used[i][index] = '\0';
            buf_index++;

            index = 0;
            g_used_percent[i] = (char *)malloc(sizeof(char) * 512);
            while (file_system_buf[buf_index] != '\n') {
                // total

                g_used_percent[i][index] = file_system_buf[buf_index];
                index++;
                buf_index++;
            }
            g_used_percent[i][index] = '\0';
            buf_index++;

            i++;

        }
            return 1;

    }


/*
 * Returns the all the file system info
 * for every single mount point on the system
 * in the format:
 * Device(filesystem/path) Directory(path) Type Total(GiB) Available(GiB) Used(GiB) Used%
 * 
 * adjusts g_num_mounts each time this is called so that you can iterate over 
 * the array returned from this function.
 */
char * get_all_file_system_info() {

    // available and used to calculate free space
    system("df -hT | awk 'NR > 1 {print $1 \" \" $7 \" \" $2 \" \" $3 \" \" $5 \" \" $4 \" \" $6}' > memory_maps/file_system.txt");

    int fd = open("memory_maps/file_system.txt", O_RDONLY);
    if (fd == -1) {
        close(fd);
        perror ("open");
        return NULL;
    }

    char * file_system_buf = (char*)malloc(sizeof(char) * FILE_SIZE * 4);
    size_t bytes_read = read(fd, file_system_buf, FILE_SIZE * 4);
    close(fd);

    file_system_buf[bytes_read] = '\0';

    if (bytes_read == -1) {
        perror("read");
        return NULL;
    }

    int index = 0;
    int num_mounts = 0;
    while (file_system_buf[index] != '\0') {
        if (file_system_buf[index] == '\n') {
            num_mounts++;
        }
        index++;
    }
    g_num_mounts = num_mounts;

    parse_components(file_system_buf);

    return file_system_buf;
}

