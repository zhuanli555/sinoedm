#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void on_buttonBox_accepted();
signals:
    void systemSetChanged();
private:
    Ui::SettingDialog *ui;
    QSettings* setting;
};

#endif // SETTINGDIALOG_H
