#include <QFile>
#include <qstringlist.h>
#include <stdio.h>
#include <string.h>
#include "deviceinfo.h"

#define TOPLEVELTAG "avrdevices"

DeviceInfo::DeviceInfo()
:QDomDocument()
{
}

bool DeviceInfo::openFile(const QString &path, QString *errorMsg, int *errorLine, int *errorColumn)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){
        if(errorMsg){
            errorMsg->clear();
            errorMsg->append("file open error:" + path);
            if(errorLine)
                *errorLine = -1;
            if(errorColumn)
                *errorColumn = -1;
        }
        return false;
    }
    if (!setContent(&file, errorMsg, errorLine, errorColumn)) {
        file.close();
        return false;
    }
    file.close();

    //xmlが正しいか
    if(firstChildElement(TOPLEVELTAG).isNull())
        return false;
    return true;
}

QStringList DeviceInfo::enumDevices()
{
    QStringList ret;
    QDomElement elm;

    elm = firstChildElement(TOPLEVELTAG);
    elm = elm.firstChildElement("device");
    while(!elm.isNull()){
        if(elm.attribute("signature")!=""){
            ret << elm.attribute("name");
            ret << elm.attribute("signature");
        }
        elm = elm.nextSiblingElement();
    }

    return ret;
}

bool DeviceInfo::deviceSpec(QString signature, DeviceSpec* spec)
{
    QDomElement elm;
    QString inherit;

    //signatureが一致するエレメントを探す
    elm = firstChildElement(TOPLEVELTAG);
    elm = elm.firstChildElement("device");
    while(!elm.isNull()){
        if(elm.attribute("signature") == signature)
            break;
        elm = elm.nextSiblingElement();
    }
    if(elm.isNull())
        return false;

    //継承があれば継承元を先に読む
    inherit = elm.attribute("inherit");
    if(inherit!=""){
        readSpec(inherit, spec);
    }
    //本来のデバイスを読む
    return readSpec(elm.attribute("name"), spec);
}


//inherit属性のための下位関数
bool DeviceInfo::readSpec(QString name, DeviceSpec *spec)
{
    QDomElement elm;
    int crtfuse;
    int i;
    QString attrib;

    elm = firstChildElement(TOPLEVELTAG);

    //nameが一致するデバイスを探す
    elm = elm.firstChildElement("device");
    while(!elm.isNull()){
        if(elm.attribute("name") == name)
            break;
        elm = elm.nextSiblingElement();
    }
    if(elm.isNull())
        return false;

    //一致するsignatureのデバイスがあったので、スペックをコピーする
    spec->name = name;
    spec->signature = elm.attribute("signature");
    elm = elm.firstChildElement("flash");
    if(!elm.isNull()){
        if(elm.hasAttribute("pagesize"))
            spec->flashpagesize = elm.attribute("pagesize").toInt();
        if(elm.hasAttribute("npages"))
            spec->nflashpages = elm.attribute("npages").toInt();
    }
    elm = elm.parentNode().toElement();
    elm = elm.firstChildElement("eeprom");
    if(!elm.isNull()){
        if(elm.hasAttribute("pagesize"))
            spec->eeppagesize = elm.attribute("pagesize").toInt();
        if(elm.hasAttribute("npages"))
            spec->neeppages = elm.attribute("npages").toInt();
    }
    elm = elm.parentNode().toElement();
    //ヒューズ
    elm = elm.firstChildElement("fuse");
    while(!elm.isNull()){
        crtfuse = -1;
        if(elm.attribute("name")=="low") crtfuse = FUSE_LOW;
        else if(elm.attribute("name")=="high") crtfuse = FUSE_HIGH;
        else if(elm.attribute("name")=="ext") crtfuse = FUSE_EXT;

        if(crtfuse>=0){
            for(i=0; i<8; i++){
                attrib = QString("bit%1").arg(i);
                if(elm.hasAttribute(attrib))
                   spec->fusedescs[crtfuse][i] = elm.attribute(attrib);
            }
        }
        elm = elm.nextSiblingElement("fuse");
    }

    return true;
}


