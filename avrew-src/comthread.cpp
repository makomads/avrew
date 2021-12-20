#include <QApplication>
#include <stdio.h>
#include "comthread.h"
#include "configdialog.h"


/*
tr_err_ionotopend
tr_err_br_responce
tr_err_modenotasync
*/

ComThread::ComThread(QObject *parent) :
	QThread(parent)
{
	receiver = NULL;
	devspec = NULL;
	comif = COMIF_NULL;
	continue_loop = false;
    io = new BridgeIO();
}

void ComThread::setError(const QString &msg)
{
	errormsg = msg;
}

QString ComThread::trIOError()
{
	QString msg;
	switch(io->errorCode()){
	case COMERR_PORT:
		msg = tr("tr_ioerr_port");
		break;
	case COMERR_BUFFER:
		msg = tr("tr_ioerr_buffer");
		break;
	case COMERR_PIN:
		msg = tr("tr_ioerr_pin");
		break;
	case COMERR_SEND:
		msg = tr("tr_ioerr_send");
		break;
	case COMERR_RECEIVE:
		msg = tr("tr_ioerr_receive");
		break;
	default:
        return QString("Undefined General Error.");
	}

    msg += QString(" %1 %2bps").arg(io->comPort()).arg(io->bitRate());
	return msg;
}


void ComThread::setTargetSpec(DeviceSpec *spec)
{
	devspec = spec;
}


//COMポートオープン
//comifmodeがSPIのときはbpsは無視
bool ComThread::openConnection(QString comport, int bps, int comifmode, int spidelay)
{
	if(!devspec){
		setError(tr("tr_err_devnotselected"));
		return false;
	}

    comif = comifmode;
	io->setSPIDelay(spidelay);

	//COMポートオープン
    if(io->isOpend()){
		io->clearBuffer();
    }
	else{
		if(!io->openRS232C(comport.toLatin1(), bps)){
			setError(trIOError());
			return false;
		}
	}
	return true;
}

void ComThread::closeConnection()
{
	clearBuffer();
	io->closeRS232C();
}

bool ComThread::clearBuffer()
{
	if(!io->clearBuffer()){
		setError(trIOError());
		return false;
	}
	tgtsendbuf.clear();
	tgtrecvbuf.clear();
	cmdrecvbuf.clear();
	return true;
}

void ComThread::run()
{
	switch(operation){
	case COMOP_WRITE:
		if(comif == COMIF_ASYNC)
            runWriteAsync(varlist[0].toByteArray(), varlist[1].toByteArray());
		else if(comif == COMIF_SPI)
            runWriteSPI(varlist[0].toByteArray(), varlist[1].toByteArray());
		break;
	case COMOP_READFLASH:
	case COMOP_READEEPROM:
		if(comif == COMIF_ASYNC)
			runReadAsync();
		else if(comif == COMIF_SPI)
			runReadSPI();
		break;
	case COMOP_TARGETEXEC:
		runTarget();
		break;
	}
}


bool ComThread::startWrite(QObject *receiver, QByteArray flashimg, QByteArray eepimg)
{
	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}

	this->receiver = receiver;
	operation = COMOP_WRITE;
	varlist.clear();
	varlist.append(flashimg);
    varlist.append(eepimg);
    wait();
	start();
	return true;
}


