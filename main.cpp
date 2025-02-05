#include "systemTab.h"
#include "processTab.h"
#include "resourceTab.h"
#include "fileSystemTab.h"
#include "CustomTableWidget.h"
#include "PropertiesWidget.h"
#include "OpenFiles.h"
#include "MemoryMapsWidget.h"
#include "CustomTableWidget.h"
#include "CustomTreeWidget.h"
#include "InfoDialog.h"

#include <iostream>
#include <string.h>
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QMessageBox>
#include <QActionGroup>
#include <QStackedWidget>

enum ViewType {
    TableView, 
    TreeView
};

class SystemMonitorWindow : public QMainWindow {
    private:
        bool isTreeView = false;
        QWidget *tableView;
        QWidget *treeView;
        QStackedWidget *stackedWidget;
        ViewType currentView;
        PropertiesWidget *propertiesWidget;
        MemoryMapsWidget *memoryMapsWidget;
        OpenFilesWidget *openFilesWidget;
        QAction *active_p;
        QAction *all_p;
        QAction *my_p;
        QAction *refresh;
        QAction *tree_view;
        QAction *table_view;
        ProcessFilter filter;

    public:

    SystemMonitorWindow() {
        setupUI();
    }
    void setupUI() {
        // Create the tab widget
        QTabWidget *tabWidget = new QTabWidget(this);
        stackedWidget = new QStackedWidget(this);

        // Initialize and use the properties widget
        propertiesWidget = new PropertiesWidget(this);
        memoryMapsWidget = new MemoryMapsWidget(this);
        openFilesWidget = new OpenFilesWidget(this);
        connectWidgets();

        // Create tabs
        QWidget *systemTab = createSystemTab();
        QWidget *resourceTab = new ResourcesTab();
        QWidget *fileSystemTab = createFileSystemTab();

        // Create process tab with a layout
        QWidget *processTab = new QWidget;
        QVBoxLayout *processLayout = new QVBoxLayout(processTab);

        // Create table and tree views
        tableView = createTableView();
        treeView = createTreeView();

        // Add both views to the layout, but only show one initially
        processLayout->addWidget(tableView);
        processLayout->addWidget(treeView);

        // Initialize the view state and update the view
        currentView = TableView;
        updateView();

        // Add tabs to the tab widget
        tabWidget->addTab(systemTab, tr("System"));
        tabWidget->addTab(processTab, tr("Processes"));
        tabWidget->addTab(resourceTab, tr("Resources"));
        tabWidget->addTab(fileSystemTab, tr("File Systems"));

        // Set the tab widget as the central widget
        // setCentralWidget(tabWidget);
        stackedWidget->addWidget(tabWidget);
        stackedWidget->addWidget(propertiesWidget);
        stackedWidget->addWidget(memoryMapsWidget);
        stackedWidget->addWidget(openFilesWidget);
        setCentralWidget(stackedWidget);

        // Set up the menu bar
        showMenuBar();

        CustomTableWidget *customTable = qobject_cast<CustomTableWidget*>(tableView);
        CustomTreeWidget *customTree = qobject_cast<CustomTreeWidget*>(treeView);
        if (customTable) {
            connect(customTable, &CustomTableWidget::propertiesSelected,
                    this, &SystemMonitorWindow::showProperties);
            connect(customTable, &CustomTableWidget::memoryMapSelected,
                    this, &SystemMonitorWindow::showMemoryMapsDialog);
            connect(customTable, &CustomTableWidget::openFilesSelected, 
                    this, &SystemMonitorWindow::showOpenFilesDialog);
        }

        if (customTree) {
            connect(customTree, &CustomTreeWidget::propertiesSelected,
                    this, &SystemMonitorWindow::showProperties);
            connect(customTree, &CustomTreeWidget::memoryMapSelected,
                    this, &SystemMonitorWindow::showMemoryMapsDialog);
            connect(customTable, &CustomTableWidget::openFilesSelected, 
                    this, &SystemMonitorWindow::showOpenFilesDialog);
        }
        connect(tabWidget, &QTabWidget::currentChanged, this, &SystemMonitorWindow::onTabChanged);
        
    }
    void showProperties(int pid) {
        propertiesWidget->setProcessPID(pid);
        stackedWidget->setCurrentWidget(propertiesWidget);
    }
    void showPropertiesDialog() {
        stackedWidget->setCurrentIndex(1);
    }
    void showOpenFilesDialog(int pid) {
        openFilesWidget->setProcessPID(pid);
        stackedWidget->setCurrentWidget(openFilesWidget);
    }
    void connectWidgets() {
        // Connect signals and slots for properties and memory maps widgets
        connect(propertiesWidget, &PropertiesWidget::closeRequested, this, &SystemMonitorWindow::showProcessView);
        connect(memoryMapsWidget, &MemoryMapsWidget::closeRequested, this, &SystemMonitorWindow::showProcessView);
        connect(openFilesWidget, &OpenFilesWidget::closeRequested, this, &SystemMonitorWindow::showProcessView);
    }

    void showMonitorInfoDialog() {
        InfoDialog dialog(ContentType::MonitorInfo, this);
        dialog.exec();
    }

    void showHelpDialog() {
        InfoDialog dialog(ContentType::Help, this);
        dialog.exec();
    }

    void connectDialogs(ContentType content) {

        switch (content) {
            case Help:
                showHelpDialog();
                break;
            case MonitorInfo:
                showMonitorInfoDialog();
                break;
        }
    }

