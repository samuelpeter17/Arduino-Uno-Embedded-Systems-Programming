/* Template for C programs in Engs 28, Embedded Systems
 * 
 * Name: Samuel Peter	
 * Assignment: Lab 2
 *
 * Program name:blinkySIM
 * Date created:1/7/23
 * Description:Flash 3 LEDs can count from 0 to 7, pause if button was pressed
 *
 * Dependencies: (what other source files are required)
 *n/a
 *
 * I/O pins:  (what ports are used, and what are they wired to?)
 * PB0(Pin 0): Output, LED on breadboard
 * PB1(Pin 1): Output, LED on breadboard
 * PB2(Pin 2): Output, LED on breadboard
 * PIND7: Input, Switch on breadboard
 *
 * Revisions: (use this to track the evolution of your program)
 *based on original blinky.c
 *
 */

/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>

// If using UART
// #include "USARTE28.h"   		// UART initializations
// #include "ioE28.h"      		// tiny_printf as well as some reading

// If using interrupts
// #include <avr/interrupt.h>	


/* FUNCTIONS */
/* Declare user-defined functions here (unless you have put the declarations in
 * their own #include file) */ 

/* CONSTANTS */
#define LED_ON_TIME   300		/* milliseconds */
#define LED_OFF_TIME  300     /* milliseconds*/
const uint8_t mask = (1 << PIND7);		//pinmask variable

/* CODE */
int main(void)
{
  	// Setup code (run once) goes here:
	// sei();					// Global interrupt enable (if using)
	DDRB |= (1 << DDB0);			// configure bit 0 as output
	DDRB |= (1 << DDB1);			// configure bit 1 as output
	DDRB |= (1 << DDB2);			// configure bit 2 as output
	DDRB |= (1 << DDB5);			// configure bit 5 as output
	DDRD &= ~(1 << DDD7);			// configure bit D7 as input
	PORTD |= (1 << PORTD7);			//enable the pullup resistor for D7
	uint8_t count = 0;				//variable to store the number being displayed
	typedef enum {IDLE, CNT} state_t;		//possible states
	state_t state = CNT;					//starting state
	uint8_t button_waspressed = 0;			//variable to overcome button bouncing

	// Main code (runs in an infinite loop) goes here:
	while(1) {
		//check if button was pressed
		if((mask & PIND) == 0){
			//check if it wasnt pressed before(not a bounce)
			if(button_waspressed == 0){
				//change states
				if(state == CNT)
					state = IDLE;
				else
					state = CNT;
				button_waspressed =1;
			}
		}else{
			button_waspressed = 0;		//reset button_waspressed
		}
		switch(state){
		case CNT:
			//clear Port B0, B1, B2 and put in our number to be displayed
			PORTB &= ~((1 << PORTB0)|(1 << PORTB1)|(1 << PORTB2)); 
			PORTB |= count;
			_delay_ms(LED_ON_TIME);
			count ++;		//increment count
			break;
		case IDLE:
			//if idle, stay idle
			state = IDLE;
		}
	}

	return 0;		/* never reached */
}

/* Code for your user-defined functions go here, if not in their own source files. */
