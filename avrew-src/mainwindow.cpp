#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QToolButton>
#include <QPainter>
#include <QTranslator>
#include <QtGlobal>
#include <QDateTime>
#include "mainwindow.h"
#include "configdialog.h"
#include "ui_mainwindow.h"
#include "intelhex.h"
#include "functionbutton.h"
#include "versiondialog.h"
#include "version.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w;

    QTranslator translator;
    translator.load(":/translation_ja");
    a.installTranslator(&translator);

    a.setWindowIcon(QIcon(":icon1.ico"));
    w = new MainWindow();
    if(w->licenceHash().contains("appname")){
        w->show();
        return a.exec();
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int i,j;
    int row;
    QString a;
    QString licensevalue;
    QFileInfo fileinfo;

    //ライセンスキー作成
    //現在はハードコーディングだが、将来有料版を作るときにはここで処理する
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream ds(&buffer);
    ds.setVersion(QDataStream::Qt_5_5);
    ds << QString("AVREW Free Edition");
    ds << QDateTime::currentDateTime().toString("yyyyMMdd");
    ds << QString("1");
    ds << QString("2147483647");
    buffer.close();

    //ライセンスキー読み込み
    QByteArray plaindata = buffer.data();
    QBuffer buffer2(&plaindata);
    buffer2.open(QIODevice::ReadOnly);
    QDataStream ds2(&buffer2);
    ds2.setVersion(QDataStream::Qt_5_5);
    ds2 >> licensevalue;
    licensehash.insert("appname", licensevalue);
    ds2 >> licensevalue;
    licensehash.insert("date", licensevalue);
    ds2 >> licensevalue;
    licensehash.insert("version", licensevalue);
    ds2 >> licensevalue;
    licensehash.insert("limit", licensevalue);
    buffer2.close();

    if(!licensehash.contains("appname")){
        QMessageBox::critical(NULL, "AVREW", tr("tr_err_invalid_license"));
        qApp->exit(-1);
        return;
    }

    //ライセンスキーからタイトル設定
    ui->setupUi(this);
    setWindowTitle(licensehash.value("appname"));

    //ウィジェットをカスタムに置き換え
    QTextEdit *e = ui->edtConsole;
    console = new Console(this);
    ui->loConsole->addWidget(console);
    ui->edtConsole = console;
    delete e;

    //GUI部品
    for(i=0; i<3;i++){
        if(i==FUSE_LOW)
            ui->scrFusesLayout->addWidget(new QLabel(tr("tr_fuse_low")));
        else if(i==FUSE_HIGH)
            ui->scrFusesLayout->addWidget(new QLabel(tr("tr_fuse_high")));
        else if(i==FUSE_EXT)
            ui->scrFusesLayout->addWidget(new QLabel(tr("tr_fuse_extended")));
        for(j=7; j>=0; j--){
            chkFuses[i][j] = new QCheckBox(QString("bit%1").arg(j));
            ui->scrFusesLayout->addWidget(chkFuses[i][j]);
        }
    }
    configdlg = new ConfigDialog(this);
    configdlg->setWindowTitle(licensehash.value("appname"));


    //リンクステータス
    row=0;
    QLabel *lblLink = new QLabel("Link",this);
    ui->grdPins->addWidget(lblLink,0,row);
    ui->grdPins->setAlignment(lblLink, Qt::AlignHCenter);
    btnLinkstatus = new FunctionButton(PINFUNC_LINK, this);
    ui->grdPins->addWidget(btnLinkstatus,1,row);
    ui->grdPins->setAlignment(btnLinkstatus, Qt::AlignHCenter);
    btnLinkstatus->setFunction(PINFUNC_LINK);
    row++;

    //ピン
	QString labels[] = {"B0","B1","B2","B3", "DSR","CTS", ""};
    for(i=0; labels[i]!=""; i++, row++){
        lblPinName[i] = new QLabel(labels[i],this);
        ui->grdPins->addWidget(lblPinName[i],0,row);
        ui->grdPins->setAlignment(lblPinName[i], Qt::AlignHCenter);

        btnFunction[i] = new FunctionButton(i, this);
        ui->grdPins->addWidget(btnFunction[i],1,row);
        ui->grdPins->setAlignment(btnFunction[i], Qt::AlignHCenter);
		if(i>=4){
            btnFunction[i]->setFunction(PINFUNC_INPUT);
        }
    }
    ui->grdPins->setAlignment(Qt::AlignHCenter);

    //その他インスタンス
    devinfo = new DeviceInfo();
    thcom = new ComThread();
	localsv = new QLocalServer(this);
    localcl = NULL;
	pinreadtimer = new QTimer(this);

    //デバイス情報読み込み
    fileinfo = QFileInfo(QApplication::applicationDirPath(), "device.xml");
    if(!devinfo->openFile(fileinfo.absoluteFilePath(), &a, &i)){
        if(i<0){
            //XMLのエラーではなくファイルが見つからないとき
            devinfo->openFile(":/device.xml", &a, &i);
        }
        else{
            console->append(CONSTXT_ERROR, QString("xml failed line %1:%2").arg(i).arg(a));
        }
    }
    QStringList devlist = devinfo->enumDevices();
    for(i=0; i<devlist.size(); i+=2){
        ui->cmbDevice->addItem(devlist.at(i), devlist.at(i+1));
    }

    //設定読み込み
    fileinfo = QFileInfo(QApplication::applicationDirPath(), "avrew.ini");
    settingspath = fileinfo.absoluteFilePath();
	QSettings stg(settingspath, QSettings::IniFormat);
    stg.beginGroup("gui");
    resize(stg.value("mainwindow.size", QSize(400,400)).toSize());
    ui->splMainH->restoreState(stg.value("splh.state").toByteArray());
    ui->splMainV->restoreState(stg.value("splv.state").toByteArray());
    ui->edtFlashWrite->setText(stg.value("flashwrite").toString());
    ui->edtEEPWrite->setText(stg.value("eepwrite").toString());
    ui->edtFlashRead->setText(stg.value("flashread").toString());
    ui->edtEEPRead->setText(stg.value("eepread").toString());
    ui->chkEEPWrite->setChecked(stg.value("chkeepwrite").toBool());
    ui->chkVerify->setChecked(stg.value("chkverify").toBool());
    a = stg.value("fuse","00000000000000000000000000000000").toString();
    for(i=0; i<3; i++){
        for(j=7; j>=0; j--){
            if(a.at(i*8+j)=='1'){
                chkFuses[i][j]->setChecked(true);
            }
        }
    }
    ui->chkShowReceived->setChecked(stg.value("showreceived",true).toBool());
    console->setVisible(CONSTXT_RECEIVED, ui->chkShowReceived->isChecked());
    ui->chkShowSended->setChecked(stg.value("showsended",true).toBool());
    console->setVisible(CONSTXT_SENDED, ui->chkShowSended->isChecked());
    ui->chkShowAppMessage->setChecked(stg.value("showappmessage",true).toBool());
    console->setVisible(CONSTXT_APPMESSAGE, ui->chkShowAppMessage->isChecked());
    ui->chkShowError->setChecked(stg.value("showerror",true).toBool());
    console->setVisible(CONSTXT_ERROR, ui->chkShowError->isChecked());
    ui->cmbDevice->setCurrentIndex(ui->cmbDevice->findData(stg.value("devicesig","")));
    stg.endGroup();
    configdlg->loadSettings(stg);
    applyConfig();

	//プロセス間通信
	localsv->listen("avrew");

    //シグナル接続
	//タイマー
    connect(pinreadtimer, SIGNAL(timeout()), this, SLOT(onPinreadTimeout()));
	pinreadtimer->start(confighash.value("pinreadinterval").toInt());
    //ピン
	for(i=0;i<4;i++){
        connect(btnFunction[i], SIGNAL(pinStateChanged(int,int)), SLOT(onPinStateChange(int,int)));
    }
    //メッセージ送受信
    connect(ui->edtSendMessage, SIGNAL(returnPressed()), SLOT(onSendMessage()));
    connect(ui->chkShowReceived, SIGNAL(stateChanged(int)), SLOT(onShowMessageChanged(int)));
    connect(ui->chkShowSended, SIGNAL(stateChanged(int)), SLOT(onShowMessageChanged(int)));
    connect(ui->chkShowAppMessage, SIGNAL(stateChanged(int)), SLOT(onShowMessageChanged(int)));
    connect(ui->chkShowError, SIGNAL(stateChanged(int)), SLOT(onShowMessageChanged(int)));
    connect(localsv, SIGNAL(newConnection ()), SLOT(onPipeNewConnection ()));

	//接続
	if(confighash.value("connectonboot").toBool()==true){
		on_btnTestConnection_clicked();
	}

    //テスト
    /*
    console->append(CONSTXT_APPMESSAGE,"app msg");
    console->append(CONSTXT_SENDED,"sended msg");
    console->append(CONSTXT_RECEIVED,"received msg");
    console->append(CONSTXT_ERROR,"error msg");*/
}