void ComThread::runWriteAsync(QByteArray flashimg, QByteArray eepimg)
{
	ThreadEvent *ev = new ThreadEvent(operation);
	unsigned char *pimg;
	int i,j,r=0;
	int orgimgsize;
	int pageaddr;
	int npages;
    QByteArray senddata, recvdata;
	unsigned char singlein[4], ins[10000];

	ev = new ThreadEvent(operation);
	ev->setValueMax(0,0);
	QApplication::postEvent(receiver, ev);

	//Flash,EEPROM消去
	if(!exchangeCommand(singlein, 0xAC, 0x80, 0, 0)){
		QApplication::postEvent(receiver, new ThreadEvent(operation, -1, trIOError()));
		return;
	}
	msleep(9);

	//イメージをページ境界になるよう拡張し、余分な部分はFFで埋める
	orgimgsize = flashimg.size();
	npages = (flashimg.length() + devspec->flashpagesize-1) / devspec->flashpagesize;
	flashimg.resize(npages * devspec->flashpagesize);
	pimg = (unsigned char*)flashimg.data();
	memset(pimg+orgimgsize, 0xFF, flashimg.size()-orgimgsize);

	//連続データの長さ設定(上位、下位の順)
	exchangeCommand(singlein, 0xFF, 0xCE,
					(devspec->flashpagesize>>8) & 0xFF, devspec->flashpagesize&0xFF);

	//書き込み
	postProgressEvent(0, flashimg.size());
	pageaddr = 0;
	while(pageaddr*2 < flashimg.size()){
		//送信データ作成
		addCommand(senddata, 0xFF, 0xC0, 0, 0);	//連続ページ設定
		senddata.append((const char*)pimg, devspec->flashpagesize);
		pimg += devspec->flashpagesize;
		addCommand(senddata, 0x4C, (pageaddr>>8)&0xFF, pageaddr&0xFF, 0);
		pageaddr += devspec->flashpagesize/2;   //ワード単位なので/2
		//送信
        send((unsigned char*)senddata.data(), senddata.size());
		//受信
        recvdata = receiveCommandResp(2*4, false); //0xFF,0xC0と、0x4Cの2コマンドが返るはず
		//ウエイト(4.5ms)
		msleep(5);
		senddata.clear();
		postProgressEvent(pageaddr*2, flashimg.size());
	}
	postProgressEvent(pageaddr*2, flashimg.size());

	//EEPROM
	if(eepimg.size()!=0){
		orgimgsize = eepimg.size();
		npages = (eepimg.length() + devspec->eeppagesize-1) / devspec->eeppagesize;
		eepimg.resize(npages * devspec->eeppagesize);
		pimg = (unsigned char*)eepimg.data();
		memset(eepimg.data()+orgimgsize, 0xFF, eepimg.size() - orgimgsize);

		//連続データの長さ設定(上位、下位の順)
		exchangeCommand(singlein, 0xFF, 0xCE,
						(devspec->eeppagesize>>8) & 0xFF, devspec->eeppagesize&0xFF);
		//書き込み
		postProgressEvent(0, eepimg.size());
		pageaddr = 0;
		while(pageaddr < eepimg.size()){
			//送信データ作成
			addCommand(senddata, 0xFF, 0xC2, 0, 0);	//連続ページ設定
			senddata.append((const char*)pimg, devspec->eeppagesize);
			pimg += devspec->eeppagesize;
			addCommand(senddata, 0xC2, (pageaddr>>8)&0xFF, pageaddr&0xFF, 0);
			pageaddr += devspec->eeppagesize;
			//送信
			send((unsigned char*)senddata.data(), senddata.size());
			//受信
			recvdata = receiveCommandResp(2*4, false); //0xFF,0xC0と、0xC2の2コマンドが返るはず
			//ウエイト(4.5ms)
			msleep(5);
			senddata.clear();
			postProgressEvent(pageaddr, eepimg.size());
		}
		postProgressEvent(eepimg.size(), eepimg.size());
	}

	//完了
	QApplication::postEvent(receiver, new ThreadEvent(operation, 0, QString("")));
}

void ComThread::runWriteSPI(QByteArray flashimg, QByteArray eepimg)
{
	ThreadEvent *ev;
	int i,j,r=0;
	int orgimgsize;
	int npages;
	int addr;
	unsigned char out, in[4];
	int cntunmatch=0;

	//Flashイメージをページ境界になるよう拡張する
	orgimgsize = flashimg.size();
	npages = (flashimg.length() + devspec->flashpagesize-1) / devspec->flashpagesize;
	flashimg.resize(npages * devspec->flashpagesize);
	memset(flashimg.data()+orgimgsize, 0xFF, flashimg.size() - orgimgsize);

	//消去
    if(!exchangeCommand(NULL, 0xAC, 0x80, 0, 0)){
		QApplication::postEvent(receiver, new ThreadEvent(operation, -1, trIOError()));
		return;
	}
	msleep(9); //9ms

	//FLASH書き込み
	postProgressEvent(0, flashimg.size());
	for(i=0; i<npages; i++){
		for(j=0; j<devspec->flashpagesize/2; j++){
			out = flashimg.at(i*devspec->flashpagesize + j*2);
            exchangeCommand(in, 0x40, 0, j, flashimg.at(i*devspec->flashpagesize + j*2));
			out = flashimg.at(i*devspec->flashpagesize + j*2+1);
            exchangeCommand(in, 0x48, 0, j, flashimg.at(i*devspec->flashpagesize + j*2 + 1));
		}
		addr = i * devspec->flashpagesize/2;
        exchangeCommand(in, 0x4C, (addr>>8)&0xFF, addr&0xFF, 0);

		//ウエイト(4.5ms)
		msleep(5);
		postProgressEvent(i*devspec->flashpagesize, flashimg.size());
	}
	postProgressEvent(flashimg.size(), flashimg.size());

	//EEPROM
	if(cntunmatch==0 && eepimg.size()!=0){
		orgimgsize = eepimg.size();
		npages = (eepimg.length() + devspec->eeppagesize-1) / devspec->eeppagesize;
		eepimg.resize(npages * devspec->eeppagesize);
		memset(eepimg.data()+orgimgsize, 0xFF, eepimg.size() - orgimgsize);

		//書き込み
		postProgressEvent(0, eepimg.size());
		for(i=0; i<npages; i++){
			for(j=0; j<devspec->eeppagesize; j++){
                exchangeCommand(NULL, 0xC1, (j>>8)&0xFF, j&0xFF, eepimg[j]);
			}
			addr = i * devspec->eeppagesize;
            exchangeCommand(NULL, 0xC2, (addr>>8)&0xFF, addr&0xFF, 0);

			//ウエイト(4.0ms)
			msleep(5);
			postProgressEvent(i*devspec->eeppagesize, eepimg.size());
		}
		postProgressEvent(eepimg.size(), eepimg.size());
	}

	//完了
	QApplication::postEvent(receiver, new ThreadEvent(operation, r, QString("")));
}



