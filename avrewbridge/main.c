#define	F_CPU 20000000UL

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define VERSION 2
#define _DEBUG 0

//ボーレートを決めるための値
//(CPUクロック/ビットレート)/(通常速16,倍速8) - 1
#define UBRR_VAL 10	


//ビット操作のマクロ定義
#define	sbi(PORT,BIT) PORT |= _BV(BIT)	
#define	cbi(PORT,BIT) PORT &=~_BV(BIT)



//グローバル変数
//割り込みルーチンでも使えるようにするためvolatileを付け最適化を無効化する

//ソフトUART受信バッファ
volatile uint8_t suarxbuf;
volatile int8_t suarxbit;
//ソフトUART送信バッファ
volatile uint8_t suatxbuf;
volatile int8_t suatxbit;

/*
	ソフトUARTのtx/rxビットの意味
	-1	アイドル
	0	スタートビット
	1-8	データビット
	9	ストップビット
	10	送出完了
	-1	アイドル
*/


//ハードUART送信バッファ
#define TXBUFSIZE 16  //必ず2の累乗にする
#define TXBUFMAXMASK (TXBUFSIZE-1)
volatile uint8_t txbuf[TXBUFSIZE];
volatile uint8_t txinpos;
volatile uint8_t txoutpos;
volatile uint8_t spidelay;



//外部割り込み1(INT1)
//ソフトUART受信スタートビット検出
ISR(INT1_vect)
{
	suarxbuf = 0;
	suarxbit = 0;
	
	//ピン変化割り込み不許可
	cbi(GIMSK, INT1);

	//タイマ開始
	TCNT0 = 15;	//パルスの真ん中で採取するためのウエイト
	TCCR0B |= 0b00000011;
}


//タイマー0比較A一致
//ソフトUART受信データビット検出
ISR(TIMER0_COMPA_vect)
{	
	uint8_t f_bitset;
	
	//ピンのhigh, low取得
	f_bitset = bit_is_set(PIND,3);
	
	//スタートビットを再確認し不正なスタートビットを検出
	if(suarxbit == 0 && f_bitset){
		//タイマ停止
		TCCR0B &= 0b11111000;
		//ピン変化割り込み許可
		sbi(EIFR, INTF1);	//割り込みフラグ強制解除
		sbi(GIMSK, INT1);
		suarxbit = -1;
		return;
	}
	//ストップビット
	else if(suarxbit == 9){
		//タイマ停止
		TCCR0B &= 0b11111000;
		//ピン変化割り込み許可
		sbi(EIFR, INTF1);	//割り込みフラグ強制解除
		sbi(GIMSK, INT1);
		
		//不正なストップビット検出
		if(!f_bitset){
			suarxbit = -1;	//こうするとメインループでホストに送信しない
			return;
		}
	}
	//データビット
	else{
		suarxbuf >>= 1;
		if(f_bitset){
			suarxbuf |= 0b10000000;
		}
	}
	
	//次のビットへ
	suarxbit++;
}


//タイマー1比較A一致
//ソフトUART送信
ISR(TIMER1_COMPA_vect)
{
	//ストップビット
	if(suatxbit==9){
		sbi(PORTD,4);
	}
	//送出完了
	else if(suatxbit==10){
		//タイマ停止する
		TCCR1B &= 0b11111000;
		suatxbit = -1;
		return;
	}
	//データビット
	else{
		if(suatxbuf & 1)
			sbi(PORTD,4);
		else
			cbi(PORTD,4);
		suatxbuf >>= 1;			
	}
	
	//次のビットへ
	suatxbit++;
}


//4バイトSPIで交換する
//USIは使わず任意のピンを使う
void spi_exchange(uint8_t *poutdata, uint8_t *pindata)
{
	uint8_t		bytecount;
	uint8_t		bitcount;
	uint8_t		outdata;
	uint8_t		indata;
	uint8_t		delaycnt;

	bytecount = 4;
	while(bytecount){
		outdata = poutdata[4-bytecount];
		indata = 0;
		bitcount = 8;
		while(bitcount){
			//送信データ(MSB先行)
			if(outdata&0x80)
				sbi(PORTD,4);
			else
				cbi(PORTD,4);
			outdata <<= 1;
			//クロック上昇
			sbi(PORTB, 4);
			delaycnt = spidelay;
			while(delaycnt) delaycnt--;//カウント1あたり3クロック=0.15us
			//受信データ
			indata <<= 1;
			if(PIND & 0b00001000)
				indata |= 1;
			//クロック下降
			bitcount--;
			cbi(PORTB, 4);
			delaycnt = spidelay;
			while(delaycnt) delaycnt--;	//カウント1あたり3クロック=0.15us
		}
		pindata[4-bytecount] = indata;
		bytecount--;
	}

}


