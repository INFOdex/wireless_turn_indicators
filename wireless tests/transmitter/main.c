#include "timer.h"
#include "adc.h"
#include "uart.h"
#include "pic.h"

int main(void) {
	
	//Variable initializations
	
	char buffer = 0xFF;	//The 8-bit transmission buffer
	int T = 0;	//the toggle state (boolean)
	
	//Pin mode initializations and
	//turn off unnecessary functionality
	__CONFIG(WDTE_OFF & PWRTE_OFF & MCLRE_OFF & BOREN_OFF);

	//set pin RA0 to output
	TRISA=0x00;

	//set pin RB5 to input and RB7 to output
	TRISB=(TRISB&~0xF0);	//clear top four bits
	TRISB=(TRISB|0x20);	//set them to 0010

	//set pin RC0 to output
	TRISC=0x00;

	//Initialize PORTS A,B and C
	PORTA=0x00;
	PORTB=0x00;
	PORTC=0x00;

	//configure the device to run as a digital input 
	ANSEL=0;
	ANSELH=0;
	
	/*TXSTA = 0b00100000;
    RCSTA = 0b10010000;
    BAUDCTL = 0b0000000;

    SPBRG = 12;
    SPBRGH = 0;

    OSCCON = 0b01111001;
	INTCON = 0b11000000;
    PIE1 = 0b00100000;
    PIE2 = 0;

    PIR1 = 0;*/
	
	
	//Begin code here
	UART_init();	//Use libraries to initialize UART
	RC7 = 1;	//pull pin RC7 to VCC to power RF module
	
	while(1){
		while(!RC1);	//Hang here
		while(RC1);		//Debounce the button
		
		//Decide whether to send "off" or "on"
		if(T==0){
			UART_sendch(buffer);
			buffer = 0x00;
			T = 1;
			RC2 = 1;
		}
		
		while(!RC1);
		while(RC1);
		
		if(T==1){
			UART_sendch(buffer);
			buffer = 0xFF;
			T = 0;
			RC2 = 0;
		}
	}
}