bool ComThread::startReadFlash(QObject *receiver)
{
	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}
	this->receiver = receiver;
	operation = COMOP_READFLASH;
	varlist.clear();
    wait();
	start();
	return true;
}

bool ComThread::startReadEEPROM(QObject *receiver)
{
	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}
	this->receiver = receiver;
	operation = COMOP_READEEPROM;
	varlist.clear();
    wait();
	start();
	return true;
}

void ComThread::runReadAsync()
{
	ThreadEvent *ev = new ThreadEvent(operation);
	QByteArray readimg, senddata, recvdata;
	int retcode = 0;
	unsigned char in[4];
	int nblocks;
	int blksize;
	int blkaddr;
	int imagesize;
	int blockmodetype;
	QString errmsg;

	if(operation == COMOP_READFLASH){
		imagesize = devspec->flashpagesize * devspec->nflashpages;
		blockmodetype = 0xC1;
		blksize = 256;
	}
	else if(operation == COMOP_READEEPROM){
		imagesize = devspec->eeppagesize * devspec->neeppages;
		blockmodetype = 0xC3;
		blksize = 128;
	}
	nblocks = (imagesize+(blksize-1))/blksize;

	//連続データのブロック長設定(上位、下位の順)、接続確認も兼ねている
	if(!exchangeCommand(in, 0xFF, 0xCE, (blksize>>8)&0xFF, blksize&0xFF))
		errmsg = trIOError();
	else if(in[1]!=0xCE){
		if(errmsg=="")
			errmsg = tr("tr_err_br_responce");
	}
	if(errmsg!=""){
		QApplication::postEvent(receiver, new ThreadEvent(operation, -1, errmsg));
		return;
	}

	//ブロック単位ループ
	postProgressEvent(0, imagesize);
	for(int i=0; i<nblocks; i++){
		//ブロックアドレス設定
		blkaddr = i*blksize;
		if(operation == COMOP_READFLASH)
			blkaddr /= 2;	//FLASHのアドレスはワード単位なので/2する、EEPはバイト単位

		//コマンド実行
        /*
        senddata.clear();
		addCommand(senddata, 0xFF, blockmodetype, (blkaddr>>8)&0xFF, blkaddr&0xFF);
		send((unsigned char*)senddata.data(), 4);
        recvdata = receiveCommandResp(4, false);*/
        exchangeCommand(in, 0xFF, blockmodetype, (blkaddr>>8)&0xFF, blkaddr&0xFF);
		recvdata = receiveCommandResp(blksize, true);
		readimg.append(recvdata, blksize);
		postProgressEvent(readimg.size(), imagesize);
	}
	postProgressEvent(imagesize, imagesize);

	//完了イベント送出
	ev->setReturnCode(retcode);
	if(retcode < 0)
		ev->setErrorMessage(errmsg);
	ev->setFinished(true);
	ev->setParameter(readimg, 0);
	QApplication::postEvent(receiver, ev);
}

