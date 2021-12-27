#include "configdialog.h"
#include "ui_configdialog.h"
#include "comthread.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    int i;
    ui->setupUi(this);

    ui->cmbSynchMode->addItem(tr("tr_async"),COMIF_ASYNC);
    ui->cmbSynchMode->addItem(tr("SPI"),COMIF_SPI);

	int rates[] = {150, 300, 600, 900, 1200, 1800, 2400, 3600, 4800, 9600,
				   14400, 19200, 28800, 33600, 38400, 56000, 57600, 76800, 115200,
				   128000, 153600, 230400, 460800, 921600, 1500000, 2000000, 0};
    for(i=0; rates[i]!=0; i++){
        ui->cmbBaudRate->addItem(tr("%1").arg(rates[i]),rates[i]);
    }
    for(i=1; i<=20; i++){
        QString itemtext = tr("COM%1").arg(i);
        ui->cmbComPort->addItem(itemtext, itemtext);
    }

    edtPinName << ui->edtPinName0;
    edtPinName << ui->edtPinName1;
    edtPinName << ui->edtPinName2;
    edtPinName << ui->edtPinName3;
    edtPinName << ui->edtPinName4;
	edtPinName << ui->edtPinName5;
    cmbPinFunc << ui->cmbPB0Func;
    cmbPinFunc << ui->cmbPB1Func;
    cmbPinFunc << ui->cmbPB2Func;
	cmbPinFunc << ui->cmbPB3Func;
    cmbPinInit << ui->cmbPB0Init;
    cmbPinInit << ui->cmbPB1Init;
    cmbPinInit << ui->cmbPB2Init;
	cmbPinInit << ui->cmbPB3Init;
    cmbPinPushed << ui->cmbPB0Pushed;
    cmbPinPushed << ui->cmbPB1Pushed;
    cmbPinPushed << ui->cmbPB2Pushed;
	cmbPinPushed << ui->cmbPB3Pushed;
    for(i=0; i<cmbPinFunc.size(); i++){
        cmbPinFunc[i]->addItem(tr("tr_disabled"), PINFUNC_DISABLED);
        cmbPinFunc[i]->addItem(tr("tr_momentary"), PINFUNC_MOMENTARY);
        cmbPinFunc[i]->addItem(tr("tr_alternate"), PINFUNC_ALTERNATE);
		cmbPinFunc[i]->addItem(tr("tr_input"), PINFUNC_INPUT);
        connect(cmbPinFunc[i], SIGNAL(activated(int)), SLOT(onPinFuncActivatedSlot(int)));

        cmbPinPushed[i]->addItem(tr("Low"), PINSTATE_LOW);
		cmbPinPushed[i]->addItem(tr("High"), PINSTATE_HIGH);
    }

    ui->cmbNewLine->addItem(tr("CR(\\r)"), "\r");
    ui->cmbNewLine->addItem(tr("LF(\\n)"), "\n");
    ui->cmbNewLine->addItem(tr("CRLF(\\r\\n)"), "\r\n");
    ui->cmbNewLine->addItem(tr("tr_none"), "");
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}


void ConfigDialog::showEvent(QShowEvent *event)
{
    int i,j;
    QList<QComboBox*> combos;

	//ui->edtTcpPort->setText(hash.value("tcpport").toString());
	//ui->edtPipeName->setText(hash.value("pipename").toString());

	ui->chkConnectOnBoot->setChecked(hash.value("connectonboot").toBool());
	ui->chkExecTargetAfterWriting->setChecked(hash.value("exectargetafterwriting").toBool());

    ui->cmbSynchMode->setCurrentIndex(ui->cmbSynchMode->findData(hash.value("synchmode")));
    ui->cmbComPort->setCurrentIndex(ui->cmbComPort->findData(hash.value("comport").toString()));
    ui->cmbBaudRate->setCurrentIndex(ui->cmbBaudRate->findData(hash.value("baudrate")));
	ui->edtSPIDelay->setText(hash.value("spidelay").toString());
	ui->edtASyncDelay->setText(hash.value("asyncspidelay").toString());

	for(i=0; i<cmbPinFunc.size(); i++){
        for(j=0; j<3; j++){
            if(j==0) combos = this->cmbPinFunc;
            else if(j==1) combos = this->cmbPinInit;
            else if(j==2) combos = this->cmbPinPushed;
            combos[i]->setCurrentIndex(combos[i]->findData(hash.value(tr("pinconf%1%2").arg(i).arg(j))));

			if(j==0)
				onPinFuncActivated(cmbPinFunc[i], cmbPinFunc[i]->currentIndex());
		}
    }
    for(i=0; i<edtPinName.size(); i++){
        edtPinName[i]->setText(hash.value(tr("pinname%1").arg(i)).toString());
    }
	ui->edtReadInterval->setText(hash.value("pinreadinterval").toString());

    ui->cmbNewLine->setCurrentIndex(ui->cmbNewLine->findData(hash.value("newlinecode")));

    QDialog::showEvent(event);
}



