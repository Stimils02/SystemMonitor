#include "CustomTableWidget.h"
#include "CustomTreeWidget.h"
#include "PropertiesDialog.h"

#include <QApplication>
#include <QMainWindow>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTreeView>
#include <QStandardItemModel>

CustomTableWidget* createTableView() {
    return new CustomTableWidget();
}

QWidget* createTreeView() {
    return new CustomTreeWidget();
}
