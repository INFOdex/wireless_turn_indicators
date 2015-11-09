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

void init(void);	//initialization functions

int main(void) {
	init();			
	delayms(1000);	//delay used to make sure that UART is set up
	
	char r_blink = 0;	//boolean to denote state of right indicator
	char l_blink = 0;	//boolean to denote state of left indicator
	char h_blink = 0;	//boolean to denote state of hazard mode

	while (!RCIF) {
		while((PORTC && 0xF0) == 0);	//check PORTC for button push, hang here if not pushed
			while(!l_blink && RC5) {		//RC5 (Left) is pushed & not blinking
				while(RC5);			//debounce
				buff = 76;			//L
				RC1 = 1;
				UART_sendch(buff);
				l_blink = 1;		//change state to BLINK
				delayms(200);
				
			}
		
			while(!r_blink && RC4) {		//RC4 (Right) is pushed & not blinking
				while(RC4);			//debounce
				buff = 82;			//R
				RC0 = 1;
				UART_sendch(buff);
				r_blink = 1;		//change state to BLINK
				delayms(200);
			}
		
		
		
			while(l_blink && RC5) {			//RC5 (Left) is pushed & blinking
				while(RC5);			//debounce
				buff = 88;			//X
				RC1 = 0;
				UART_sendch(buff);
				l_blink = 0;		//change state to NOblink
				delayms(200);
			}

			while(r_blink && RC4) {			//RC4 (Right) is pushed & blinking
				while(RC4);			//debounce
				buff = 88;			//X
				RC0 = 0;
				UART_sendch(buff);
				r_blink = 0;		//change state to NOblink
				delayms(200);
			}
			
			while((r_blink || l_blink) && RC6) {		//ResetAll or cancel is pushed
				while(RC6);			//debounce
				buff = 88;			//X
				RC0 = 0;			//turn off right dash indicator
				RC1 = 0;			//turn off left dash indicator
				UART_sendch(buff);
				l_blink = 0;		//reset to NOblink
				r_blink = 0;		//reset to NOblink
				h_blink = 0;
				delayms(200);
			}
			
			while(!h_blink && RC7) {			//RC7 (Hazard) is pushed
				while(RC7);			//debounce
				buff = 82;			//R
				UART_sendch(buff);
				RC0 = 1;			//turn on both dash indicators
				RC1 = 1;
				buff = 76;			//L
				UART_sendch(buff);
				h_blink = 1;		//change state to blink for both indicators
				r_blink = 1;
				l_blink = 1;
				delayms(200);
			}
			
			while(h_blink && RC7) {		//ResetAll or cancel is pushed
				while(RC7);			//debounce
				buff = 88;			//X
				RC0 = 0;			//turn off right dash indicator
				RC1 = 0;			//turn off left dash indicator
				UART_sendch(buff);
				h_blink = 0;		//reset to NOblink for both indicators
				r_blink = 0;
				l_blink = 0;
				delayms(200);
			}
		
	
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