void ConfigDialog::loadSettings(QSettings &stg)
{
    int i;
    QString keys[] = {
		"connectonboot", "exectargetafterwriting",
		"pipename", "tcpport",
		"synchmode", "comport", "baudrate", "spidelay", "asyncspidelay",
        "targetif",
        "pinconf00", "pinconf01", "pinconf02",
        "pinconf10", "pinconf11", "pinconf12",
        "pinconf20", "pinconf21", "pinconf22",
		"pinconf30", "pinconf31", "pinconf32",
        "pinname0", "pinname1", "pinname2", "pinname3",
		"pinname4", "pinname5",
        "pinreadinterval", "newlinecode",
        ""
    };
    QVariant defaults[] = {
		true, false,
		"avrew", 12000,
		COMIF_ASYNC, "COM1", 115200, 100, 0.5,
        0,
        PINFUNC_ALTERNATE, PINSTATE_LOW, PINSTATE_HIGH,
        PINFUNC_ALTERNATE, PINSTATE_LOW, PINSTATE_HIGH,
        PINFUNC_ALTERNATE, PINSTATE_LOW, PINSTATE_HIGH,
		PINFUNC_ALTERNATE, PINSTATE_LOW, PINSTATE_HIGH,
        "PB0", "PB1", "PB2", "PB3",
		"DSR","CTS",
		500,  "\n"
    };

    stg.beginGroup("config");
    hash.clear();
    for(i=0; keys[i]!=""; i++){
        hash.insert(keys[i], stg.value(keys[i],defaults[i]));
    }

    stg.endGroup();
}

void ConfigDialog::saveSettings(QSettings &stg)
{
    int i;
    QList<QString> keys = hash.keys();
    QList<QVariant> values = hash.values();

    stg.beginGroup("config");
    for(i=0; i<keys.size(); i++){
        stg.setValue(keys[i], values[i]);
    }
    stg.endGroup();
}

void ConfigDialog::onPinFuncActivatedSlot(int index)
{
    onPinFuncActivated((QComboBox*)QObject::sender(), index);
}
void ConfigDialog::onPinFuncActivated(QComboBox *sender, int index)
{
    int i=-1;
    QComboBox *cmbfunc, *cmbinit, *cmbpushed;

    for(i=0; i<cmbPinFunc.size(); i++){
        if(sender == cmbPinFunc[i]){
            cmbfunc = cmbPinFunc[i];
            cmbinit = cmbPinInit[i];
            cmbpushed = cmbPinPushed[i];
            break;
        }
    }

    //アイテムを動的に変更
    switch(cmbfunc->itemData(index).toInt()){
    case PINFUNC_DISABLED:
		cmbinit->setDisabled(true);
		cmbpushed->setDisabled(true);
		break;
    case PINFUNC_INPUT:
		cmbinit->setDisabled(false);
		cmbinit->clear();
		cmbinit->addItem(tr("tr_hiz"), PINSTATE_HIZ);
		cmbinit->addItem(tr("tr_pullup"), PINSTATE_PULLUP);
		cmbpushed->setDisabled(true);
        break;
    case PINFUNC_ALTERNATE:
    case PINFUNC_MOMENTARY:
		cmbinit->setDisabled(false);
		cmbinit->clear();
		cmbinit->addItem(tr("Low"), PINSTATE_LOW);
		cmbinit->addItem(tr("High"), PINSTATE_HIGH);
        cmbpushed->setDisabled(false);
		break;
	}

}


void ConfigDialog::on_btnOKCancel_accepted()
{
    int i,j;
	QComboBox* combo;

    hash.clear();	
	hash.insert("connectonboot", ui->chkConnectOnBoot->isChecked());
	hash.insert("exectargetafterwriting", ui->chkExecTargetAfterWriting->isChecked());
//	hash.insert("pipename", ui->edtPipeName->text());
//	hash.insert("tcpport", ui->edtTcpPort->text().toInt());

    hash.insert("synchmode", ui->cmbSynchMode->itemData(ui->cmbSynchMode->currentIndex()));
    hash.insert("baudrate", ui->cmbBaudRate->itemData(ui->cmbBaudRate->currentIndex()));
    hash.insert("comport", ui->cmbComPort->itemData(ui->cmbComPort->currentIndex()));
	hash.insert("spidelay", ui->edtSPIDelay->text().toInt());
	hash.insert("asyncspidelay", ui->edtASyncDelay->text().toFloat());

    for(i=0; i<cmbPinFunc.size(); i++){
        for(j=0; j<3; j++){
			if(j==0) combo = this->cmbPinFunc[i];
			else if(j==1) combo = this->cmbPinInit[i];
			else if(j==2) combo = this->cmbPinPushed[i];
			hash.insert(tr("pinconf%1%2").arg(i).arg(j), combo->itemData(combo->currentIndex()));
        }
    }
    for(i=0; i<edtPinName.size(); i++){
        hash.insert(tr("pinname%1").arg(i), edtPinName[i]->text());
    }
	hash.insert("pinreadinterval", ui->edtReadInterval->text().toInt());

    hash.insert("newlinecode", ui->cmbNewLine->itemData(ui->cmbNewLine->currentIndex()));
}

void ConfigDialog::on_btnOKCancel_rejected()
{

}
