#include "console.h"
#include <QStringBuilder>
#include <QTextBlock>

Console::Console(QWidget *parent) :
    QTextEdit(parent)
{
    int i;
    for(i=0; i<MAX_CONSTXT; i++){
        visible[i] = true;
        colors[i] = QColor(0,0,0);
    }

    maxblocks = 100;
    maxblockchars = 1000;

    //[test]
    colors[0] = QColor(128,128,128);
    colors[CONSTXT_SENDED] = QColor(128,0,0);
    colors[CONSTXT_RECEIVED] = QColor(0,192,0);
    colors[CONSTXT_APPMESSAGE] = QColor(0,0,192);
    colors[CONSTXT_ERROR] = QColor(255,0,0);
}

void Console::append(int type, QString text, bool newblock)
{
    QStringList lines = text.split("\n");
    QStringList::Iterator it;

    for(it=lines.begin(); it!=lines.end(); it++){
        if(document()->lastBlock().userState()==type
                && it==lines.begin()
                && blocks.last()->text.length()<maxblockchars
                && !newblock){
            //前回と同じタイプ、かつ最初の行、かつ行が最大数以下なら現在の行に追加する
            if(visible[type]){
                textCursor().insertText(*it);
            }
            blocks.last()->text.append(*it);

        }
        else{
            //前回と違うタイプか、改行があったとき、または行内の文字数が制限以上のとき、引数の新ブロック指定があったときは行を変える
            //QTextDocumntは新しいブロックを作成する
            if(visible[type]){
                setTextColor(colors[type]);
                QTextEdit::append(*it);
                document()->lastBlock().setUserState(type);
            }

            //表示状態変更に備え、このクラス自身でもブロックのデータを保持する
            Block* b = new Block();
            b->text = *it;
            b->type = type;
            blocks.append(b);
        }

        //ブロックが制限を越えたら古いものから消す
        if(blocks.size()>maxblocks){
            blocks.removeFirst();

            QTextCursor cur(document());
            cur.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
            cur.deleteChar();
        }
    }
}

void Console::setVisible(int type, bool v)
{
    QList<Block*>::Iterator it;

    if(visible[type] == v)
        return;
    visible[type] = v;

    //表示状態を変更する
	QTextEdit::clear();
    for(it=blocks.begin(); it!=blocks.end(); it++){
        if(!visible[(*it)->type])
            continue;
        setTextColor(colors[(*it)->type]);
        QTextEdit::append((*it)->text);
        document()->lastBlock().setUserState((*it)->type);
    }
}

void Console::assignColor(int type, QColor color)
{
    if(type>MAX_CONSTXT)
        return;
	colors[type] = color;
}

void Console::clear()
{
	QList<Block*>::Iterator it;

	for(it=blocks.begin(); it!=blocks.end(); it++){
		delete *it;
	}
	blocks.clear();

	QTextEdit::clear();
}


