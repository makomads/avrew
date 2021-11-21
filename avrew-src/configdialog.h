#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QSettings>

namespace Ui {
class ConfigDialog;
}

enum{
    PINFUNC_DISABLED,
    PINFUNC_MOMENTARY,
	PINFUNC_ALTERNATE,
    PINFUNC_INPUT
};

enum{
    PINSTATE_DISABLED,
    PINSTATE_LOW,
    PINSTATE_HIGH,
	PINSTATE_HIZ,
	PINSTATE_PULLUP
};



class ConfigDialog : public QDialog
{
    Q_OBJECT

    QHash<QString,QVariant> hash;
    QList<QComboBox*> cmbPinFunc,cmbPinInit,cmbPinPushed;
    QList<QLineEdit*> edtPinName;

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    void showEvent(QShowEvent * event);

    void loadSettings(QSettings &stg);
    void saveSettings(QSettings &stg);
    QHash<QString,QVariant> dataHash(){return hash;}

private slots:
    void onPinFuncActivatedSlot(int index);
    void on_btnOKCancel_accepted();
    void on_btnOKCancel_rejected();

private:
    void onPinFuncActivated(QComboBox *sender, int index);
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
