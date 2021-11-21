#include "functionbutton.h"
#include "configdialog.h"
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

FunctionButton::FunctionButton(int number, QWidget *parent) :
    QPushButton(parent)
{
    pinnumber = number;
    func = PINFUNC_MOMENTARY;
    pinstt1 = PINSTATE_HIGH;
    pinstt2 = PINSTATE_LOW;
    crtstat = pinstt1;

    sttstr.insert(PINSTATE_HIGH, "H");
    sttstr.insert(PINSTATE_LOW, "L");
	
	chigh = QColor(255, 192, 192);
    clow = QColor(128, 64, 64);
	creadhigh = QColor(192, 255, 192);
    creadlow = QColor(64, 128, 64);
    clinkhigh = QColor(192, 192, 255);
    clinklow = QColor(64, 64, 128);

	
    assignColor(pinstt1, pinstt2);

}

FunctionButton::~FunctionButton()
{
}

void FunctionButton::setFunction(int function)
{
    func = function;

    if(func==PINFUNC_ALTERNATE){
        setCheckable(true);
    }else if(func==PINFUNC_MOMENTARY){
        setCheckable(false);
	}else if(func==PINFUNC_INPUT){
        setCheckable(false);
        assignColor(PINSTATE_LOW, PINSTATE_LOW);
        setText(sttstr.value(PINSTATE_LOW,""));
    }else if(func==PINFUNC_DISABLED){
        setCheckable(false);
        assignColor(PINSTATE_DISABLED, PINSTATE_DISABLED);
        setText("");
    }else if(func==PINFUNC_LINK){
        setCheckable(false);
        assignColor(PINSTATE_LOW, PINSTATE_LOW);
        setText("");
    }
}

void FunctionButton::assignPinState(int stt1, int stt2)
{
	if(func==PINFUNC_INPUT || func==PINFUNC_DISABLED){
		pinstt1 = pinstt2 = stt1;
		crtstat = pinstt1;
        return;
	}

    pinstt1 = stt1;
    pinstt2 = stt2;
    assignColor(stt1, stt2);
    setText(sttstr.value(pinstt1,""));
    crtstat = pinstt1;
    setChecked(false);
}

void FunctionButton::setInputState(int pinstate)
{
    if(func!=PINFUNC_INPUT && func!=PINFUNC_LINK)
        return;
    crtstat = pinstate;
    assignColor(pinstate, pinstate);
    setText(sttstr.value(pinstate,""));
}


//離した状態、押した状態それぞれの色を変更する
void FunctionButton::assignColor(int releasedstt, int pressedstt)
{
	QString basestyle, addedstyle;
	QColor *creleased, *cpressed;
	int stt;
	QColor *pcolor;

	for(int i=0; i<2; i++){
		if(i==0){
			stt = releasedstt;
		}else{
			stt = pressedstt;
		}
		
        if(func==PINFUNC_INPUT){
			if(stt == PINSTATE_HIGH)
                pcolor = &creadhigh;
			else
                pcolor = &creadlow;
		}
        else if(func==PINFUNC_LINK){
            if(stt == PINSTATE_HIGH)
                pcolor = &clinkhigh;
            else
                pcolor = &clinklow;
        }
		else{
			switch(stt){
			case PINSTATE_HIGH:
                pcolor = &chigh;
				break;
            case PINSTATE_LOW:
                pcolor = &clow;
                break;
			}
		}
		
		if(i==0){
			creleased = pcolor;
		}else{
			cpressed = pcolor;
		}
	}
	
	basestyle=tr(
            "QPushButton{"
            "width:24px; height:24px; border: 3px solid darkgray; border-style: outset;"
            "} "
            "QPushButton:pressed{"
            "border-style:inset;"
            "}");
	addedstyle = tr(
            "QPushButton{"
            "background-color:rgb(%1,%2,%3);color:white;"
            "} "
            "QPushButton:pressed{"
            "background-color:rgb(%4,%5,%6); color:white;"
            "}").arg(creleased->red()).arg(creleased->green()).arg(creleased->blue())
            .arg(cpressed->red()).arg(cpressed->green()).arg(cpressed->blue());
	
    setStyleSheet(basestyle + addedstyle);
}

void FunctionButton::emitPinStateChanged(int pinstate)
{
    crtstat = pinstate;
    emit pinStateChanged(pinnumber, pinstate);
}

void FunctionButton::mousePressEvent(QMouseEvent *e)
{
    switch(func){
    case PINFUNC_ALTERNATE:
        if(e->button()==Qt::LeftButton){
            if(isChecked()){
                assignColor(pinstt1, pinstt1);
                setText(sttstr.value(pinstt1,""));
                emitPinStateChanged(pinstt1);
            }
            else{
                assignColor(pinstt2, pinstt2);
                setText(sttstr.value(pinstt2,""));
                emitPinStateChanged(pinstt2);
            }
        }
        break;
    case PINFUNC_MOMENTARY:
        if(e->button()==Qt::LeftButton){
            setText(sttstr.value(pinstt2,""));
            emitPinStateChanged(pinstt2);
        }
		break;
    case PINFUNC_INPUT:
        return;
    case PINFUNC_DISABLED:
        return;
    case PINFUNC_LINK:
        return;
    }

    QPushButton::mousePressEvent(e);
}

void FunctionButton::mouseReleaseEvent(QMouseEvent *e)
{
    switch(func){
    case PINFUNC_MOMENTARY:
        setText(sttstr.value(pinstt1,""));
        emitPinStateChanged(pinstt1);
		break;
    case PINFUNC_INPUT:
        return;
    case PINFUNC_DISABLED:
        return;
    }

    QPushButton::mouseReleaseEvent(e);
}





