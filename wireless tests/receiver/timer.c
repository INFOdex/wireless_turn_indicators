#include <pic.h>

void delayms(unsigned int ms) {
	unsigned int i;
	while(ms) {
		for(i=0;i<8;i++) {
			TMR0=45;
			T0IF=0;
			T0IE=1;
			T0CS=0;
			while(!T0IF);
			T0IF=0;
			T0IE=0;
		}
		ms--;
	}

}