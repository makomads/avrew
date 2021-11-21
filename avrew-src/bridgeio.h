#ifndef BRIDGEIO_H
#define BRIDGEIO_H

void usleep(unsigned long waitTime);
#define MAX_BUFFER_SIZE 4096

enum{
	COMERR_NONE = 0,
	COMERR_PORT = -1,
	COMERR_BUFFER = -2,
	COMERR_PIN = -3,
	COMERR_SEND = -11,
	COMERR_RECEIVE = -12
};

class BridgeIO
{
    char comport[100];
    int bitrate;
    void *hcom; //COMポートのHANDLEだがヘッダでwindows.hをincludeしたくないのでvoid*を使う
    int blksize;
    int errcode;
	int spidelay;

public:
    BridgeIO();
    ~BridgeIO();
    bool openRS232C(const char* comport, int bps);
    void closeRS232C();
    bool isOpend();
	bool clearBuffer();
	int getInQue();
    bool modemStatus(int *cts, int *dsr, int *ring, int *rlsd);
    bool escapeRTS(bool stat);

    bool send(unsigned char *buf, int len);
    int receive(unsigned char *buf, int maxlen);
    bool exchangeSPI(unsigned char *in, int a, int b, int c, int d);
	void setSPIDelay(int delay){spidelay = delay;}


	int errorCode(){int e=errcode; errcode=COMERR_NONE; return e;}
    char* comPort(){return comport;}
    int bitRate(){return bitrate;}

};

#endif // BRIDGEIO_H
