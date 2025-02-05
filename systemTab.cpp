#include "system_info.h"
#include <iostream>
#include <string.h>
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFont>


extern "C" char* get_os_release_version();
extern "C" char* get_kernel_version();
extern "C" char* get_amount_memory();
extern "C" char* get_processor_version();
extern "C" char* get_disk_storage();
extern "C" char* get_network_title();
extern "C" char* get_os_name();

QWidget* createSystemTab() {
    auto *tab = new QWidget;
    auto *mainlayout = new QVBoxLayout(tab);

    // Fetching the system information
    char *network_title = get_network_title();
    char *os_name = get_os_name();
    char *os_version = get_os_release_version();
    char *kernal_version = get_kernel_version();
    char *mem_amt = get_amount_memory();
    char *proc_version = get_processor_version();
    char *disk_storage = get_disk_storage();

    // Creating labels for the category names with bold font
    auto *networkLabel = new QLabel(QObject::tr("Network Title: "));
    QFont boldFont = networkLabel->font();
    boldFont.setBold(true);
    networkLabel->setFont(boldFont);
    auto *networkValue = new QLabel(QString::fromUtf8(network_title));

    auto *OSNameLabel = new QLabel(QObject::tr("OS Name: "));
    OSNameLabel->setFont(boldFont);
    auto *OSNameLabelValue = new QLabel(QString::fromUtf8(os_name));


    auto *OSVersionLabel = new QLabel(QObject::tr("OS Version: "));
    OSVersionLabel->setFont(boldFont);
    auto *OSVersionValue = new QLabel(QString::fromUtf8(os_version));
    
    auto *kernalVersionLabel = new QLabel(QObject::tr("Kernel Version: "));
    kernalVersionLabel->setFont(boldFont);
    auto *kernalVersionValue = new QLabel(QString::fromUtf8(kernal_version));
    
    auto *memAmountLabel = new QLabel(QObject::tr("Amount of Memory: "));
    memAmountLabel->setFont(boldFont);
    auto *memAmountValue = new QLabel(QString::fromUtf8(mem_amt));
    
    auto *processorVersionLabel = new QLabel(QObject::tr("Processor Version: "));
    processorVersionLabel->setFont(boldFont);
    auto *processorVersionValue = new QLabel(QString::fromUtf8(proc_version));
    
    auto *diskStorageLabel = new QLabel(QObject::tr("Disk Storage: "));
    diskStorageLabel->setFont(boldFont);
    auto *diskStorageValue = new QLabel(QString::fromUtf8(disk_storage));

    // Adding the labels to the layout
    mainlayout->addWidget(networkLabel);
    mainlayout->addWidget(networkValue);
    mainlayout->addWidget(OSNameLabel);
    mainlayout->addWidget(OSNameLabelValue);
    mainlayout->addWidget(OSVersionLabel);
    mainlayout->addWidget(OSVersionValue);
    mainlayout->addWidget(kernalVersionLabel);
    mainlayout->addWidget(kernalVersionValue);
    mainlayout->addWidget(memAmountLabel);
    mainlayout->addWidget(memAmountValue);
    mainlayout->addWidget(processorVersionLabel);
    mainlayout->addWidget(processorVersionValue);
    mainlayout->addWidget(diskStorageLabel);
    mainlayout->addWidget(diskStorageValue);

    // Freeing the char* variables
    free(os_version);
    free(kernal_version);
    free(mem_amt);
    free(proc_version);
    free(disk_storage);
    free(network_title);
    free(os_name);

    tab->setLayout(mainlayout);
    return tab;
}