MainWindow::~MainWindow()
{
    delete ui;
}



//起動時とコンフィグダイアログOKを押したときに呼ばれる
void MainWindow::applyConfig()
{
    int i;
    int func[5], init[5], pushed[5];
	QString message;

	if(thcom->bridgeIo()->isOpend()){
		thcom->bridgeIo()->closeRS232C();
	}

    //設定値を取得
	confighash = configdlg->dataHash();

	for(i=0; i<6; i++){
		lblPinName[i]->setText(confighash.value(tr("pinname%1").arg(i)).toString());
    }
	for(i=0; i<4; i++){
		func[i] = confighash.value(tr("pinconf%1%2").arg(i).arg(0)).toInt();
		init[i] = confighash.value(tr("pinconf%1%2").arg(i).arg(1)).toInt();
		pushed[i] = confighash.value(tr("pinconf%1%2").arg(i).arg(2)).toInt();
    }
	for(i=0; i<4; i++){
        btnFunction[i]->setFunction(func[i]);
		btnFunction[i]->assignPinState(init[i], pushed[i]);
    }
	//入力専用ピンはここで設定しない
	pinreadtimer->setInterval(confighash.value("pinreadinterval").toInt());
	newlinecode = confighash.value("newlinecode").toString();


	//COMポートオープン
	if(!thcom->openConnection(
				confighash.value("comport").toString(),
				confighash.value("baudrate").toInt(),
				confighash.value("synchmode").toInt(),
				confighash.value("spidelay").toInt() ) ){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
		return;
	}
	message = tr("tr_msg_comportopened");
	if(thcom->comportMode()==COMIF_ASYNC){
        message += QString(" %1 %2bps").arg(thcom->bridgeIo()->comPort()).arg(thcom->bridgeIo()->bitRate());
	}
	else if(thcom->comportMode()==COMIF_SPI){
		message += QString(" %1 SPI").arg(thcom->bridgeIo()->comPort());
	}
	console->append(CONSTXT_APPMESSAGE, message);


	//ASync時の設定
	if(thcom->comportMode()==COMIF_ASYNC){
		//ピンの機能設定
		int pinfuncs[4], pinstats[4];
		for(i=0; i<4; i++){
		   pinfuncs[i] = btnFunction[i]->function();
		   pinstats[i] = btnFunction[i]->pinState();
		}
		if(!thcom->setPinDirections(pinfuncs))
		   console->append(CONSTXT_ERROR, thcom->errorMessage());
		if(!thcom->setPinStates(pinstats))
		   console->append(CONSTXT_ERROR, thcom->errorMessage());

		//SPIディレイ設定
		float delayusec = confighash.value("asyncspidelay").toFloat();
		thcom->setSPIDelayForAsync(delayusec);
	}
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    int i,j;
    QString str;

	thcom->closeConnection();

	QSettings stg(settingspath, QSettings::IniFormat);
    stg.beginGroup("gui");
    stg.setValue("mainwindow.size", this->size());
    stg.setValue("splh.state", ui->splMainH->saveState());
    stg.setValue("splv.state", ui->splMainV->saveState());
    stg.setValue("flashwrite", ui->edtFlashWrite->text());
    stg.setValue("eepwrite", ui->edtEEPWrite->text());
    stg.setValue("flashread", ui->edtFlashRead->text());
    stg.setValue("eepread", ui->edtEEPRead->text());
    stg.setValue("chkeepwrite", ui->chkEEPWrite->isChecked());
    stg.setValue("chkverify", ui->chkVerify->isChecked());
    stg.setValue("showreceived", ui->chkShowReceived->isChecked());
    stg.setValue("showsended", ui->chkShowSended->isChecked());
    stg.setValue("showappmessage", ui->chkShowAppMessage->isChecked());
    stg.setValue("showerror", ui->chkShowError->isChecked());
    for(i=0; i<3; i++){
        for(j=0; j<8; j++){
            if(chkFuses[i][j]->isChecked())
                str += "1";
            else
                str += "0";
        }
    }
    stg.setValue("fuse", str);
    stg.setValue("devicesig", ui->cmbDevice->itemData(ui->cmbDevice->currentIndex()));
    stg.endGroup();
    configdlg->saveSettings(stg);
}



