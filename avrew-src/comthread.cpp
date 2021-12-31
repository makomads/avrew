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
	case COMOP_READ:
		if(comif == COMIF_ASYNC)
			runReadAsync(varlist[0].toInt(), varlist[1].toInt());
		else if(comif == COMIF_SPI)
			runReadSPI(varlist[0].toInt(), varlist[1].toInt());
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
	int pos;
	int realimgsize;
	int npages;
	int blocksize = 32;
	int nblocks;
    QByteArray senddata, recvdata;
	unsigned char in[4];

	ev = new ThreadEvent(operation);
	ev->setValueMax(0,0);
	QApplication::postEvent(receiver, ev);

	//Flash,EEPROM消去
	if(!exchangeCommand(in, 0xAC, 0x80, 0, 0)){
		QApplication::postEvent(receiver, new ThreadEvent(operation, -1, trIOError()));
		return;
	}
	msleep(10);

	//イメージをページまたはブロックの大きい方の境界になるよう拡張し、余分な部分はFFで埋める
	realimgsize = flashimg.size();
	if(devspec->flashpagesize > blocksize){
		npages = (flashimg.length() + devspec->flashpagesize-1) / devspec->flashpagesize;
		flashimg.resize(npages * devspec->flashpagesize);
		nblocks = flashimg.size() / blocksize;
	}
	else{
		nblocks = (flashimg.length() + blocksize-1) / blocksize;
		flashimg.resize(nblocks * blocksize);
		npages = flashimg.size() / devspec->flashpagesize;
	}
	memset(flashimg.data()+realimgsize, 0xFF, flashimg.size()-realimgsize);

    //ページサイズ設定(上位、下位の順)
	exchangeCommand(in, 0xFF, 0xCE, (devspec->flashpagesize>>8)&0xFF, devspec->flashpagesize&0xFF);
	//連続書き込み開始
	exchangeCommand(in, 0xFF, 0xC0, (nblocks>>8)&0xFF, nblocks&0xFF);

	//書き込み
	postProgressEvent(0, flashimg.size());
	pos = 0;
	while(pos < flashimg.size()){
		//送信
		senddata = flashimg.mid(pos, blocksize);
		pos += blocksize;
		send((unsigned char*)senddata.data(), blocksize);
		senddata.clear();
		//受信
		recvdata = receiveCommandResp(4, false);
		postProgressEvent(pos, flashimg.size());
	}
	postProgressEvent(pos, flashimg.size());

	//EEPROM
	if(eepimg.size()!=0){
		realimgsize = eepimg.size();
		if(devspec->eeppagesize > blocksize){
			npages = (eepimg.length() + devspec->eeppagesize-1) / devspec->eeppagesize;
			eepimg.resize(npages * devspec->eeppagesize);
			nblocks = eepimg.size() / blocksize;
		}
		else{
			nblocks = (eepimg.length() + blocksize-1) / blocksize;
			eepimg.resize(nblocks * blocksize);
			npages = eepimg.size() / devspec->eeppagesize;
		}
		memset(eepimg.data()+realimgsize, 0xFF, eepimg.size()-realimgsize);

		//ページサイズ設定(上位、下位の順)
		exchangeCommand(in, 0xFF, 0xCE,
						(devspec->eeppagesize>>8) & 0xFF, devspec->eeppagesize&0xFF);
		//連続書き込み開始
		exchangeCommand(in, 0xFF, 0xC2, (nblocks>>8)&0xFF, nblocks&0xFF);

		//書き込み
		postProgressEvent(0, eepimg.size());
		pos = 0;
		while(pos < eepimg.size()){
			//送信
			senddata = eepimg.mid(pos, blocksize);
			pos += blocksize;
			send((unsigned char*)senddata.data(), blocksize);
			senddata.clear();
			//受信
			recvdata = receiveCommandResp(4, false);
			postProgressEvent(pos, eepimg.size());
		}
		postProgressEvent(eepimg.size(), eepimg.size());
	}

	//完了
	ev = new ThreadEvent(operation);
	ev->setFinished(true);
	ev->setParameter(flashimg, 0);
	ev->setParameter(eepimg, 1);
	QApplication::postEvent(receiver, ev);
}

void ComThread::runWriteSPI(QByteArray flashimg, QByteArray eepimg)
{
	ThreadEvent *ev;
	int i,j,r=0;
	int realimgsize;
	int npages;
	int addr;
	unsigned char out, in[4];
	int cntunmatch=0;

	//Flashイメージをページ境界になるよう拡張する
	realimgsize = flashimg.size();
	npages = (flashimg.length() + devspec->flashpagesize-1) / devspec->flashpagesize;
	flashimg.resize(npages * devspec->flashpagesize);
	memset(flashimg.data()+realimgsize, 0xFF, flashimg.size() - realimgsize);

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
		realimgsize = eepimg.size();
		npages = (eepimg.length() + devspec->eeppagesize-1) / devspec->eeppagesize;
		eepimg.resize(npages * devspec->eeppagesize);
		memset(eepimg.data()+realimgsize, 0xFF, eepimg.size() - realimgsize);

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
	ev = new ThreadEvent(operation);
	ev->setFinished(true);
	ev->setParameter(flashimg, 0);
	ev->setParameter(eepimg, 1);
	QApplication::postEvent(receiver, ev);
}



