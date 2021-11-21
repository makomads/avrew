#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QLocalServer>
#include <QLocalSocket>
#include "comthread.h"
#include "deviceinfo.h"
#include "configdialog.h"
#include "functionbutton.h"
#include "console.h"

namespace Ui {
class MainWindow;
}


typedef QHash<QString,QString> StringHash;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QString newlinecode;
	QString settingspath;
	QHash<QString,QVariant> confighash;
    StringHash licensehash; //ライセンスキー

    DeviceSpec targetspec;
    QTimer *pinreadtimer;
    DeviceInfo* devinfo;
    ComThread* thcom;
    QCheckBox *chkFuses[3][8];
    FunctionButton *btnLinkstatus;
    QLabel *lblPinName[8];
    FunctionButton *btnFunction[8];
    ConfigDialog* configdlg;
    Console *console;
    bool toverify;

    QLocalServer *localsv;
    QLocalSocket *localcl;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    bool event( QEvent *event );
    StringHash licenceHash(){return licensehash;}

private slots:
    void on_btnSelectFlashWrite_clicked();
    void on_btnSelectEEPWrite_clicked();
    void on_btnSelectFlashRead_clicked();
    void on_btnSelectEEPRead_clicked();

    void on_actionConfig_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    void on_cmbDevice_currentIndexChanged(int index);

    void on_btnTestConnection_clicked();

    void on_btnReadFuse_clicked();

    void on_btnExecTarget_clicked();

    void on_btnWriteFuse_clicked();

    void on_btnWrite_clicked();


    void on_btnFlashRead_clicked();

    void on_btnEEPRead_clicked();

    void onPinreadTimeout();
    void onPinStateChange(int pinnumber, int pinstate);
    void onSendMessage();
    void onShowMessageChanged(int);

    void onPipeNewConnection ();
    void onPipeReadyRead ();

    void on_btnClearConsole_clicked();


	void on_btnResetTarget_clicked();

private:
    void applyConfig();
    void enableButtons(bool enable);
    void selectFile(QLineEdit *edit);
    QByteArray loadImage(QString filepath);

    Ui::MainWindow *ui;

protected:
};

#endif // MAINWINDOW_H