bool MainWindow::event(QEvent *event)
{
    if(event->type()==QEvent_Thread){
        ComThread::ThreadEvent* e = (ComThread::ThreadEvent*)event;
		QByteArray readdata, wrotedata;
		QString msg;
		QString path;
		int i,r;
		int cntunmatch;
        bool f_enablebuttons=true;

        //スレッド実行中
		if(!e->isFinished()){
            switch(e->operation()){
            case COMOP_WRITE:
			case COMOP_READ:
            case COMOP_PROGRESS:
                statusBar()->showMessage(tr("%1/%2").arg(e->value()).arg(e->maximum()));
                break;
            case COMOP_TARGETEXEC:
				//ターゲットからの受信
				readdata = e->parameter(0).toByteArray();
				if(readdata.right(1)==newlinecode)
					readdata.remove(readdata.length()-1,1);
				console->append(CONSTXT_RECEIVED, readdata, false);
                if(localcl && localcl->isOpen()){
					localcl->write(readdata);
                }
                break;
            }
        }
        //スレッド完了
        else{
            switch(e->operation()){
            case COMOP_WRITE:
				if(e->returnCode()<0){
					console->append(CONSTXT_ERROR, e->errorMessage());
				}
				else{
					statusBar()->showMessage(tr("tr_msg_completed"));
                    //照合もあれば続いて照合モードでFLASHの読み込みをする
                    if(toverify){
                        f_enablebuttons = false;
						thcom->startRead(this,
										 e->parameter(0).toByteArray().size(),
										 e->parameter(1).toByteArray().size());
                    }
				}
                break;
			case COMOP_READ:
				if(e->returnCode()<0){
                    console->append(CONSTXT_ERROR, e->errorMessage());
                }
                else{
					QByteArray flashimg, eepimg;
					flashimg = e->parameter(0).toByteArray();
					eepimg = e->parameter(1).toByteArray();
                    //照合モードのとき
                    if(toverify){
						//書き込みデータと照合する
						for(int opcnt=0; opcnt<2; opcnt++){
							if(opcnt==1 && !ui->chkEEPWrite->isChecked())
								continue;

							if(opcnt==0){
								path = ui->edtFlashWrite->text();
								readdata = flashimg;
							}
							else if(opcnt==1){
								path = ui->edtEEPWrite->text();
								readdata = eepimg;
							}

							wrotedata = loadImage(path);
							cntunmatch = 0;
							for(i=0; i<wrotedata.size(); i++){
								if(readdata[i]!=wrotedata[i])
									cntunmatch++;
							}
							if(cntunmatch==0){
								//照合成功
								console->append(CONSTXT_APPMESSAGE, tr("tr_msg_verify_success") + path);
							}
							else{
								//照合失敗
								console->append(CONSTXT_ERROR, tr("tr_msg_verify_failed") + path);
							}
						}
                    }
                    //読み込みモードの時
					else{
						if(flashimg.size()>0){
							path = ui->edtFlashRead->text();
							r = saveIntelHex(path.toLocal8Bit().data(),
									(unsigned char*)flashimg.data(), flashimg.size());
						}
						else if(eepimg.size()>0){
							path = ui->edtEEPRead->text();
							r = saveIntelHex(path.toLocal8Bit().data(),
									(unsigned char*)eepimg.data(), eepimg.size());
						}

                        console->append(CONSTXT_APPMESSAGE, tr("tr_msg_completed"));
                    }
                }
				break;
			case COMOP_TARGETEXEC:
				if(!thcom->resetTarget(false)){
					console->append(CONSTXT_ERROR, thcom->errorMessage());
					break;
				}
				thcom->enableProgramming();
                break;
            }

            if(f_enablebuttons)
                enableButtons(true);
        }
        return true;
    }
    return QMainWindow::event(event);
}



