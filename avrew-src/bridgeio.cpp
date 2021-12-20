#include "bridgeio.h"
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")



BridgeIO::BridgeIO()
{
	hcom = NULL;
	comport[0] = '\0';
	bitrate = 0;
	blksize = 32;
	spidelay = 100;


}

BridgeIO::~BridgeIO()
{

}

bool BridgeIO::openRS232C(const char* port, int bps)
{
    DCB dcb;
    COMMTIMEOUTS comto;
	wchar_t comportw[100];
	int timeout;

    if(hcom != INVALID_HANDLE_VALUE && hcom != NULL){
        return true;
    }

	sprintf(comport, "\\\\.\\%s", port);
	//strcpy(comport, port);
	mbstowcs(comportw, comport, sizeof(comportw));
	bitrate = bps;

    hcom = CreateFile(
		comportw, GENERIC_READ|GENERIC_WRITE,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL );
    if(hcom == INVALID_HANDLE_VALUE){
		errcode = COMERR_PORT;
        return false;
    }

    //送受信バッファ確保
    if(!SetupComm(hcom, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE)){
		errcode = COMERR_BUFFER;
        return false;
    }

    //通信条件設定
    dcb.DCBlength = sizeof(DCB);
    memset(&dcb, 0, sizeof(DCB));
	dcb.BaudRate = 9600;	//CH340の不具合?9600bpsで一度初期化しないとボーレートが有効にならない
	//dcb.BaudRate = bitrate;
    dcb.fBinary = TRUE;
    dcb.ByteSize = 8;
    dcb.fParity = FALSE;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    //ハードウェアフロー制御を行わない
    dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
    //出力ピンはフロー制御ではなく独自にON/OFFする
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
    // ソフトウェアフロー制御なし
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    if(!SetCommState(hcom, &dcb)){  //SetCommState()関数にポートハンドルおよびdcb構造体のアドレスを代入する
		 errcode = COMERR_PORT;
         return false;
    }

	//本来のビットレートで設定し直す
	dcb.BaudRate = bitrate;
	SetCommState(hcom, &dcb);

    //タイムアウト時間
	switch(bitrate){
		case 115200:
			timeout = 30; break;
		case 14400:
			timeout = 100; break;
		case 1800:
			timeout = 300; break;
	}

	comto.ReadIntervalTimeout = 0; //文字間隔の最大待ち時間(ミリ秒) NT系では無効
	comto.ReadTotalTimeoutMultiplier = 0; //文字数 NT系では無効
	comto.ReadTotalTimeoutConstant = timeout;//ReadFileのタイムアウト時間

	comto.WriteTotalTimeoutMultiplier = 0; //文字間隔の最大待ち時間 NT系では無効
	comto.WriteTotalTimeoutConstant = timeout;//WriteFile全体の最大待ち時間
    if( !SetCommTimeouts(hcom, &comto) ){
		errcode = COMERR_PORT;
        return false;
    }

    //監視するイベント
    //SetCommMask(hcom, EV_RXCHAR);

    //ピンクリア
    if( !EscapeCommFunction(hcom, CLRRTS) ){
		errcode = COMERR_PIN;
        return false;
    }
    if( !EscapeCommFunction(hcom, CLRDTR) ){
		errcode = COMERR_PIN;
        return false;
    }
	Sleep(100);

    //送受信バッファクリア
    if(!PurgeComm(hcom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)){
		errcode = COMERR_BUFFER;
        return false;
    }

    return true;
}

void BridgeIO::closeRS232C()
{
    if(hcom){
        CloseHandle(hcom);
        hcom = NULL;
    }
}

bool BridgeIO::isOpend()
{
    return hcom==NULL?false:true;
}

bool BridgeIO::clearBuffer()
{
	if(!hcom){
		errcode = COMERR_PORT;
		return false;
	}
	if(!PurgeComm(hcom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)){
		errcode = COMERR_BUFFER;
		return false;
	}
	return true;
}

int BridgeIO::getInQue()
{
	DWORD dwErrors;
	COMSTAT ComStat;

	ClearCommError(hcom, &dwErrors, &ComStat);
	return (int)ComStat.cbInQue;
}


