#ifndef COMTHREAD_H
#define COMTHREAD_H

#include <QThread>
#include <QEvent>
#include <QVariant>
#include <QBuffer>
#include <QMutex>
#include <QQueue>
#include "bridgeio.h"
#include "deviceinfo.h"

#define QEvent_Thread (QEvent::Type)(QEvent::User+100)

enum ComIF{
    COMIF_NULL = 0,
    COMIF_ASYNC = 1,
    COMIF_SPI = 2
};

enum{
    COMOP_NULL=0,
    COMOP_WRITE=1,
	//COMOP_READFLASH=2,
	//COMOP_READEEPROM=3,
	COMOP_READ=4,
    COMOP_TARGETEXEC=5,
    COMOP_PROGRESS=100
};

class ComThread : public QThread
{
    bool continue_loop;
    bool bridge_ready;
    QObject *receiver;
    DeviceSpec *devspec;
    QList<QVariant> varlist;
    int operation;
    bool toverify;
    BridgeIO *io;
    int comif;
    QString errormsg;

	QMutex iomutex;
	QQueue<unsigned char> tgtsendbuf;
	QByteArray tgtrecvbuf;
	QByteArray cmdrecvbuf;

	void setError(const QString &msg);
	QString trIOError();

    Q_OBJECT
public:
    explicit ComThread(QObject *parent = 0);
	BridgeIO *bridgeIo(){return io;}
	void setTargetSpec(DeviceSpec *spec);
    int comportMode(){return comif;}
	bool openConnection(QString comport, int bps, int comifmode, int spidelay);
	void closeConnection();
	bool clearBuffer();
	QString errorMessage(){return errormsg;}

    //スレッド系
    void run();
    bool startWrite(QObject *receiver, QByteArray flashimg, QByteArray eepimg);
	bool startRead(QObject *receiver, int flashsize, int eepsize);
	bool startTargetExecution(QObject *receiver);

    void runWriteAsync(QByteArray flashimg, QByteArray eepimg);
    void runWriteSPI(QByteArray flashimg, QByteArray eepimg);
	void runReadAsync(int flashsize, int eepsize);
	void runReadSPI(int flashsize, int eepsize);

	void runReadFlash();
	void runReadEEPROM();
	void runTarget();
	void stopTarget();
	bool sendToTarget(QByteArray data);

    //ヒューズ読み書きやデバイス問い合わせなどは一瞬で終わるのでスレッドを使わない
	bool enableProgramming();
    bool readSignature(QString *sig);
    bool bridgeVersion(int *ver);
    bool readAllFuses(unsigned char[]);
    bool writeAllFuses(unsigned char[]);
    bool inquirePinStates(int stt[]);
    bool setPinDirections(int directions[4]);
    bool setPinStates(int states[4]);
	bool resetTarget(bool execmode);

	//送受信低レベル関数
	bool exchangeCommand(unsigned char *in, int a, int b, int c, int d);
	bool send(unsigned char *buf, int outlen);
	QByteArray receiveTargetMessage();
    QByteArray receiveCommandResp(int len, bool blockmode);

protected:
    void postProgressEvent(int value, int max);
	void addCommand(QByteArray& data, int a, int b, int c, int d);

signals:

public slots:

public:
    class ThreadEvent: public QEvent
    {
        int max;
        int val;
        QVariant params[4];
        int ope;
        bool finished;
        int retcd;
		QString errmsg;

    public:
        ThreadEvent();
        ThreadEvent(int o);
		ThreadEvent(int o, int retcode, QString& errmsg);
        void setOperation(int o){ope=o;}
        void setMaximum(int m){max=m;}
        void setValue(int v){val=v;}
        void setValueMax(int v, int m){val=v, max=m;}
        void setParameter(QVariant var, int pos){params[pos]=var;}
        void setFinished(bool b){finished = b;}
        void setReturnCode(int i){retcd = i;}
		void setErrorMessage(QString& msg){errmsg=msg;}

        int operation(){return ope;}
        int maximum(){return max;}
        int value(){return val;}
        QVariant parameter(int pos){return params[pos];}
        bool isFinished(){return finished;}
        int returnCode(){return retcd;}
		QString errorMessage(){return errmsg;}
    };

};

#endif // COMTHREAD_H
