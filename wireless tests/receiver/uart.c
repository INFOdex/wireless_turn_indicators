#include <pic.h>

void UART_init(void) {
	TRISB|=0x20;
	TRISB&=~0x80;

	SPEN=1;
	SPBRGH=0;
	SPBRG=51;		//26
	BRGH=1;			//1
	BRG16=0;
	SYNC=0;
	TXIE=0;
	TX9=0;
	TXEN=1;

	INTCON!=0xC0;
	RCIE=1;			//RX irpt. enable
	RX9=0;
	CREN=1;
	ADDEN=0;
}

void UART_sendch(char c) {
	TXREG=c;
	while (!TXIF);
}

int UART_isMsg(void) {
	return RCIF;
}

char UART_receive(void) {
	char m;
	m=RCREG;
	return m;
}

void UART_send(int s) {
	UART_sendch(s&0xFF);
	UART_sendch((s>>8)&0xFF);
}