    void onTabChanged(int index) {
        if (index == 1) {
            // Make specific actions visible
            active_p->setVisible(true);
            all_p->setVisible(true);
            my_p->setVisible(true);
            refresh->setVisible(true);
            tree_view->setVisible(true);
            table_view->setVisible(true);
        } else {
            // Hide these actions for other tabs
            active_p->setVisible(false);
            all_p->setVisible(false);
            my_p->setVisible(false);
            refresh->setVisible(false);
            tree_view->setVisible(false);
            table_view->setVisible(false);
        }

    }


    void showMemoryMapsDialog(int pid) {
        int ret = memoryMapsWidget->updateTable(pid);
        if (ret == 1) {
            QMessageBox::information(this, "Error: ", "Can't Display Process Information");
        } else {
            stackedWidget->setCurrentWidget(memoryMapsWidget);
        }
    }
    void showProcessView() {
        if (isTreeView) {
            currentView = TreeView;
        } else {
            currentView = TableView;
        }
        stackedWidget->setCurrentIndex(0);
        updateView();
    }

    void onRefreshTriggered() {
        // Assuming tableView is a pointer to your CustomTableWidget instance
        CustomTableWidget *customTableWidget = dynamic_cast<CustomTableWidget*>(tableView);
        CustomTreeWidget *customTreeWidget = dynamic_cast<CustomTreeWidget*>(treeView);
        if (customTableWidget && !isTreeView) {
            customTableWidget->refreshData(filter);
            QMessageBox::information(this, "Refresh Completed", "The table data has been refreshed.");
        } else if (customTreeWidget && isTreeView) {
            customTreeWidget->refreshData(filter);
            QMessageBox::information(this, "Refresh Completed", "The table data has been refreshed.");
        }

    }

    void showMenuBar() {
        QMenuBar *menuBar = new QMenuBar(this);

        // Create menus
        QMenu *monitor = menuBar->addMenu("&Monitor");
        QMenu *view = menuBar->addMenu("&View");
        QMenu *help = menuBar->addMenu("&Help");

        QActionGroup *helpActions = new QActionGroup(this);
        helpActions->setExclusive(true);

        QAction *howToUse = help->addAction("How to use");
        helpActions->addAction(howToUse);
        connect(howToUse, &QAction::triggered, [this]() { 
            this->connectDialogs(ContentType::Help); 
        });

        QActionGroup *monitorActions = new QActionGroup(this);
        QAction *info = monitor->addAction("Info");
        monitorActions->addAction(info);
        connect(info, &QAction::triggered, [this]() { 
            this->connectDialogs(ContentType::MonitorInfo); 
        });

        // Create an action group to ensure only one checkbox can be checked
        QActionGroup *viewGroup = new QActionGroup(this);
        viewGroup->setExclusive(true);

        active_p = view->addAction("&Active Processes");
        active_p->setCheckable(true);
        viewGroup->addAction(active_p);

        all_p = view->addAction("&All Processes");
        all_p->setCheckable(true);
        viewGroup->addAction(all_p);

        my_p = view->addAction("&My Processes");
        my_p->setCheckable(true);
        viewGroup->addAction(my_p); 

        tree_view = view->addAction("&Tree View");
        tree_view->setCheckable(true);
        viewGroup->addAction(tree_view);

        table_view = view->addAction("&Table View");
        table_view->setCheckable(true);
        viewGroup->addAction(table_view);
        
        refresh = view->addAction("&Refresh");
        refresh->setCheckable(true);
        viewGroup->addAction(refresh);

        // inital setup
        active_p->setVisible(false);
        all_p->setVisible(false);
        my_p->setVisible(false);
        refresh->setVisible(false);
        tree_view->setVisible(false);
        table_view->setVisible(false);

        connect(refresh, &QAction::triggered, this, &SystemMonitorWindow::onRefreshTriggered);
        connect(active_p, &QAction::triggered, this, &SystemMonitorWindow::onRefreshTriggered);
        connect(all_p, &QAction::triggered, this, &SystemMonitorWindow::onRefreshTriggered);
        connect(my_p, &QAction::triggered, this, &SystemMonitorWindow::onRefreshTriggered);


        // Connect actions to slots for functionality
        connect(viewGroup, &QActionGroup::triggered, this, &SystemMonitorWindow::optionToggled);

        setMenuBar(menuBar);
    }
    void updateView() {
        if (currentView == TableView) {
            tableView->show();
            treeView->hide();
            isTreeView = false;
        } else {
            treeView->show();
            tableView->hide();
            isTreeView = true;
        }
    }

    // Slot to handle checkable action toggling
    void optionToggled(QAction *action) {
        if (action) {
            qDebug() << action->text() << "toggled to" << action->isChecked();
            // Handle the action's toggled state
            if (action->text() == "&Tree View") {
                currentView = TreeView;
                updateView();
            } else if (action->text() == "&Table View") {
                currentView = TableView;
                updateView();
            } else if (action->text() == "&Active Processes") {
                filter = ProcessFilter::ActiveProcesses;
            } else if (action->text() == "&All Processes") {
                filter = ProcessFilter::AllProcesses;
            } else if (action->text() == "&My Processes") {
                filter = ProcessFilter::MyProcesses;
            }
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SystemMonitorWindow window;

    // width * height
    window.resize(600, 800);
    window.show();

    return app.exec();
}
