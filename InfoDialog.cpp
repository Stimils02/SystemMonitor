#include "InfoDialog.h"

InfoDialog::InfoDialog(ContentType type, QWidget *parent)
    : QDialog(parent), layout(new QVBoxLayout(this)), infoLabel(new QLabel(this)) {
    layout->addWidget(infoLabel);

    // Call the appropriate setup function based on the enum
    switch (type) {
        case ContentType::Help:
            createHelpDialog();
            break;
        case ContentType::MonitorInfo:
            createMonitorAboutDialog();
            break;
    }
}

void InfoDialog::createMonitorAboutDialog() {
    setWindowTitle("About Monitor - SystemMonitor");

    QVBoxLayout *mainLayout = layout; // Initialize the QVBoxLayout with 'this' to associate it with the current dialog
    mainLayout->setContentsMargins(mainLayout->contentsMargins().left(), 0, mainLayout->contentsMargins().right(), mainLayout->contentsMargins().bottom());

    QLabel *purposeLabel = new QLabel("<b>Purpose of the SystemMonitor:</b>", this);
    mainLayout->addWidget(purposeLabel);

    QLabel *purposeInfoLabel = new QLabel(
        "SystemMonitor is designed to provide real-time visibility into system performance and resource usage.\n "
        "It allows users to monitor processes, view resource consumption, and manage system state effectively.\n"
        "In addition, users have the ability to use the refresh button in the view menu in order to see an update\n"
        "of all the changes. We will show popups and dialogs whenever some action is requested so please be patient.\n", this);
    mainLayout->addWidget(purposeInfoLabel);

    QLabel *creatorsLabel = new QLabel("<b>Creators:</b>", this);
    mainLayout->addWidget(creatorsLabel);

    QLabel *creator1Label = new QLabel("1. <i>Sujal Timilsina</i> - Senior", this);
    mainLayout->addWidget(creator1Label);

    QLabel *creator2Label = new QLabel("2. <i>Josh Tseng</i> - Junior", this);
    mainLayout->addWidget(creator2Label);

    QLabel *creator3Label = new QLabel("3. <i>Alexandre Cunha Moraes</i> - Junior", this);
    mainLayout->addWidget(creator3Label);

    resize(400, 300);
}


void InfoDialog::createHelpDialog() {
    setWindowTitle("Monitor About - Tab Information");

    QVBoxLayout *mainLayout = layout;
    mainLayout->setContentsMargins(mainLayout->contentsMargins().left(), 0, mainLayout->contentsMargins().right(), mainLayout->contentsMargins().bottom());

    QLabel *systemTabLabel = new QLabel(
        "<b>&lt;=========================== SYSTEM TAB ===========================&gt;</b><br><br>"
        "The System Tab contains information about the system.<br>"
        "Please visit that tab if you need information about such System.<br><br>"
        "<b>&lt;=========================== SYSTEM TAB ===========================&gt;</b><br><br>", this);
    mainLayout->addWidget(systemTabLabel);

    QLabel *processesTabLabel = new QLabel(
        "<b>&lt;=========================== Process TAB ===========================&gt;</b><br><br>"
        "The Process Tab contains information about all the running processes.<br>"
        "In the Process Tab, you can toggle various options from the view option only availably in Process Tab.<br>"
        "Options Include:<br><br>"
        "    - See Active Processes<br><br>"
        "    - See All Processes<br><br>"
        "    - See User Processes<br><br>"
        "    - Toggle Tree View<br><br>"
        "    - Toggle Table View<br><br>"
        "    - Refresh<br><br>"
        "<b>&lt;=========================== Process TAB ===========================&gt;</b><br><br>", this);
    mainLayout->addWidget(processesTabLabel);

    QLabel *resourcesTabLabel = new QLabel(
        "<b>&lt;=========================== Resources TAB ===========================&gt;</b><br><br>"
        "The Resources Tab provides details about the system's resource usage such as CPU, memory, and disk usage.<br>"
        "You can view different graphs and get a real-time update of the resources being consumed by your system.<br><br>"
        "<b>&lt;=========================== Resources TAB ===========================&gt;</b><br><br>", this);
    mainLayout->addWidget(resourcesTabLabel);

    // File Systems Tab label with HTML formatting
    QLabel *fileSystemsTabLabel = new QLabel(
        "<b>&lt;=========================== File Systems TAB ===========================&gt;</b><br><br>"
        "The File Systems Tab shows information regarding the various file systems mounted on the system.<br>"
        "It includes details like total size, used space, available space, and file system type.<br><br>"
        "<b>&lt;=========================== File Systems TAB ===========================&gt;</b><br>", this);
    
    mainLayout->addWidget(fileSystemsTabLabel);


    resize(400, 300);
}