void ComThread::runReadSPI()
{
	ThreadEvent *ev;
	QByteArray readimg;
	int i;
	unsigned char in[4];
	int addr;
    int pagesize;

    if(operation == COMOP_READFLASH){
        pagesize = devspec->flashpagesize;
        readimg.resize(pagesize * devspec->nflashpages);
    }
    else if(operation == COMOP_READEEPROM){
        pagesize = devspec->eeppagesize;
        readimg.resize(pagesize * devspec->neeppages);
    }
	postProgressEvent(0, readimg.size());
	for(i=0; i<readimg.size(); i++){
        if(operation == COMOP_READFLASH){
            addr = i/2;
            if(i%2==0)
                exchangeCommand(in, 0x20, (addr>>8)&0xFF, addr&0xFF, 0);
            else
                exchangeCommand(in, 0x28, (addr>>8)&0xFF, addr&0xFF, 0);
        }
        else if(operation == COMOP_READEEPROM){
            addr = i;
            exchangeCommand(in, 0xA0, (addr>>8)&0xFF, addr&0xFF, 0);
        }
		readimg[i] = in[3];
        if(i%pagesize == 0)
			postProgressEvent(i, readimg.size());
	}
	postProgressEvent(readimg.size(), readimg.size());

	//完了イベント送出
	ev = new ThreadEvent(operation);
	ev->setReturnCode(i);
	ev->setFinished(true);
	ev->setParameter(readimg, 0);
	QApplication::postEvent(receiver, ev);
}



bool ComThread::startTargetExecution(QObject *receiver)
{
	unsigned char in[4];

    if(!io->isOpend())
		return false;

	//ターゲットリセットピンをハイにしてプログラミングモードを解除
	//実行モードへ
	exchangeCommand(in, 255, 6, 1, 0);

	this->receiver = receiver;
	operation = COMOP_TARGETEXEC;
	continue_loop=true;
	tgtsendbuf.clear();
	tgtrecvbuf.clear();
	io->clearBuffer();

	wait();
	start();
	return true;
}


void ComThread::runTarget()
{
    ThreadEvent *ev = new ThreadEvent(operation);
	int readlen=0;
	int i;
	int errcnt;
	int inquecount;
	QByteArray senddata, recvdata;

	while(continue_loop){
		//送信
		if(!tgtsendbuf.isEmpty()){
            iomutex.tryLock(3000);
			senddata.clear();
			errcnt=0;

			for(i=0; !tgtsendbuf.isEmpty(); i++){
				addCommand(senddata, 0xFF, 0xF1, tgtsendbuf.dequeue(), 0);
				//ソフトUARTの方が遅いので3コマンド分ウエイトを入れる
				addCommand(senddata, 0xFF, 0xFF, 0, 0);
				addCommand(senddata, 0xFF, 0xFF, 0, 0);
				addCommand(senddata, 0xFF, 0xFF, 0, 0);
			}

			send((unsigned char*)senddata.data(), senddata.size());
			recvdata = receiveCommandResp(i*4, false);
            iomutex.unlock();
		}

		//受信
		inquecount = io->getInQue();
		if(inquecount>=0){
			iomutex.tryLock(3000);
			recvdata = receiveTargetMessage();
			iomutex.unlock();
			if(recvdata.length()!=0){
				ev = new ThreadEvent(operation);
				ev->setReturnCode(readlen);
				ev->setFinished(false);
				ev->setParameter(recvdata, 0);
				QApplication::postEvent(receiver, ev);
			}
		}

		//受信も送信もしないならしばらく休む
		if(inquecount==0 && tgtsendbuf.isEmpty()){
			msleep(100);
		}

	}

	//ターゲットをプログラミングモードへ
	exchangeCommand(NULL, 255, 6, 0, 0);

	//完了通知
	ev = new ThreadEvent(operation);
	ev->setReturnCode(0);
	ev->setFinished(true);
	QApplication::postEvent(receiver, ev);

}

bool ComThread::sendToTarget(QByteArray data)
{
	int i;
	if(!continue_loop)
		return false;
	if(tgtsendbuf.size() + data.size() > MAX_BUFFER_SIZE){
		//バッファ溢れ
		return false;
	}

    iomutex.tryLock(3000);
	for(i=0; i<data.size(); i++){
		tgtsendbuf.enqueue(data[i]);
	}
    iomutex.unlock();
	return true;
}

bool ComThread::enableProgramming()
{
	unsigned char in[4];
	char str[100];
	exchangeCommand(in, 0xAC, 0x53, 0, 0);
	if(in[2]!=0x53){ //0b01010011
		sprintf(str, "%02X %02X %02X %02X", in[0],in[1],in[2],in[3]);
		setError(tr("tr_err_setprg_enabled") + QString(" %1").arg(str));
		return false;
	}
	return true;
}

