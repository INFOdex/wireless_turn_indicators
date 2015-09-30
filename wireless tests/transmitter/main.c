#include "timer.h"
#include "adc.h"
#include "uart.h"
#include <pic.h>

char buff = 0;		//Buffer to be sent
char lastbuff = 0;	
char lastlastbuff = 0;	
char myID = 69;		//ID # of this module
char ID;			//ID that comes in from reciever on error
char IDend = 0;		//boolean to determine if end of ID
char TooManyS = 0;	//boolean to determine interference
char blink = 0;

void init(void);

int main(void) {
	init();
	delayms(1000);

	while (1) {
		while((PORTC && 0xF0) == 0);	//check PORTC for button push
		if (!blink && RC5) {		//RC5 (Left) is pushed & not blinking
			buff = 76;
			RC0 = 1;
			UART_sendch(buff);
			delayms(200);
		}
		
		if (!blink && RC4) {		//RC4 (Right) is pushed & not blinking
			buff = 82;
			RC0 = 1;
			UART_sendch(buff);
			delayms(200);
		}
		
		blink++;		//change state to BLINK
		
		while((PORTC && 0xF0) == 0);
			if (blink && RC5) {			//RC5 (Left) is pushed & blinking
				buff = 88;
				RC0 = 0;
				UART_sendch(buff);
				delayms(200);
			}

			if (blink && RC4) {			//RC4 (Right) is pushed & blinking
				buff = 88;
				RC0 = 0;
				UART_sendch(buff);
				delayms(200);
			}
		
		blink--;		//change state to NOBLINK
	}
	return 0;

}


void init(void) {

	//set internal oscillator to 500 kHz
	//and run from external clock defined by FOSC
	OSCCON|=0b00111000;

	//set pin RA0 to output
	TRISA=0;

	//set pin RB0 to output
	TRISB=0;

	//set pin RC0 to output
	TRISC=0xF0;

	//Initialize PORTS A,B and C
	PORTA=0;
	PORTB=0;
	PORTC=0;

	//configure the device to run as a digital input 
	ANSEL=0;
	ANSELH=0;

	//ADC_init();
	UART_init();

	
//	RB4=0;
//	RB6=0;
//	delayms(50);
//	RB4=1;
//	RB6=1;
//	delayms(50);
	//RB4=0;
	//RB6=0;
}