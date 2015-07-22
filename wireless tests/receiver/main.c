#include "timer.h"
#include "adc.h"
#include "uart.h"
#include "pic.h"

int main(void) {
	
	//Variable initializations
	
	char buffer = 0x00;	//The 8-bit transmission buffer
	
	//Pin mode initializations and
	//turn off unnecessary functionality
	__CONFIG(WDTE_OFF & PWRTE_OFF & MCLRE_OFF & BOREN_OFF);

	//set pin RA0 to output
	TRISA=(TRISA&~0x3F);

	//set pin RB0 to output
	TRISB=(TRISB&~0xF0);

	//set pin RC0 to output
	TRISC=(TRISC&~0xFF);

	//Initialize PORTS A,B and C
	PORTA=0x00;
	PORTB=0x00;
	PORTC=0x00;

	//configure the device to run as a digital input 
	ANSEL=0;
	ANSELH=0;
	
	
	//Begin code here
	RC2 = 1;	//pull pin RC2 to VCC to power RF module
	UART_init();	//Use libraries to initialize UART
	RC7 = 0;

	TXSTA = 0b00100000;
    RCSTA = 0b10010000;
    BAUDCTL = 0b0000000;

    SPBRG = 12;
   	SPBRGH = 0;

    OSCCON = 0b01111001;
	INTCON = 0b11000000;
    PIE1 = 0b00100000;
    PIE2 = 0;

    PIR1 = 0;
	
	while(1){
		while(!RCIF);	//wait for interrupt flag to be set
		RC7=1;
		buffer = RCREG;	//get the tranmission buffer from register
		
		if(buffer==0xFF){	//check to see if LED should be on
			RC7 = 1;
		}
		if(buffer==0x00){	//check to see if LED should be on
			RC7 = 0;
		}
	}
}
