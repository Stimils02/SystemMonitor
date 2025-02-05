#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include "ContentType.h" // Include the ContentType enum

class InfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit InfoDialog(ContentType type, QWidget *parent = nullptr);

private:
    void createHelpDialog();
    void createMonitorAboutDialog();
    QVBoxLayout *layout;
    QLabel *infoLabel;
};

#endif // INFODIALOG_H
