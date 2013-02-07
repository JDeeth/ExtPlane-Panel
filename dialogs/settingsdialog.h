#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QDebug>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent, QSettings *appSettings);
    ~SettingsDialog();
public slots:
    void loadSettings();
    void saveSettings();
protected:
    void changeEvent(QEvent *e);
signals:
    void rotationChanged(int r);
    void fullscreenChanged(bool fs);
    void simulateChanged(bool simulate);
    void setServerAddress(QString host);
private:
    Ui::SettingsDialog *ui;
    QSettings *appSettings;
};

#endif // SETTINGSDIALOG_H
