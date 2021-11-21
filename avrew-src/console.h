#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>

enum{
    CONSTXT_NORMAL,
    CONSTXT_SENDED,
    CONSTXT_RECEIVED,
    CONSTXT_APPMESSAGE,
    CONSTXT_ERROR,
    MAX_CONSTXT
};



class Console : public QTextEdit
{
    struct Block{
        QString text;
        int type;
    };

    QList<Block*> blocks;
    int maxblocks;
    int maxblockchars;

    Q_OBJECT
    QColor colors[MAX_CONSTXT];
    bool visible[MAX_CONSTXT];

public:
    explicit Console(QWidget *parent = 0);
    void setMaxBlocks(int max){maxblocks = max;}
    void append(int type, QString text, bool newblock=true);
    void setVisible(int type, bool visible);
    void assignColor(int type, QColor color);
	void clear();

signals:

public slots:

};

#endif // CONSOLE_H
