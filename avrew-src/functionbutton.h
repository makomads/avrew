#ifndef FUNCTIONBUTTON_H
#define FUNCTIONBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QHash>

namespace Ui {
class FunctionButton;
}

#define PINFUNC_LINK 99

class FunctionButton : public QPushButton
{
    Q_OBJECT
    //QPixmap pixon, pixoff, pixhiz;
    int pinnumber;
    int func;
    int pinstt1, pinstt2;
    int crtstat;
    QColor chigh, clow, chiz, creadhigh, creadlow, clinkhigh, clinklow;
    QHash<int,QString> sttstr;

public:
    explicit FunctionButton(int number, QWidget *parent = 0);
    ~FunctionButton();
    void setFunction(int func);
    int function(){return func;}
    int pinState(){return crtstat;}

    void assignPinState(int state1, int state2);
    void setInputState(int pinstate);

    void mousePressEvent ( QMouseEvent * e );
    void mouseReleaseEvent ( QMouseEvent * e );

signals:
    void pinStateChanged(int number, int pinstate);

private:
    void assignColor(int releasedstt, int pressedstt);
    void emitPinStateChanged(int pinstate);
};

#endif // FUNCTIONBUTTON_H