void ComThread::stopTarget()
{
	continue_loop=false;
	wait();
}



//スレッドの進行状況をイベントで送る
void ComThread::postProgressEvent(int value, int max)
{
	ThreadEvent *ev;
	ev = new ThreadEvent(COMOP_PROGRESS);
	ev->setValueMax(value, max);
	ev->setFinished(false);
	QApplication::postEvent(receiver, ev);
}


//識票取得
bool ComThread::readSignature(QString *sig)
{
	int i, num;
	char str[3];
	unsigned char in[4];

    for(i=0; i<3; i++){
        exchangeCommand(in, 0x30, 0, i, 0);
		num = in[3];
		sprintf(str, "%02X", num);
		sig->append(str);
	}
	return true;
}

bool ComThread::bridgeVersion(int *ver)
{
	unsigned char in[4];
	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}

	exchangeCommand(in, 255, 6, 0, 0);
	exchangeCommand(in, 0xFF, 20, 0, 0);
	if(in[0]!=0xFF || in[1]!=20){
		setError(tr("tr_err_br_responce"));
		return false;
	}
	*ver = in[2];
	return true;
}

bool ComThread::readAllFuses(unsigned char fuses[])
{
	unsigned char in[4];
	int i;
	unsigned char commands[5][2]={{0x50,0x00}, {0x58,0x08}, {0x50,0x08}, {0x58,0x00}, {0x38,0x00}};

	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}

	//Low, High, eXtension, (locK, Calibration)の順にヒューズを読む
	for(i=0; i<3; i++){
		exchangeCommand(in, commands[i][0],commands[i][1], 0, 0);
		fuses[i] = in[3];
	}

	return true;
}


bool ComThread::writeAllFuses(unsigned char fuses[])
{
	int i;
	unsigned char commands[3][2]={{0xAC,0xA0}, {0xAC,0xA8}, {0xAC,0xA4}};

	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}

	for(i=0; i<3; i++){
		exchangeCommand(NULL, commands[i][0], commands[i][1], 0, fuses[i]);
	}
	return true;
}


bool ComThread::inquirePinStates(int stt[])
{
	unsigned char in[4];
	int dsr, cts;
	int i;

	//初期値
	for(i=0; i<6; i++)
		stt[i] = PINSTATE_LOW;

	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}

	//RS-232Cのピン
	if(!io->modemStatus(&cts, &dsr, NULL, NULL))
		return false;
	if(dsr)
		stt[4] = PINSTATE_HIGH;
	if(cts)
		stt[5] = PINSTATE_HIGH;

	if(comif==COMIF_ASYNC){
		//ブリッジのピン
		if(!exchangeCommand(in, 0xFF, 14, 0, 0)){
			return false;
		}
		for(i=0; i<4; i++){
			if((in[2]&1))
				stt[i]=PINSTATE_HIGH;
			in[2] >>= 1;
		}


	}
	return true;
}


bool ComThread::setPinDirections(int dir[])
{
    int i;
    unsigned char ddr;
    unsigned char in[4];

    if(comif!=COMIF_ASYNC) return false;

    ddr = 0;  //1で出力、0で入力
	for(i=0; i<4; i++){
        ddr <<= 1;
		if(dir[4-1-i]==PINFUNC_MOMENTARY || dir[4-1-i]==PINFUNC_ALTERNATE){
            ddr |= 1;
        }
    }
	if(!exchangeCommand(in, 0xFF, 12, ddr, 0)){
		setError(trIOError());
        return false;	//DDR B設定
	}
    return true;
}

bool ComThread::setPinStates(int states[])
{
    int i;
    unsigned char port;
    unsigned char in[4];

    if(comif!=COMIF_ASYNC) return false;

    port = 0; //DDR=0[1でプルアップ、0でハイインピーダンス] DDR=1[1=high 0=low]
	for(i=0; i<4; i++){
        port <<= 1;
		if(states[4-1-i]==PINSTATE_HIGH || states[4-1-i]==PINSTATE_PULLUP){
            port |= 1;
        }
    }
	if(!exchangeCommand(in, 0xFF, 10, port, 0)){
		setError(trIOError());
		return false;
	}
	return true;
}

