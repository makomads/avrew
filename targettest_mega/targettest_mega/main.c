#define	F_CPU  18000000UL 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define	sbi(PORT,BIT) PORT |= _BV(BIT)	
#define	cbi(PORT,BIT) PORT &=~_BV(BIT)
#define false 0
#define true 1



#define SOFTUART_BAUDRATE 9600L
#define SOFTUART_TX_PORT PORTB
#define SOFTUART_TX_PINNO 4
#define SOFTUART_RX_PIN PINB
#define SOFTUART_RX_PINNO 3
#define SOFTUART_WAIT (F_CPU/SOFTUART_BAUDRATE/6-1)




void UARTSendByte(uint8_t data)
{
	uint8_t bitcnt;
	uint16_t framedata;

	//[debug]
	//data = 0x53;

	cli();

	//スタート、ストップビットを付加
	framedata = data;
	framedata <<= 1;
	framedata |= 0x0200;
	framedata &= 0xFFFE;
	//LSBから順に送出
	bitcnt = 10;
	do{
		if(framedata&1)
			sbi(SOFTUART_TX_PORT, SOFTUART_TX_PINNO);
		else
			cbi(SOFTUART_TX_PORT, SOFTUART_TX_PINNO);
		framedata >>= 1;
		_delay_us(34);
		_delay_us(34);
		_delay_us(35);
		bitcnt--;
	}while(bitcnt);

	sei();
}



volatile uint8_t sua_rxdata = 0;
volatile uint8_t sua_frxcomp = 0;	//ソフトUART受信完了フラグ

ISR(PCINT0_vect)
{
	uint8_t bitcnt;

	//UARTはアイドル時High、スタートビットは立下りを検出
	if( bit_is_set(SOFTUART_RX_PIN, SOFTUART_RX_PINNO) )
		return;

	cli();

	//スタートビットのパルス中央になるまで待つ
	_delay_us(26);
	_delay_us(26);
	sua_rxdata = 0;
	//LSBから順に採取
	bitcnt = 8;
	do{
		_delay_us(34);
		_delay_us(34);
		_delay_us(35);
		sua_rxdata >>= 1;
		if( bit_is_set(SOFTUART_RX_PIN, SOFTUART_RX_PINNO) )
			sua_rxdata |= 0x80;
		bitcnt--;
	}while(bitcnt);

	//ストップビットを読み飛ばす
	_delay_us(34);
	_delay_us(34);
	_delay_us(35);


	sua_frxcomp = true;

	//ピン変化割り込みフラグ解除
	PCIFR = 1<<PCIF0;
	//マスター割り込み許可
	sei();

}



int main(void) 
{

/*
       ┏━━━┓
~RESET ┃RS  C5┃
    RxD┃D0  C4┃
    TxD┃D1  C3┃out2
       ┃D2  C2┃in2
       ┃D3  C1┃out1
       ┃D4  C0┃in1
       ┃VC  GN┃  
       ┃GN  AR┃  
   XTL1┃B6  AV┃  
   XTL2┃B7  B5┃SCK
       ┃D5  B4┃MISO
       ┃D6  B3┃MOSI(PCINT3)
       ┃D7  B2┃    
       ┃B0  B1┃    
       ┗━━━┛
*/
	
	uint8_t uartbuf[20];
	uint8_t bufsize=20;
	uint8_t bufidx=0;
	uint8_t i;

	//マスター割り込み禁止
	cli();

	//IO方向セット(0入力,1出力)
	DDRB = 0b00010000;
	DDRC = 0b00001010;
	DDRD = 0b00000000;

	//ポート初期化
	//出力の場合1でHigh、0でLow
	//入力の場合1を書くとプルアップ抵抗活性化、0を書くとハイインピーダンス
	PORTB = ~DDRB;
	PORTC = ~DDRC;
	PORTD = ~DDRD;
	
	
	//////////////////////////////////////////////////////////////////////
	//ピン割り込み
	//ピン変化割り込みマスク設定
	PCMSK0 = 1<<PCINT3;
	//ピン割り込み許可
	PCICR = 1<<PCIE0;
	PCIFR = 1<<PCIF0;
	sbi(SOFTUART_TX_PORT, SOFTUART_TX_PINNO);

	//////////////////////////////////////////////////////////////////////
	
	//諸変数初期化
	sua_frxcomp = false;

	//[debug]
	UARTSendByte(0x55);

	//割り込み許可
	sei();

	//メインループ
	while(1){
		if(bit_is_set(PINB,0))
			sbi(PORTB,1);
		else
			cbi(PORTB,1);
		if(bit_is_set(PINB,2))
			sbi(PORTB,3);
		else
			cbi(PORTB,3);
			
		//ソフトUART受信完了待ち
		if(sua_frxcomp){
			sua_frxcomp = false;
			uartbuf[bufidx++] = sua_rxdata;
		}
		//返信
		if(sua_rxdata=='\n' || bufidx==bufsize){
			for(i=0; i<bufidx; i++){
				if(uartbuf[i]>=0x20)
					UARTSendByte(uartbuf[i]+1);
			}
			bufidx=0;
		}
			

	}

	return 0;
}

