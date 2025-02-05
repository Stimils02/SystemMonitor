#ifndef MEMORYMAPSWIDGET_H
#define MEMORYMAPSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

class MemoryMapsWidget : public QWidget {
    Q_OBJECT

public:
    explicit MemoryMapsWidget(QWidget *parent = nullptr);
    int updateTable(int pid);

signals:
    void closeRequested();

private:
    QTableWidget *table;
    QVBoxLayout *layout;
    int processPid;

    // Function to setup the initial layout and table structure
    void setupTable();
};

#endif // MEMORYMAPSWIDGET_H