bool ComThread::resetTarget(bool execmode)
{
	bool r;
	unsigned char in[4];
	if(comif!=COMIF_ASYNC) return false;

	if(execmode){
		//実行モードでリセット(low->high)
		r = exchangeCommand(in, 0xFF, 6, 2, 0);
		r = exchangeCommand(in, 0xFF, 6, 3, 0);
	}
	else{
		//プログラミングモードでリセット(high->low)
		r = exchangeCommand(in, 0xFF, 6, 3, 0);
		r = exchangeCommand(in, 0xFF, 6, 2, 0);
		r = enableProgramming();
	}
	if(!r){
		setError(trIOError());
		return false;
	}
	return true;
}


bool ComThread::exchangeCommand(unsigned char *in, int a, int b, int c, int d)
{
    bool r;
    if(!iomutex.tryLock(3000))
        return false;

    if(comif == COMIF_ASYNC){
        unsigned char outbuf[4];
        QByteArray inbuf;
        outbuf[0] = a;
        outbuf[1] = b;
        outbuf[2] = c;
        outbuf[3] = d;
        if(!send(outbuf, 4)){
            iomutex.unlock();
			return false;
        }
        inbuf = receiveCommandResp(4, false);
        if(inbuf.length()!=4){
            iomutex.unlock();
			return false;
        }
		if(in)
			memcpy(in, inbuf.constData(), 4);
        iomutex.unlock();
        return true;
    }
    else if(comif == COMIF_SPI){
        r = io->exchangeSPI(in,a,b,c,d);
        iomutex.unlock();
        return r;
    }

    return false;
}

void ComThread::addCommand(QByteArray& data, int a, int b, int c, int d)
{
	char command[4];
	command[0] = (char)a;
	command[1] = (char)b;
	command[2] = (char)c;
	command[3] = (char)d;
	data.append(command,4);
}

bool ComThread::send(unsigned char *buf, int outlen)
{
    return io->send(buf, outlen);
}

QByteArray ComThread::receiveTargetMessage()
{
	QByteArray ret;
	unsigned char in[4];
	int r;

	while(1){
        //1単位読む
		r = io->receive(in, 4);
		if(r==4){
			//データが受信できたとき
			//ターゲットメッセージレスポンスの場合
			if(in[0]==0xFF && in[1]==0xF2){
				tgtrecvbuf.append(in[2]);
			}
			//コマンドレスポンスの場合
			else{
				cmdrecvbuf = QByteArray((const char*)in, 4);
			}
        }
		if(r!=4){
			//タイムアウトだとここに来る
			//ターゲットメッセージは必ずあるわけではないのでエラーではない
			break;
		}

	}


	//データがたまっていればそれを返す
	if(tgtrecvbuf.size()>0){
		ret = QByteArray(tgtrecvbuf.constData());	//deep copy
		tgtrecvbuf.clear();
	}
	return ret;
}

QByteArray ComThread::receiveCommandResp(int len, bool blockmode)
{
	QByteArray ret;
	unsigned char in[4];
	int nloop = len/4;
	int i;
	int r;

	nloop = (len - cmdrecvbuf.size())/4;
	for(i=0; i<nloop; ){
		r = io->receive(in, 4);
		if(r==4){
			//データが受信できたとき
			//ターゲットメッセージレスポンスの場合
			if(in[0]==0xFF && in[1]==0xF2 && !blockmode){
				tgtrecvbuf.append(in[2]);
			}
			//コマンドレスポンスの場合
			else{
				cmdrecvbuf.append((const char*)in, 4);
				i++;
			}
        }

		if(r!=4){
			//タイムアウト等
			//ターゲットメッセージと違い、コマンドレスポンスは
			//タイムアウト時間中に十分返るはずなので何らかのエラーが発生している
			break;
		}
	}

	//エラーがなければ要求されたデータ長を受信しているはずなのでそれを返す
	if(cmdrecvbuf.size()>=len){
		ret = QByteArray(cmdrecvbuf.constData(),len);	//deep copy
		cmdrecvbuf.remove(0,len);
	}
	return ret;
}



//////////////////////////////////////////////////////////////////////////////
ComThread::ThreadEvent::ThreadEvent()
:QEvent(QEvent_Thread)
{

}

ComThread::ThreadEvent::ThreadEvent(int o)
:QEvent(QEvent_Thread)
{
	ope = o;
	max = 0;
	val = 0;
	finished = false;
	retcd = 0;
}

ComThread::ThreadEvent::ThreadEvent(int o, int retcode, QString &errmsg)
:QEvent(QEvent_Thread)
{
	ope = o;
	max = 0;
	val = 0;
	finished = false;
	retcd = 0;
	setErrorMessage(errmsg);
	setFinished(true);
	setReturnCode(retcode);
}
