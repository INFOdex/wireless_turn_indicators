#include "timer.h"
#include "adc.h"
#include "uart.h"
#include <pic.h>
#include <htc.h>
#define _LEGACY_HEADERS

__CONFIG (FOSC_XT & WDTE_OFF & PWRTE_ON & BOREN_OFF, LVP_OFF & CPD_OFF & WRT_OFF);

int count = 0;		//counter to keep track of time in interrupt
char buff = 0;		//buffer to be sent
volatile char r_blink = 0;	//boolean to denote state of right indicator
char l_blink = 0;	//boolean to denote state of left indicator
char h_blink = 0;	//boolean to denote state of hazard mode

/*------LEGEND------*/
//X is for all cancel
//Y is for left cancel
//Z is for right cancel

//R is for right blink
//L is for left blink
//H is for hazard mode
/*------------------*/


void init(void);	//initialization function prototype

void interrupt ISR(void){	//interrupt routine for dash indicator blinking
	r_blink = PORTA;
	RABIF = 0;
	RC0 = 0;
	delayms(1000);
	RC0 = 1;
}

int main(void) {
	RABIF = 0;
	init();			
	delayms(1000);	//delay used to make sure that UART is set up
	RABIF = 0;

	//char r_blink = 0;	//boolean to denote state of right indicator
	//char l_blink = 0;	//boolean to denote state of left indicator
	//char h_blink = 0;	//boolean to denote state of hazard mode

	while (1) {
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
				buff = 89;			//Y
				RC1 = 0;
				UART_sendch(buff);
				l_blink = 0;		//change state to NOblink
				delayms(200);
			}

			while(r_blink && RC4) {			//RC4 (Right) is pushed & blinking
				while(RC4);			//debounce
				buff = 90;			//Z
				RC0 = 0;
				UART_sendch(buff);
				r_blink = 0;		//change state to NOblink
				delayms(200);
			}
			
			while((r_blink || l_blink) && RC6) {	//ResetAll or cancel is pushed
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
				buff = 72;			//H
				UART_sendch(buff);
				RC0 = 1;			//turn on both dash indicators
				RC1 = 1;
				h_blink = 1;		//change state to blink for both indicators
				r_blink = 1;
				l_blink = 1;
				delayms(200);
			}
			
			while(h_blink && RC7) {		//RC7 (Hazard) is pushed & hazard mode is enabled
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

	//set internal oscillator to 4 MHz (default)
	OSCCON|=0b00111000;

	//set pin RA0 to output
	TRISA=0x3F;

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
	
	//disable comparators
	//CM1CON0 = 0;        //Initialize comp 1 off 
    //CM2CON0 = 0;        //Initialize comp 2 off 

	//Configure timer0 for interrupts
	INTCON = 0x00;
	IOCA4 = 1;          //Enabled RA4 Interrupt-On-Change 
    IOCA5 = 1;          //Enabled RA5 Interrupt-On-Change 
    RABIF = 0;          //Enabled Interrupt-On-Change Interupt
    RABIE = 1;
	INTCON = 0b00001000;
	GIE = 1;		//enable Global interrupts
	RABIF = 0;
	
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