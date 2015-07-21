#include "timer.h"
#include "adc.h"
#include "uart.h"
#include "pic.h"

int main(void) {
	//turn off unnecessary functionality
	__CONFIG(WDTE_OFF & PWRTE_OFF & MCLRE_OFF & BOREN_OFF);

	//set pin RA0 to output
	TRISA=0;

	//set pin RB0 to output
	TRISB=0;

	//set pin RC0 to input
	TRISC=1;

	//Initialize PORTS A,B and C
	PORTA=0;
	PORTB=0;
	PORTC=0;

	//configure the device to run as a digital input 
	ANSEL=0;
	ANSELH=0;
	
	//delayms(100);
	while(1){
		while(!RC7);
		while(RC7);
		
		RB5 = !RB5;
	}
}
