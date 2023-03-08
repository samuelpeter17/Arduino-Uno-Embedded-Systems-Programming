/* Template for C programs in Engs 28, Embedded Systems
 * 
 * Name: Samuel Peter	
 * Assignment: Lab 3
 *
 * Program name: pulse
 * Date created:1/23/23
 * Description:Flash 32 pulses on Led with frequency of 10Hz; pulse on time increasing before reset
 *
 * Dependencies: (what other source files are required)
 *n/a
 *
 * I/O pins:  (what ports are used, and what are they wired to?)
 * PIND6: Output, LED on breadboard
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
#define LED_TIME   100		/* milliseconds */
const uint8_t mask = (1 << PIND7);		//pinmask variable

/* CODE */
int main(void)
{
  	// Setup code (run once) goes here:
	// sei();					// Global interrupt enable (if using)
	DDRD |= (1 << DDD6);			// configure bit 6 as output
	DDRD &= ~(1 << DDD7);			// configure bit D7 as input
	PORTD |= (1 << PORTD7);			//enable the pullup resistor for D7
	uint8_t LED_ON_TIME = 0;
	uint8_t button_waspressed = 0;			//variable to overcome button bouncing
	
	
	// Main code (runs in an infinite loop) goes here:
	while(1) {
		//check if button was pressed
		if((mask & PIND) == 0){
			//check if it wasnt pressed before(not a bounce)
			if(button_waspressed == 0){
				LED_ON_TIME += 3;			//we increment by 3 since 3.125 is a float and the actual step size
				if(LED_ON_TIME > 96)		//32*3 = 96
					LED_ON_TIME = 0;
				button_waspressed = 1;
			}
		}else{
			button_waspressed = 0;		//reset button_waspressed
		}
		//turn on LED for the respective on time
		PORTD |= (1 << PORTD6);
		_delay_ms(LED_ON_TIME);
		//turn of LED for the rest of the pulse time
		PORTD &= ~(1 << PORTD6);
		_delay_ms(96-LED_ON_TIME);
	}

	return 0;		/* never reached */
}

/* Code for your user-defined functions go here, if not in their own source files. */
