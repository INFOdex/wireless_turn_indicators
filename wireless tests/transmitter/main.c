#include "uart.h"
#include <pic.h>
#include <htc.h>
#define _LEGACY_HEADERS

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)


volatile char count = 0;	//counter to keep track of time in interrupt
volatile int state = 0;     //binary, digital state machine for reset
volatile char buff = 0;		//buffer to be sent
volatile char workaround;   //used to send X (all cancel) via interrupt
volatile char r_blink = 0;	//boolean to denote state of right indicator
volatile char l_blink = 0;	//boolean to denote state of left indicator
volatile char h_blink = 0;	//boolean to denote state of hazard mode
volatile char error_chk = 0; //buffer with error checking result

/*------LEGEND------*/
//X is for all cancel

//R is for right blink
//L is for left blink
//H is for hazard mode
/*------------------*/


void init(void);	//initialization function prototype

void interrupt ISR(void){	//interrupt routine for dash indicator blinking
    for(int i = 0; i<10000; i++);
    if(RA5 && RABIF){
        count = PORTA;              //read from portA to clear interrupt
        RABIF = 0;                  //clear the flag
        if(l_blink || r_blink || h_blink){
            workaround = 1;                 //for use outside interrupt
        }
        l_blink = 0;        //left off
        r_blink = 0;        //right off
        h_blink = 0;        //hazard mode off
        PORTC = 0;
    }
}

int main(void) {
	RABIF = 0;         //turn off button interrupt flag for good measure
    init();			
	for(int i = 0; i<25000; i++);	//delay used to make sure that UART is set up
	RABIF = 0;          //turn it off again, because it is finicky

	while (1) {
		while((PORTC && 0xF0) == 0);	//check PORTC for button push, hang here if not pushed
		/*TURN LEFT*/	
            while(!l_blink && RC5) {		//RC5 (Left) is pushed & not blinking
				while(RC5);			//debounce
				buff = 76;			//L
				RC1 = 1;            //turn light on to show visual feedback
				UART_sendch(buff);  //send signal to XBee
				l_blink = 1;		//change state to BLINK
            /*----error checking begins below----*/
                for(int i = 0;!RCIF;i++){       //getting response from indicator
                    for(int i = 0; i<10; i++);  //this delay was calibrated via trial and error
                    if(i>10000){                //don't wait here forever (timeout)
                        RC2 = 1;                //ERROR!
                        break;
                    }
                }
                while(RCIF){
                    error_chk = RCREG;          //read the register message until empty
                }
                if(error_chk != 83){ //make sure not garbage
                    RC2 =1;                 //else: ERROR!
                    error_chk = 0;  //reset error-checking buffer after ERROR
                }
                error_chk = 0;      //reset error-checking buffer after success
            /*---error checking completed----*/
                while(l_blink){
                    RC1 = !RC1;     //toggle LED state
                    for(int i = 0; i<27000; i++);   //delay
                }
				
			}
		/*TURN RIGHT*/
			while(!r_blink && RC4) {		//RC4 (Right) is pushed & not blinking
				while(RC4);			//debounce
				buff = 82;			//R
				RC0 = 1;            //turn on LED for visual feedback
				UART_sendch(buff);  //send data to XBee
				r_blink = 1;		//change state to BLINK
                
                for(int i = 0;!RCIF;i++){       //getting response from indicator
                    for(int i = 0; i<10; i++);  //this delay was calibrated via trial and error
                    if(i>10000){                //don't wait here forever (timeout)
                        RC2 = 1;                //ERROR!
                        break;          
                    }
                }
                while(RCIF){
                    error_chk = RCREG;          //read the register message until empty
                }
                if(error_chk != 83){ //make sure not garbage
                    RC2 =1;                 //else: ERROR!
                    error_chk = 0;  //reset error-checking variable after ERROR!
                }
                error_chk = 0;      //reset error-checking variable after success
                
                while(r_blink){
                    RC0 = !RC0;     //toggle LED state
                    for(int i = 0; i<27000; i++);   //delay
                }
			}
		/*HAZARD MODE*/	
			while(!h_blink && RC7) {			//RC7 (Hazard) is pushed
				while(RC7);			//debounce
				buff = 72;			//H
				UART_sendch(buff);  //send data to XBee
				RC0 = 1;			//turn on both dash indicators
				RC1 = 0;            //asynchronously...
				h_blink = 1;		//change state to blink for both indicators
				r_blink = 1;
				l_blink = 1;
                while(h_blink){
                    RC0 = !RC0; //toggle both
                    RC1 = !RC1; //LED states
                    for(int i = 0; i<27000; i++);
                }
			}
        
        if(workaround){     //send all cancel if interrupt sets "workaround" to 1
            buff = 88;          //X
            UART_sendch(buff);  //send data to XBee
            workaround = 0;     //reset workaround        
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
	TRISB=0x00;

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

	//Configure Interrupt On Change for RA5
	INTCON = 0x00; 
    IOCA5 = 1;          //Enabled RA5 Interrupt-On-Change 
    RABIF = 0;          //Enabled Interrupt-On-Change Interupt
    RABIE = 1;
    RABIF = 0;
	//INTCON = 0b00001000;
	GIE = 1;		//enable Global interrupts
	
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