#if _DEBUG
//デバッグ用テストデータ
//AVRStudioでオフラインデバッグする際に使う
/*
const uint8_t debugdata[] = {
	0xFF,0xCE,0x00,0x02,  		//ブロック長設定
	0xFF,0xC0,0x00,0x00,		//FLASHページ設定
	0x01,0x02,					//ブロックデータ
	0x4C,0x00,0x00,0x00			//ページ書き込み
};*/

/*
const uint8_t debugdata[] = {
	0xAC, 0x53, 0x00, 0x00,
	0xAC, 0x53, 0x00, 0x00,
	0x30, 0x00, 0x00, 0x00,
	0x30, 0x00, 0x01, 0x00
};*/

const uint8_t debugdata[] = {
	0xFF, 0xF1, 0, 0x55,
	0xFF, 0x06, 1, 0
};
uint8_t debugpos=0;
#define DBGUDR (debugdata[debugpos++])
#endif



int main(void) 
{

	uint8_t rxbuf[4];
#define spibuf rxbuf	//union
	uint8_t rxpos;
	uint8_t cmdresp[4];
	//uint8_t uarterr;
	uint16_t cnttimeout;
#define MAX_TIMEOUT 65535

	//ブロック転送関連
	uint8_t blkmodetype;
	uint16_t nblks;
	uint16_t cntblk;
	uint8_t blkreadpos;
	uint8_t blkwritepos;
	uint16_t addr;			//アドレス
	
	#define BLKSIZE 32
	volatile uint8_t blkbuf[BLKSIZE];
	
	//ページ関係
	uint16_t pagesize;		//1ページのサイズ
	uint16_t pagepos;		//ページ内の走査位置


reset:
	//マスター割り込み禁止
	cli();

/*
       ┏━━━┓
~RESET ┃A2  Vc┃Vcc
   RxD ┃D0  B7┃USCK
   TxD ┃D1  B6┃MISO
   XTL ┃A1  B5┃MOSI
   XTL ┃A0  B4┃TgtSCK
     - ┃D2  B3┃B3
TgtMISO┃D3  B2┃B2
TgtMOSI┃D4  B1┃B1
TgtRest┃D5  B0┃B0
   GND ┃GN  D6┃SpeedSW
       ┗━━━┛
*/

	//IO方向セット(0入力,1出力)
	DDRB  = 0b00010000;
	DDRD  = 0b00110010;

	//ポート初期化
	//出力の場合1でHigh、0でLow
	//入力の場合1を書くとプルアップ抵抗活性化、0を書くとハイインピーダンス
	PORTB = 0b00000000;
	PORTD = 0b01010001;
	
	//////////////////////////////////////////////////////////////////////
	//タイマ設定
	
	//TCCR0
	//B3,A1,A0	動作種別、CTC動作は010
	//B210	プリスケーラ(000:停止, 001:1, 010:8, 011:64, 100:256, 101:1024, 110,111:予約)
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000000;

	//タイマ0比較値(AVRのクロック/UART速度/プリスケーラ)
	OCR0A = 31;
	TCNT0 = 0;
	
	//TCCR1
	//B4,B3,A1,A0 動作種別 	CTCは0100(比較A一致)か1100(捕獲一致)
	//B210	プリスケーラ 設定値はタイマ0と同じ
	TCCR1A = 0;
	TCCR1B = 0b00001000;
	OCR1A  = 32;
	TCNT1  = 0;

	//タイマ割り込みはここで許可するが、タイマーが動いていないので割り込みはまだ発生しない
	TIMSK = 1<<OCIE0A | 1<<OCIE1A;

	//////////////////////////////////////////////////////////////////////
	//ピン割り込み

	//MCU制御
	//7		1を書くとプルアップ禁止
	//6,5,4	休止種別、休止許可
	//3,2	INT1割り込み条件(00:Low,01:両端,10:下降,11:上昇)
	//1,0	INT0割り込み条件
	MCUCR = 0b00001000;
	//ピン割り込みは実行モードになったら許可するのでここではまだ不許可
	GIMSK = 0;
	//GIMSK = 1<<INT1; //[debug]

	//////////////////////////////////////////////////////////////////////
	//USART設定

	//初期化時はボーレート0にする必要がある
	UBRRH = 0;
	UBRRL = 0;

	//データレジスタ
	UDR = 0;
	//7 受信完了フラグ
	//6 送信完了フラグ
	//5 送信レジスタ空きフラグ
	//4	フレーミングエラーフラグ
	//3	データオーバーランフラグ
	//2	パリティエラーフラグ
	//1	倍速許可
	//0	マルチプロセッサ許可
	UCSRA = 0b00000000;
	//7 受信完了割り込み許可
	//6 送信完了割り込み許可
	//5 送信レジスタ空き割り込み許可
	//4	受信(RXD0pin)許可
	//3	送信(TXD0pin)許可
	//2	データビット長選択2
	//1	受信追加データビット(9ビットフレームの時使用)
	//0	送信追加データビット(9ビットフレームの時使用)
	UCSRB = 0b00011000;
	//76 動作モード選択(00:非同期(調歩), 01:同期, 11:SPI)
	//54 パリティ選択(00:パリティなし, 10:偶数, 11:奇数)
	//3	 ストップビット(0:1bit, 1:2bit)
	//21 データビット長選択10(8bitは011)
	//0	 クロック極性(0:送信立ち上がり,受信立下り, 1:その逆、同期動作時のみ有効)
	UCSRC = 0b00000110;

	//ボーレート
	UBRRH = 0;
	UBRRL = UBRR_VAL;

	//////////////////////////////////////////////////////////////////////
	
	//諸変数初期化
	txinpos = 0;
	txoutpos = 0;
	rxpos = 0;
	rxbuf[0] = 0x55;
	rxbuf[1] = 0x55;
	cmdresp[0] = 0xFF;
	cmdresp[1] = 0;		//実行時はコマンド番号が入る
	cmdresp[2] = 0;
	cmdresp[3] = 0;
	suarxbit = -1;
	suatxbit = -1;
	blkmodetype = 0;
	spidelay = 2;
	

	//マスター割り込み許可
	sei();
	
#if _DEBUG	
	//testdebug
	spi_exchange(rxbuf, cmdresp);
	
	
#endif
	//sbi(GIMSK, INT1);


	//メインループ
	while(1){
		///////////////////////////////////////////////////////////////////////
		//UART送信
		
		//ブリッジからホストへ送信
		if(txinpos != txoutpos){
			//UART送信レジスタ空き待ちフラグ(UDRE)
			if(bit_is_set(UCSRA,UDRE)){
				//送信データセット
				//同時に送信が開始され、UDREは解除される
				UDR = txbuf[txoutpos++];
				txoutpos &= TXBUFMAXMASK; //if(txoutpos==TXBUFSIZE) txoutpos = 0;と同じ効果
			}
		}

		//ターゲットから受信したソフトUARTのデータがあればホストへ送出するバッファに入れる
		if(suarxbit==10){
			suarxbit = -1;
			txbuf[txinpos] = 0xFF;
			txbuf[txinpos+1] = 0xF2;
			txbuf[txinpos+2] = suarxbuf;
			txbuf[txinpos+3] = 0;
			txinpos += 4;
			txinpos &= TXBUFMAXMASK; //if(txinpos==TXBUFSIZE) txinpos = 0;と同じ効果
		}


		///////////////////////////////////////////////////////////////////////
		//UART受信

		//ブロック転送のときは4バイト固定ではなく指定したブロック長になる
		if(blkmodetype!=0){
			switch(blkmodetype){
			case 0xC0:
			case 0xC2:
				//連続書き込み
				if(UCSRA & 0b10000000){	//UART受信データがあるか
					blkbuf[blkwritepos] = UDR;
					blkwritepos++;
					cnttimeout = MAX_TIMEOUT;
				}
				if(blkreadpos < blkwritepos){
					//ページ設定
					if(blkmodetype==0xC0)	//FLASHページ設定
						spibuf[0] = (blkreadpos&1)==0? 0x40: 0x48;
					else if(blkmodetype==0xC2)	//EEPROMページ設定
						spibuf[0] = 0xC1;
					spibuf[1] = pagepos>>8;
					spibuf[2] = pagepos&0xFF;
					spibuf[3] = blkbuf[blkreadpos]; //DBGUDR; //UDR;
					//SPI
					spi_exchange(spibuf, cmdresp);
				
					//ページ内位置を進める
					if( (blkmodetype==0xC0 && (blkreadpos&1)==1) || blkmodetype==0xC2){
						pagepos++;
					}
					//ページ端なら書き込み
					if(pagepos == pagesize){
						pagepos = 0;
						if(blkmodetype==0xC0)
							spibuf[0] = 0x4C;	//Flash書き込み
						else if(blkmodetype==0xC2)
							spibuf[0] = 0xC2;	//EEPROM書き込み
						spibuf[1] = addr>>8;
						spibuf[2] = addr&0xFF;
						spibuf[3] = 0;
						spi_exchange(spibuf, cmdresp);
						addr += pagesize;
						_delay_ms(5);
					}
					
					//バッファ読み込み位置を進めて、バッファ端ならホストへ通知
					blkreadpos++;
					if(blkreadpos==BLKSIZE){
						blkreadpos=0;
						blkwritepos=0;
						txbuf[txinpos++] = 0xFF;
						txbuf[txinpos++] = blkmodetype;
						txbuf[txinpos++] = (addr>>8)&0xFF;
						txbuf[txinpos++] = addr&0xFF;
						txinpos &= TXBUFMAXMASK;
						
						//ブロックカウンタを進め、ブロック終了判定
						//終了判定はページではなくブロックで行っている
						//現時点(ver2)でAVRのROMサイズはいずれもブロックサイズ(32)の倍数なので問題ないと思われる
						cntblk++;
						if(cntblk == nblks){
							blkmodetype = 0;
						}
					}
				}
				break;
			case 0xC1:
			case 0xC3:
				//flash/eeprom連続読み込み
				if(txinpos == txoutpos){ //コマンドレスポンスを優先する(txbufが空になってから読み込み開始)
					do{
						if(blkmodetype==0xC1)
							spibuf[0] = (blkreadpos&1)==0? 0x20: 0x28;
						else if(blkmodetype==0xC3)
							spibuf[0] = 0xA0;
						spibuf[1] = addr>>8;	//アドレス上位
						spibuf[2] = addr&0xFF;	//アドレス下位
						spibuf[3] = 0;
						
						//SPI
						spi_exchange(spibuf, cmdresp);
						//アドレスを進める
						if( (blkmodetype==0xC1 && (blkreadpos&1)==1) || blkmodetype==0xC3){
							addr++;
						}
						
						//読み込みはブロック単位ではないがカウンタとして使う
						blkreadpos++;
						if(blkreadpos==BLKSIZE){
							cntblk++;
							blkreadpos = 0;
						}
						
						//ホストへ戻し
						while(bit_is_clear(UCSRA,UDRE));
						UDR = cmdresp[3];
						
					}while(cntblk != nblks);
					blkmodetype = 0;
				}
				break;
			}
		}
		//コマンド転送では4バイト単位で処理する
		else{	//blkmodetype==0
			//UART受信完了待ち	
#if _DEBUG
			rxbuf[rxpos++] = DBGUDR;
#else
			//UART受信あり
			if(bit_is_set(UCSRA,RXC)){
				//受信バッファ異常フラグ
				//現時点では特に処理しない
				//uarterr |= (UCSRA & 0b00011000);	//bit4=フレーム異常, bit3=オーバーラン
				
				//UDRを読むと自動的に受信完了フラグは消える
				rxbuf[rxpos++] = UDR;
				cnttimeout = MAX_TIMEOUT;
			}
#endif
			//4バイト受信したら処理する
			if(rxpos==4){
				rxpos=0;
				if(rxbuf[0]!=0xFF && GIMSK==0){
					//SPIでターゲットと4バイト交換する
					spi_exchange(rxbuf, cmdresp);
					cmdresp[0] = 0x6F;
				}
				//ブリッジコマンド
				else{
					cmdresp[0] = 0xFF;
					cmdresp[1] = rxbuf[1];
					cmdresp[2] = 0;
					cmdresp[3] = 0;

					//コマンド振り分け
					switch(rxbuf[1]){
					case 0:	//null command
						cmdresp[0] = 0xFE; //0xFEは返信なしの意味
						break;
					case 6:
						//ターゲットリセットピン切り替え
						if(rxbuf[2]==0){
							//プログラミングモードに切り替え
							//外部割り込み不許可でソフトUART使用不可に
							cbi(GIMSK, INT1);
							//ソフトSPI MOSI Low
							cbi(PORTD, 4);
							//RESETをlow
							cbi(PORTD, 5);
						}
						else if(rxbuf[2]==1){
							//実行モードに切り替え
							//ソフトUART Tx High(アイドル)
							sbi(PORTD, 4);
							//RESETをhiにして実行モードへ
							sbi(PORTD, 5);
							//外部割り込み許可でソフトウェアUARTに
							sbi(EIFR, INTF1);	//1を書き込むと割り込みフラグ強制解除
							sbi(GIMSK, INT1);
						}
						else if(rxbuf[2]==2){
							//RESETをlow
							cbi(PORTD, 5);
						}
						else if(rxbuf[2]==3){
							//RESETをhigh
							sbi(PORTD, 5);					
						}
						break;
					case 10: //0x0A
						//ポートB下位ニブル変更
						PORTB = (PORTB & 0xF0) | (rxbuf[2] & 0x0F);
						break;
					case 11: //0x0B
						//ポートB下位ニブル読み取り
						cmdresp[2] = PORTB & 0x0F;
						break;
					case 12: //0x0C
						//ポートB下位ニブル方向変更
						DDRB = (DDRB & 0xF0) | (rxbuf[2] & 0x0F);
						break;
					case 13: //0x0D
						//ポートB下位ニブル方向読み取り
						cmdresp[2] = DDRB & 0x0F;
						break;
					case 14: //0x0E
						//ポートB下位ニブル読み取り
						cmdresp[2] = PINB & 0x0F;
						break;
					//このブリッジのバージョン
					case 20: //0x14
						cmdresp[2] = VERSION;
						break;
					case 21:
						//SPIディレイ設定
						spidelay = rxbuf[2];
						break;
					case 0xC0:
					case 0xC1:
					case 0xC2:
					case 0xC3:
						//ブロック転送モード開始
						blkmodetype = rxbuf[1];
						if(rxbuf[1] == 0xC0)
							pagesize /= 2; //flash書き込みはword単位
						nblks = rxbuf[2];
						nblks <<= 8;
						nblks += rxbuf[3];
						
						addr = 0;
						blkwritepos = 0;
						blkreadpos = 0;
						cntblk = 0;
						pagepos = 0;
						break;
					case 0xCE:
						//ページサイズ設定
						//rxbuf[2]に上位8bit、[3]に下位8bit
						pagesize = rxbuf[2];
						pagesize <<= 8;
						pagesize += rxbuf[3];
						break;
					case 0xF1:
						//ホストからターゲットへソフトUARTでデータ送信
						if(suatxbit != -1){
							//すでに送信中
							cmdresp[3] = 0xFF;
						}
						else{
							suatxbuf = rxbuf[2];
							//タイマを開始
							TCNT1 = 0;
							TCCR1B |= 0b00000011;
							//スタートビット送出
							cbi(PORTD,4);
							suatxbit = 1;
						}
						break;
					case 0xFF:
						//padding command
						//no response
						cmdresp[0] = 0xFE; //0xFEは返信なしの意味
						break;
					}
				} //if(rxbuf[0]==0xFF)
				
				//レスポンス
				//NULLコマンドとパディングコマンドを除く
				//実際の送信はメインループ内
				if(cmdresp[0] != 0xFE){
					txbuf[txinpos++] = cmdresp[0];
					txbuf[txinpos++] = cmdresp[1];
					txbuf[txinpos++] = cmdresp[2];
					txbuf[txinpos++] = cmdresp[3];
					txinpos &= TXBUFMAXMASK;
				}
			} //if(rxpos==4)
		} // if(!blockmode)
		
		//4バイト受信できないときは受信バッファをリセットする
		//ウォッチドッグタイマの代わり
		cnttimeout--;
		if(cnttimeout==0){
			goto reset;
		}
		
	}
}


