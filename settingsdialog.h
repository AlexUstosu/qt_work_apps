#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QPushButton>
#include <QSerialPort>
#include <QString>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
signals:
    void signalSaveSettings(int, int, int, int, int);
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