void MainWindow::selectFile(QLineEdit *edit)
{
	QFileDialog dlg(this, tr("tr_selectfile"), edit->text());
    QStringList filters, selected;
    filters << tr("*.bin *.hex");
	filters << tr("tr_allfiles");
    dlg.setNameFilters(filters);
	if(!dlg.exec())
		return;

    selected = dlg.selectedFiles();
    edit->setText(selected[0]);
}

//Intel Hex形式で読み、読めなければ通常のバイナリとして読む
//エラー時はQByteArray.isNull()==trueが返る
QByteArray MainWindow::loadImage(QString filepath)
{
	QByteArray ret;
    int imgsize;
    unsigned char image[64*1024];
    QFileInfo fileinfo;
    QString msg;

    //存在を調べる
    fileinfo.setFile(filepath);
	if(!fileinfo.exists()){
		console->append(CONSTXT_ERROR, tr("tr_err_fileopen") + filepath);
        return ret;
    }

    imgsize = loadIntelHex(filepath.toLocal8Bit().data(), image, sizeof(image));
    if(imgsize<0){
        //ファイル読み込み失敗
		console->append(CONSTXT_ERROR, tr("tr_invalid_file") + filepath);
        return ret;
    }
    else if(imgsize==0){
		//ファイルは読み込めたがIntelHexでない場合はバイナリとして開く
        QFile file(fileinfo.absoluteFilePath());
        file.open(QIODevice::ReadOnly);
        ret = file.readAll();
        file.close();
    }
    else{
        //正常にIntel Hexを読めた場合
        ret = QByteArray((const char*)image, imgsize);
    }
    return ret;
}