bool ComThread::startRead(QObject *receiver, int flashsize, int eepsize)
{
	if(!io->isOpend()){
		setError(tr("tr_err_ionotopend"));
		return false;
	}
	this->receiver = receiver;
	operation = COMOP_READ;
	varlist.clear();
	varlist.append(flashsize);
	varlist.append(eepsize);
    wait();
	start();
	return true;
}


void ComThread::runReadAsync(int flashsize, int eepsize)
{
	ThreadEvent *ev;
	QByteArray recvdata;
	int opcnt;
	unsigned char in[4];
	int nblocks;
	int blksize = 32; //読み込みはバイト単位だがある程度まとめて読む
	int pos;
	int imagesize, realsize;
	int blockmodetype;
	QString errmsg;
	QByteArray flashimg, eepimg, *pimg;

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

	//FlashとEEPROMで2回ループ
	for(opcnt=0; opcnt<2; opcnt++){
		if( (opcnt==0 && flashsize<=0) || (opcnt==1 && eepsize<=0) )
			continue;

		//ブロック数
		if(opcnt==0){
			realsize = flashsize;
			blockmodetype = 0xC1;
			pimg = &flashimg;
		}
		else if(opcnt==1){
			realsize = eepsize;
			blockmodetype = 0xC3;
			pimg = &eepimg;
		}
		nblocks = (realsize + blksize-1) / blksize;
		imagesize = nblocks * blksize;

		//連続読み込み開始
		exchangeCommand(in, 0xFF, blockmodetype, (nblocks>>8)&0xFF, nblocks&0xFF);

		//読み込み
		postProgressEvent(0, imagesize);
		pos = 0;
		while(pos < imagesize){
			//受信
			recvdata = receiveCommandResp(blksize, true);
			pimg->append(recvdata, blksize);
			pos += blksize;
			postProgressEvent(pos, imagesize);
		}
		postProgressEvent(pos, imagesize);
		pimg->resize(realsize);
	}

	//完了イベント送出
	ev = new ThreadEvent(operation, 0, QString(""));
	ev->setFinished(true);
	ev->setValueMax(imagesize,imagesize);
	ev->setParameter(flashimg, 0);
	ev->setParameter(eepimg, 1);
	QApplication::postEvent(receiver, ev);
}

void ComThread::runReadSPI(int flashsize, int eepsize)
{
	ThreadEvent *ev;
	int i;
	unsigned char in[4];
	int addr;
	int realsize;
	int blksize=32;
	int opcnt;
	QByteArray flashimg, eepimg, *pimg;

	for(opcnt=0; opcnt<2; opcnt++){
		if(opcnt == 0){
			realsize = flashsize;
			pimg = &flashimg;
		}
		else if(opcnt == 1){
			realsize = eepsize;
			pimg = &eepimg;
		}
		pimg->resize(realsize);

		postProgressEvent(0, realsize);
		for(i=0; i<realsize; i++){
			if( (opcnt==0 && flashsize<=0) || (opcnt==1 && eepsize<=0) )
				continue;
			if(opcnt == 0){
				addr = i/2;
				if(i%2==0)
					exchangeCommand(in, 0x20, (addr>>8)&0xFF, addr&0xFF, 0);
				else
					exchangeCommand(in, 0x28, (addr>>8)&0xFF, addr&0xFF, 0);
			}
			else if(opcnt == 1){
				addr = i;
				exchangeCommand(in, 0xA0, (addr>>8)&0xFF, addr&0xFF, 0);
			}
			(*pimg)[i] = in[3];
			if(i%blksize == 0)
				postProgressEvent(i, realsize);
		}
		postProgressEvent(realsize, realsize);
	}

	//完了イベント送出
	ev = new ThreadEvent(operation, 0, QString(""));
	ev->setFinished(true);
	ev->setParameter(flashimg, 0);
	ev->setParameter(eepimg, 1);
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

	msleep(100);
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

	//exchangeCommand(in, 255, 6, 0, 0);
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

bool ComThread::setSPIDelayForAsync(float usec)
{
	int delayval;
	unsigned char in[4];
	delayval = (int)(usec / 0.3);

	exchangeCommand(in, 0xFF, 21, delayval, 0);
	if(in[0]!=0xFF || in[1]!=21){
		setError(tr("tr_err_br_responce"));
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
        r = io->receive(in, 4, 1);
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
