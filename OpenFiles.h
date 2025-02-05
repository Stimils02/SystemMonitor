#ifndef OPENFILES_H
#define OPENFILES_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include "process.h"

class OpenFilesWidget : public QWidget {
    Q_OBJECT
public:
    explicit OpenFilesWidget(QWidget *parent = nullptr);
    void setProcessPID(int pid);
    void clearContent();
    void updateOpenFiles();
signals:
    void closeRequested();
private:
    int processPid;
    QVBoxLayout *layout;
    QTableWidget *table; // Declare the table here
};

#endif // OPENFILES_H