void MainWindow::on_btnSelectFlashWrite_clicked()
{
    selectFile(ui->edtFlashWrite);
}

void MainWindow::on_btnSelectEEPWrite_clicked()
{
    selectFile(ui->edtEEPWrite);
}

void MainWindow::on_btnSelectFlashRead_clicked()
{
    selectFile(ui->edtFlashRead);
}

void MainWindow::on_btnSelectEEPRead_clicked()
{
    selectFile(ui->edtEEPRead);
}


void MainWindow::on_actionConfig_triggered()
{
    if(configdlg->exec()==QDialog::Accepted){
		thcom->closeConnection();
        applyConfig();
    }
}

void MainWindow::enableButtons(bool en)
{
    ui->btnTestConnection->setEnabled(en);
    ui->btnWrite->setEnabled(en);
    ui->chkVerify->setEnabled(en);
    ui->btnFlashRead->setEnabled(en);
    ui->chkEEPWrite->setEnabled(en);
    ui->btnEEPRead->setEnabled(en);
    ui->btnReadFuse->setEnabled(en);
    ui->btnWriteFuse->setEnabled(en);
}

void MainWindow::on_actionExit_triggered()
{
    thcom->closeConnection();
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QString text;
    VersionDialog dlg(this);
    dlg.setWindowTitle(licensehash.value("appname"));

    text = licensehash.value("appname");
    text += tr(" %1").arg(VER_PRODUCTVERSION_STR);
	text += "<br>"
			"<br><a href='http://mkmds.wordpress.com/'>http://mkmds.wordpress.com/</a>"
            "<br><br><a href='https://twitter.com/makomads'>Twitter</a>"
            ;
    dlg.setText(text);
    dlg.exec();
}

