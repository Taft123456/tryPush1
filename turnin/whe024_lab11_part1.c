/* Author: whe024
 * Partner(s) Name: Wentao He
 * Lab Section: A21
 * Assignment: Lab #11  Exercise #1
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <avr/io.h>
#include <bit.h>
#include <timer.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char GetKeypadKey() {
    PORTC = 0xEF; 
    asm("nop"); 
    if (GetBit(PINC,0)==0) 
    { return('1'); }
    if (GetBit(PINC,1)==0) 
    { return('4'); }
    if (GetBit(PINC,2)==0) 
    { return('7'); }
    if (GetBit(PINC,3)==0) 
    { return('*'); }

    PORTC = 0xDF; 
    asm("nop"); 
    if (GetBit(PINC,0)==0) 
    { return('2'); }
    if (GetBit(PINC,1)==0) 
    { return('5'); }
    if (GetBit(PINC,2)==0) 
    { return('8'); }
    if (GetBit(PINC,3)==0) 
    { return('0'); }

    PORTC = 0xBF; 
    asm("nop"); 
    if (GetBit(PINC,0)==0) 
    { return('3'); }
    if (GetBit(PINC,1)==0) 
    { return('6'); }
    if (GetBit(PINC,2)==0) 
    { return('9'); }
    if (GetBit(PINC,3)==0) 
    { return('#'); }

    PORTC = 0x7F; 
    asm("nop");
    if (GetBit(PINC,0)==0) 
    { return('A'); }
    if (GetBit(PINC,1)==0) 
    { return('B'); }
    if (GetBit(PINC,2)==0) 
    { return('C'); }
    if (GetBit(PINC,3)==0) 
    { return('D'); }

    return('\0');
}

enum pauseButtonSM_States {pauseButton_wait, pauseButton_press, pauseButton_release};

int pauseButtonSMTick(int state){
	unsigned char press = GetKeypadKey();
	switch(state){
		case pauseButton_wait:
			state = (press == '\0') ? pauseButton_wait: pauseButton_press;
			break;
		case pauseButton_press:
			state = pauseButton_release;
			break;
		case pauseButton_release:
			state = (press == '\0') ? pauseButton_wait: pauseButton_release;
			break;
		default:
			state = pausebutton_wait;
			break;
	}
	switch(state){
		case pauseButton_wait:
			break;
		case pauseButton_press:
			switch (press) {
                		case '\0': 
					PORTB = 0x1F; 
					break; 
                		case '1':
					PORTB = 0x01; 
					break; 
                		case '2': 
					PORTB = 0x02; 
					break;
                		case '3': 
					PORTB = 0x03; 
					break;
                		case '4': 
					PORTB = 0x04; 
					break;
                		case '5': 
					PORTB = 0x05; 
					break;
                		case '6': 
					PORTB = 0x06; 
					break;
                		case '7': 
					PORTB = 0x07; 
					break;
                		case '8': 
					PORTB = 0x08; 
					break;
                		case '9': 
					PORTB = 0x09; 
					break;
                		case 'A': 
					PORTB = 0x0A; 
					break;
                		case 'B': 
					PORTB = 0x0B; 
					break;
                		case 'C': 
					PORTB = 0x0C; 
					break;
                		case 'D': 
					PORTB = 0x0D; 
					break;
                		case '*': 
					PORTB = 0x0E; 
					break;
                		case '0': 
					PORTB = 0x00; 
					break;
                		case '#': 
					PORTB = 0x0F; 
					break;
                		default: 
					PORTB = 0x1B; 
					braak;
       			}
			break;
		case pauseButton_release:
			break;
		default:
			break;
	}
	return state;
}

typedef struct _task{
	signed char state; 		
	unsigned long period; 		
	unsigned long elapsedTime; 	
	int (*TickFct)(int); 		
} task;

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; 
    DDRC = 0xF0;
    PORTB = 0x00;
    PORTC = 0x0F;
    /* Insert your solution below */
    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
    task1.state = pauseButton_wait;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &pauseButtonSMTick;
    
    unsigned long int GCD = 1;
    TimerSet(1);
    TimerOn();
	
    unsigned short i;
    while (1) {
       for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period )
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
	       		GCD = findGCD(GCD, tasks[i]->period);
       }
       
       while (!TimerFlag);
       TimerFlag = 0;
    }
    return 1;
}
