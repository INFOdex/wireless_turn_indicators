#include "timer.h"
#include "uart.h"
#include <pic.h>

__CONFIG (FOSC_XT & WDTE_OFF & PWRTE_ON, BOREN_OFF & LVP_OFF & CPD_OFF & WRT_OFF);

char buff = 0;			//buffer to be recieved
char lastbuff = 0;		//the previous buffer (error checking)
char blink = 0;			//binary state machine to determine command mode status
char TooManyS = 0;		//binary state machine to determine error in data recieved
char i;					//counter

void init(void);

int main(void) 
{	
	init();
	delayms(1000);		//used to insure the initializations

	while (1) {

		while(!RCIF) {
			while(!RCIF);		//wait for the recieved data flag
		}
		if(RCIF){
			lastbuff = FERR;	//check for framing errors
			lastbuff = buff;	//keep track of the last buffer
			buff = RCREG;		//get the new buffer from register
		} 

		if ((buff == 82) || (buff == 72)) {
			blink = 1;		//if buff is R or H, command mode on
			
			if (lastbuff == buff) {
				TooManyS = 1;		//check if R is put in twice
			}
			
			RC2 = 1;			//turn on light
			buff = 83;			//change buffer
			UART_sendch(buff);	//send confirmation "s"
			RC2 = 0;			//turn off light		
		}

		if (buff != 82) {
			TooManyS = 0;		//R is not in there twice
		}

		if ((buff == 88)) {
			blink = 0;				//if buff is X, turn off command mode
			RC2 = 0;
		}

		if ((blink == 1) && (!TooManyS)) {
			while(!RCIF){ 
				RC2 = !RC2;			//send the value in buff, if all is good
				delayms(700);
				}
		}	
		//i++;
	}
	return 0;

}


void init(void) {

	//set internal oscillator to 500 kHz
	//and run from external clock defined by FOSC
	//if no FOSC, 4MHz internal clock is default
	OSCCON|=0b00111000;

	//set pin RA0 to output
	TRISA=0;

	//set pin RB0 to output
	TRISB=0;

	//set pin RC0 to output
	TRISC=0;

	//Initialize PORTS A,B and C
	PORTA=0;
	PORTB=0;
	PORTC=0;

	//configure the device to run as a digital input 
	ANSEL=0;
	ANSELH=0;
	

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