//現在のヒューズがwarning、errorに該当するか調べ、
//warningの時は0、errorの時は-1を、該当なしの場合は1を返す。
int DeviceInfo::inspect(QString signature, bool bitstate[][8], QString *retmsg)
{
    QString elmname, pattern;
    int i;
    QDomElement elm;
    int targetfuse;
    bool matched;

    //signatureが一致するエレメントを探す
    elm = firstChildElement(TOPLEVELTAG);
    elm = elm.firstChildElement("device");
    while(!elm.isNull()){
        if(elm.attribute("signature") == signature)
            break;
        elm = elm.nextSiblingElement();
    }
    if(elm.isNull()){
        *retmsg = QObject::tr("%1に一致するデバイスが見つかりません。").arg(signature);
        return -1;
    }


    //トラバース
    elm = elm.firstChildElement();
    while(!elm.isNull()){
        elmname = elm.tagName();
        if(elmname=="warning" || elmname=="error"){
            pattern = elm.attribute("pattern");
            if(pattern.length()!=8) goto NEXT_ELM;

            if(elm.attribute("fuse")=="low") targetfuse=FUSE_LOW;
            else if(elm.attribute("fuse")=="high") targetfuse=FUSE_HIGH;
            else if(elm.attribute("fuse")=="ext") targetfuse=FUSE_EXT;
            else goto NEXT_ELM;

            matched = true;
            for(i=0; i<8; i++){
                if( ( pattern.at(7-i)=='0' && bitstate[targetfuse][i] ) ||
                    ( pattern.at(7-i)=='1' && !bitstate[targetfuse][i] ) ){
                    matched = false;
                    break;
                }
            }

            if(matched){
                *retmsg = elm.attribute("message");
                if(elmname=="warning"){
                    return 0;
                }
                else{
                    return -1;
                }
            }
        }
NEXT_ELM:
        elm = elm.nextSiblingElement();
    }

    return 1;
}

/*
//現在のヒューズがwarning、errorに該当するか調べ、
//該当すればメッセージボックスを出す。該当しなければtrueを返す。
//warningの時はメッセージボックスの返り値を返す。
//errorの時は常にfalseを返す。
bool FuseBits::inspect()
{
    QString elmname, pattern, message;
    int i;
    UDomElement *elm;
    int targetfuse;
    bool matched;

    elm = findDevice(crtdevice);
    if(!elm) return true;

    elm = (UDomElement*)elm->getFirstChild();
    while(elm){
        if(elm->getNodeType()!=ELEMENT_NODE) goto NEXT_ELM;

        elmname = tu(elm->getNodeName());
        if(elmname=="warning" || elmname=="error"){
            pattern = elm->getAttribute("pattern").data();
            if(pattern.length()!=8) goto NEXT_ELM;

            if(elm->getAttribute("fuse")=="low") targetfuse=FUSE_LOW;
            else if(elm->getAttribute("fuse")=="high") targetfuse=FUSE_HIGH;
            else if(elm->getAttribute("fuse")=="ext") targetfuse=FUSE_EXT;
            else goto NEXT_ELM;

            matched = true;
            for(i=0; i<8; i++){
                if( ( pattern.at(7-i)=='0' && bitstate[targetfuse][i]->isChecked() ) ||
                    ( pattern.at(7-i)=='1' && !bitstate[targetfuse][i]->isChecked() ) ){
                    matched = false;
                    break;
                }
            }

            if(matched){
                message = tu(elm->getAttribute("message"));
                if(elmname=="warning"){
                    if( QMessageBox::warning(topLevelWidget(), "", message,
                        QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
                        return false;
                }
                else{
                    QMessageBox::critical(topLevelWidget(), "", message);
                    return false;
                }
            }
        }
NEXT_ELM:
        elm = (UDomElement*)elm->getNextSibling();
    }

    return true;
}
*/

