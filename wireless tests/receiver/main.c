#include "timer.h"
#include "adc.h"
#include "uart.h"
#include <pic.h>

char buff = 0;
char lastbuff = 0;
char lastlastbuff = 0;
char myID = 69;
char ID;
char blink = 0;
char IDend = 0;
char TooManyS = 0;
char i;

void init(void);

int main(void) {
	init();
	delayms(1000);

	while (1) {
		if (!RCIF) {
			while(!RCIF);		//wait for the recieved data flag
		}
		if(RCIF){
			lastbuff = buff;	//keep track of the last buffer
			buff = RCREG;		//get the new buffer from register
		} 

		if (buff == 83) {
			blink = 1;		//if buff is "S", command mode on
			i = 0;

			if (lastbuff == buff) {
				TooManyS = 1;		//check if S is put in twice
			}		
		}

		if (buff != 83) {
			TooManyS = 0;		//S is not in there twice
		}

		/*if ((i >= 1) && (!IDend)) {			//if more than 1 bytes, the ID begins
			if ((buff == 82) || (buff == 88)) {	//if R or compliment X, the end of the ID is reached
				IDend = 1;			//denotes the ending of the ID #
			}
			else {
				ID += buff - 48;	//un-ascii the ID #
			}
		}*/

		if ((buff == 88) /*&& (myID == ID)*/) {
			blink = 0;				//if buff is R, turn off command mode
			UART_sendch(buff);
		}

		if ((blink == 1) /*&& (!TooManyS) && (myID == ID)*/) {
			while(!RCIF){ 
				RC2 = !RC2;			//send the value in buff, if all is good
				delayms(700);
				}
			RC2 = 0;
		}

		i++;
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
	TRISC=0;

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