void MainWindow::on_cmbDevice_currentIndexChanged(int index)
{
    QString devsig;
    int i,j;

    devsig = ui->cmbDevice->itemData(index).toString();
    if(devinfo->deviceSpec(devsig, &targetspec)){
        ui->txtDevice->setText(
          tr("FlashROM:%1bytes * %2pages EEPROM:%3bytes")
                    .arg(targetspec.flashpagesize)
                    .arg(targetspec.nflashpages)
                    .arg(targetspec.eeppagesize * targetspec.neeppages));
    }

    //ヒューズ
    for(i=0; i<3; i++){
        for(j=7; j>=0; j--){
            chkFuses[i][j]->setText(tr("%1: ").arg(j) + targetspec.fusedescs[i][j]);
        }
    }

    thcom->setTargetSpec(&targetspec);
}



extern unsigned long g_eltime;

//接続確認と識票の読み出し
void MainWindow::on_btnTestConnection_clicked()
{
	QString sig;
	int version;
	int synchmode = confighash.value("synchmode").toInt();

	if(!thcom->clearBuffer()){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
		return;
	}

	//調歩同期ではブリッジ接続も確認する
	if(synchmode==COMIF_ASYNC){
		//ブリッジバージョンを取得して接続確認
		if(!thcom->bridgeVersion(&version)){
			console->append(CONSTXT_ERROR, thcom->errorMessage());
			console->append(CONSTXT_APPMESSAGE, QString("eltime:%1").arg(g_eltime));
			return;
		}
		console->append(CONSTXT_APPMESSAGE, QString("Bridge version:%1").arg(version));

		//ターゲットリセット
		thcom->resetTarget(false);

		//ピン読み取りタイマー開始
		pinreadtimer->stop();
		pinreadtimer->start(confighash.value("pinreadinterval").toInt());
	}

	//プログラミング許可
	if(!thcom->enableProgramming()){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
	}
	//シグネチャ読み出し
	if(!thcom->readSignature(&sig)){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
    }
    else{
        QString msg;
		msg = tr("tr_msg_target_signature") + QString("%1").arg(sig);
        console->append(CONSTXT_APPMESSAGE, msg);
        ui->cmbDevice->setCurrentIndex(ui->cmbDevice->findData(sig));
        if(ui->cmbDevice->currentIndex()<0){
            msg = tr("tr_err_target_unknown") + QString("%1").arg(sig);
            console->append(CONSTXT_ERROR, msg);
        }
    }

	console->append(CONSTXT_APPMESSAGE, QString("eltime:%1").arg(g_eltime));
}


//ヒューズ読み出し
void MainWindow::on_btnReadFuse_clicked()
{
    unsigned char fuses[10];
    int i,j;
    if(thcom->readAllFuses(fuses)){
        console->append(CONSTXT_APPMESSAGE, tr("tr_msg_fuseread"));
        for(i=0; i<3;i++){
            for(j=7; j>=0; j--){
                chkFuses[i][j]->setChecked(fuses[i]&(1<<j));
            }
        }
    }
    else{
		console->append(CONSTXT_ERROR, thcom->errorMessage());
    }
}

