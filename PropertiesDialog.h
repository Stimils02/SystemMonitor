#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class PropertiesDialog : public QDialog {
    Q_OBJECT
public:
    explicit PropertiesDialog(QWidget *parent = nullptr);
    void setProperties(const QString &propertiesText);

private:
    QLabel *propertiesLabel;
};

#endif // PROPERTIESDIALOG_H
