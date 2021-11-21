#ifndef __DEVICE__
#define __DEVICE__


#include "QDomDocument"


enum{
    FUSE_LOW = 0,
    FUSE_HIGH = 1,
    FUSE_EXT = 2,
    FUSE_LOCK = 3,
    FUSE_CALIB = 4
};

struct DeviceSpec
{
    QString name;
    QString signature;
    int flashpagesize;  //単位はバイト
    int nflashpages;
    int eeppagesize;
    int neeppages;
    QString fusedescs[4][8];

    DeviceSpec(){
        flashpagesize=0;
        nflashpages=0;
        eeppagesize=0;
        neeppages=0;
        //fusedesc[FUSE_LOW];
    }
};

class DeviceInfo: public QDomDocument
{
protected:
    bool readSpec(QString name, DeviceSpec *spec);

public:
	DeviceInfo();
    bool openFile(const QString &path, QString * errorMsg = 0, int * errorLine = 0, int * errorColumn = 0);
    QStringList enumDevices();
    bool deviceSpec(QString signature,  DeviceSpec* spec);
    int inspect(QString signature, bool bitstate[][8], QString *retmsg);

};


#endif