//ヒューズ書き込み
void MainWindow::on_btnWriteFuse_clicked()
{
    int i,j;
    bool bitstate[3][8];
    unsigned char fuses[3];
    int result_inspect;
    QString message;

    for(i=0; i<3;i++){
        fuses[i] = 0;
        for(j=7; j>=0; j--){
            bitstate[i][j] = chkFuses[i][j]->isChecked();
            if(chkFuses[i][j]->isChecked())
                fuses[i] |= 1<<j;
        }
    }

    //ヒューズ監査
    result_inspect = devinfo->inspect(ui->cmbDevice->itemData(ui->cmbDevice->currentIndex()).toString(),
                                      bitstate, &message);
    //エラーのとき
    if(result_inspect<0){
        QMessageBox::critical(this, "", message);
        return;
    }
    //ワーニングのとき
    else if(result_inspect==0){
        if( QMessageBox::warning(this, "", message,
            QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
            return;
    }
    //エラー、ワーニングなしで続行


    //[debug]
    //ui->edtConsole->append(tr("%1 %2 %3").arg(fuses[0]).arg(fuses[1]).arg(fuses[2]));
    //return;

    if(thcom->writeAllFuses(fuses)){
		console->append(CONSTXT_APPMESSAGE, tr("tr_msg_fusewrite"));
    }
    else{
		console->append(CONSTXT_ERROR, thcom->errorMessage());
    }


}



//ROM書き込み
void MainWindow::on_btnWrite_clicked()
{
	QByteArray flashimg, eepimg;
	QString msg;

	//SPIモードでは(現時点では)EEPROMに対応しない
	if(ui->chkEEPWrite->isChecked() && thcom->comportMode()==COMIF_SPI){
		console->append(CONSTXT_ERROR, tr("tr_err_notallowedeeponspi"));
		return;
	}

	//フラッシュ読み込み
    flashimg = loadImage(ui->edtFlashWrite->text());
	if(flashimg.isEmpty())
        return;

	//ライセンス制限
    int limitsize = licensehash.value("limit").toInt();
    if(flashimg.size()>limitsize && thcom->comportMode()==COMIF_ASYNC){
		msg = tr("tr_licence_exceedlimit") + tr("flash %1 > limit %2").arg(flashimg.size()).arg(limitsize);
		console->append(CONSTXT_ERROR, msg);
        console->append(CONSTXT_APPMESSAGE, tr("tr_licence_purchase"));
        return;
    }

	//EEPROM読み込み
    if(ui->chkEEPWrite->isChecked()){
		eepimg = loadImage(ui->edtEEPWrite->text());
		if(eepimg.isEmpty())
			return;
    }

    //照合フラグ
	toverify = ui->chkVerify->isChecked();

    //スレッド開始
	console->append(CONSTXT_APPMESSAGE, tr("tr_write_bytes").arg(flashimg.size()));
    if(!thcom->startWrite(this, flashimg, eepimg)){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
    }
    else{
        //スレッド実行中はボタン使用禁止
        enableButtons(false);
    }
}


void MainWindow::on_btnFlashRead_clicked()
{
    QString msg;

    //ファイルに書き込み許可があるか調べる
    QFile file(ui->edtFlashRead->text());
    if(!file.open(QIODevice::WriteOnly)){
        msg = tr("tr_err_fileopen") + QString("%1").arg(ui->edtFlashRead->text());
        console->append(CONSTXT_ERROR, msg);
        return;
    }

    //スレッド開始
    toverify = false;   //照合モードではない
    console->append(CONSTXT_APPMESSAGE, tr("tr_msg_startreadflash"));
	if(!thcom->startRead(this, targetspec.flashpagesize*targetspec.nflashpages, -1)){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
    }
    else{
        //スレッド実行中はボタン使用禁止
        enableButtons(false);
    }
}



void MainWindow::on_btnEEPRead_clicked()
{
	//SPIモードでは(現時点では)EEPROMに対応しない
	if(thcom->comportMode()==COMIF_SPI){
		console->append(CONSTXT_ERROR, tr("tr_err_notallowedeeponspi"));
		return;
	}


    QString msg;
    QFile file(ui->edtEEPRead->text());
    if(!file.open(QIODevice::WriteOnly)){
        msg = tr("tr_err_fileopen") + QString("%1").arg(ui->edtEEPRead->text());
        console->append(CONSTXT_ERROR, tr("tr_err_fileopen"));
        return;
    }

    toverify = false;   //照合モードではない
    console->append(CONSTXT_APPMESSAGE, tr("tr_msg_startreadeep"));
	if(!thcom->startRead(this, -1, targetspec.eeppagesize*targetspec.neeppages)){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
    }
    else{
        enableButtons(false);
    }
}


//ターゲット起動
void MainWindow::on_btnExecTarget_clicked()
{
	if(thcom->comportMode()!=COMIF_ASYNC){
        console->append(CONSTXT_ERROR, tr("tr_err_modenotasync"));
        return;
    }
    //起動
    if(ui->btnExecTarget->isChecked()){
        //スレッド開始
        if(!thcom->startTargetExecution(this)){
			console->append(CONSTXT_ERROR, thcom->errorMessage());
            ui->btnExecTarget->setChecked(false);
        }
        else{
            enableButtons(false);
        }
    }
    //停止
    else{
		thcom->stopTarget();
		//以降の処理はスレッド終了通知のイベントで行う
	}
}


//ピン読み取りタイマー
void MainWindow::onPinreadTimeout()
{
    int i;
    int pinstats[6];

    //ターゲット起動以外でスレッド動作時はコンフリクトを避けるためタイマー動作をしない
	if(thcom->isRunning() && !ui->btnExecTarget->isChecked()){
		return;
	}

    //ピンの状態を得る、同時に接続状態も得る
	if(!thcom->inquirePinStates(pinstats)){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
        btnLinkstatus->setInputState(PINSTATE_LOW);
		pinreadtimer->stop();
        return;
    }
    btnLinkstatus->setInputState(PINSTATE_HIGH);

    //入力のみボタンの色を反映
    for(i=0; i<6; i++){
        if(btnFunction[i]->function() != PINFUNC_INPUT)
            continue;
        btnFunction[i]->setInputState(pinstats[i]);
    }
}

//各ボタンが押されたときのシグナルが接続されたスロット
void MainWindow::onPinStateChange(int pinnumber, int pinstate)
{
    int i;
    int pinstats[4];

    for(i=0; i<4; i++){
        pinstats[i] = btnFunction[i]->pinState();
    }
    if(!thcom->setPinStates(pinstats))
        console->append(CONSTXT_ERROR, thcom->errorMessage());
}

void MainWindow::onSendMessage()
{
    QString text = ui->edtSendMessage->text();
    text.append(newlinecode);

    if(!thcom->sendToTarget(text.toUtf8())){
        console->append(CONSTXT_ERROR, thcom->errorMessage());
    }

    if(ui->chkShowSended->isChecked()){
		console->append(CONSTXT_SENDED, ui->edtSendMessage->text(), false);
    }

	ui->edtSendMessage->clear();
}



void MainWindow::onShowMessageChanged(int state)
{
    QCheckBox *chk = (QCheckBox*)QObject::sender();
    bool visible;

    if(state == Qt::Unchecked)
        visible = false;
    else
        visible = true;

    if(chk==ui->chkShowReceived){
        console->setVisible(CONSTXT_RECEIVED, visible);
    }
    else if(chk==ui->chkShowSended){
        console->setVisible(CONSTXT_SENDED, visible);
    }
    else if(chk==ui->chkShowAppMessage){
        console->setVisible(CONSTXT_APPMESSAGE, visible);
    }
    else if(chk==ui->chkShowError){
        console->setVisible(CONSTXT_ERROR, visible);
    }

}

void MainWindow::onPipeNewConnection()
{
    if(localcl && localcl->isOpen()){
        localcl->close();
        delete localcl;
    }
    localcl = localsv->nextPendingConnection();
    connect(localcl, SIGNAL(readyRead()), SLOT(onPipeReadyRead()));
}

void MainWindow::onPipeReadyRead()
{
    QByteArray pipedata;
    pipedata = localcl->readAll();
    if(ui->chkShowSended->isChecked()){
        console->append(CONSTXT_SENDED, pipedata.constData(), false);
    }
    if(!thcom->sendToTarget(pipedata)){
        console->append(CONSTXT_ERROR, tr("送信失敗。"));
    }
}



void MainWindow::on_btnClearConsole_clicked()
{
    console->clear();
}


void MainWindow::on_btnResetTarget_clicked()
{
	if(!thcom->resetTarget(ui->btnExecTarget->isChecked())){
		console->append(CONSTXT_ERROR, thcom->errorMessage());
	}
}
