#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class PropertiesWidget : public QWidget {
    Q_OBJECT

public:
    explicit PropertiesWidget(QWidget *parent = nullptr);
    void setProcessPID(int pid);

signals:
    void closeRequested();

private:
    int processPid;
    QVBoxLayout *layout;

    void clearContent();
    void updateProperties(int pid);
};

#endif // PROPERTIESWIDGET_H