bool BridgeIO::modemStatus(int *cts, int *dsr, int *ring, int *rlsd)
{
    DWORD stat;
    BOOL r;
    r = ::GetCommModemStatus(hcom, &stat);
	if(r==FALSE){
		errcode = COMERR_PIN;
        return false;
	}

    if(cts)
        *cts = stat & MS_CTS_ON?1:0;
    if(dsr)
        *dsr = stat & MS_DSR_ON?1:0;
    if(ring)
        *ring = stat & MS_RING_ON?1:0;
    if(rlsd)
        *rlsd = stat & MS_RLSD_ON?1:0;

    return true;
}

bool BridgeIO::escapeRTS(bool stat)
{
    BOOL r;
    if(stat)
        r = EscapeCommFunction(hcom, SETRTS);
    else
        r = EscapeCommFunction(hcom, CLRRTS);
	if(r==FALSE){
		errcode = COMERR_PIN;
		return false;
	}
    return (bool)r;
}


bool BridgeIO::send(unsigned char *buf, int outlen)
{
	int sendlen, sendleft, index;
	unsigned char sendbuf[4096];
	int sendbufsize = (int)sizeof(sendbuf);
	DWORD wlen;
	char debugstr[10000], cmdstr[100];
	debugstr[0]='\0';
	int i;

	//バッファサイズをブロック長の倍数になるよう合わせる
	sendlen = (((int)(outlen-1)/blksize)+1) * blksize;
	if(sendlen > sendbufsize)
		return false;

	//デバッグがやりやすくなるようオシロで見やすい波形でパディング
	for(i=0; i<sendlen; i++){
		if(i%4==0) sendbuf[i] = 0xFF;
		else if(i%4==1) sendbuf[i] = 0xFF;
		else if(i%4==2) sendbuf[i] = 0xCC;  //0b11001100
		else if(i%4==3) sendbuf[i] = 0xAA;  //0b10101010
	}

	memcpy(sendbuf+(sendlen-outlen), buf, outlen);

	index = 0;
	sendleft = sendlen;
	while (sendleft > 0){
		if(WriteFile(hcom, sendbuf+index, sendleft, &wlen, NULL)==FALSE){
			errcode = COMERR_SEND;
			return false;
		}
		index += wlen;
		sendleft -= wlen;
	}

	return true;
}

unsigned long g_eltime;

int BridgeIO::receive(unsigned char *buf, int maxlen)
{
	DWORD readlen;

	DWORD start = timeGetTime();
	::ReadFile(hcom, buf, maxlen, &readlen, NULL);
	DWORD end = timeGetTime();
	g_eltime = end - start;


	if(readlen!=maxlen){
		errcode = COMERR_RECEIVE;
		return 0;
	}
	return maxlen;
}


bool BridgeIO::exchangeSPI(unsigned char *in, int a, int b, int c, int d)
{
    unsigned char out[4];
    int dat, ret;
    DWORD modemstt;
    int i,j;
	BOOL r;

    out[0]=a; out[1]=b; out[2]=c; out[3]=d;
    for(i=0; i<4; i++){
        dat = out[i];
        ret = 0;
		for(j=0; j<8; j++){
			r = TRUE;
			//送信データセット(MSB先行)
            if(dat&0x80)
				r &= EscapeCommFunction(hcom, SETRTS);
            else
				r &= EscapeCommFunction(hcom, CLRRTS);
			dat <<= 1;

            //クロック上昇
			EscapeCommFunction(hcom, SETDTR);
			if(spidelay!=0)
				usleep(spidelay);

			//受信
			ret <<= 1;
			r &= GetCommModemStatus(hcom, &modemstt);
			if(modemstt & MS_RLSD_ON) ret |= 1;

			//クロック下降
			r &= EscapeCommFunction(hcom, CLRDTR);
			if(spidelay!=0)
				usleep(spidelay);


			if(r==FALSE){
				errcode =COMERR_PIN;
				return false;
			}
        }
        if(in)
			in[i] = ret;
    }
	return true;
}


void usleep(unsigned long waitTime){
    LARGE_INTEGER perfCnt, start, now;

    QueryPerformanceFrequency(&perfCnt);
    QueryPerformanceCounter(&start);

    do {
        QueryPerformanceCounter((LARGE_INTEGER*) &now);
    } while ((now.QuadPart - start.QuadPart) / float(perfCnt.QuadPart) * 1000 * 1000 < waitTime);
}
