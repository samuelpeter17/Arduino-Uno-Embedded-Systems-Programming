/* Template for C programs in Engs 28, Embedded Systems
 * 
 * Name: Samuel Peter	
 * Assignment: Lab 1
 *
 * Program name:blinkySIM
 * Date created:1/7/23
 * Description:Flash 3 LEDs can count from 0 to 7
 *
 * Dependencies: (what other source files are required)
 *n/a
 *
 * I/O pins:  (what ports are used, and what are they wired to?)
 * PB0(Pin 0): Output, LED on breadboard
 * PB0(Pin 1): Output, LED on breadboard
 * PB0(Pin 2): Output, LED on breadboard
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
#define LED_ON_TIME   200		/* milliseconds */
#define LED_OFF_TIME  200     /* milliseconds*/

/* CODE */
int main(void)
{
  	// Setup code (run once) goes here:
	// sei();					// Global interrupt enable (if using)
	DDRB |= (1 << DDB0);			// configure bit 0 as output
	DDRB |= (1 << DDB1);			// configure bit 1 as output
	DDRB |= (1 << DDB2);			// configure bit 2 as output

	// Main code (runs in an infinite loop) goes here:
	while(1) {
		
		for(int i=0;i<8;i++){
			if(i==0){
				_delay_ms(LED_OFF_TIME);    //kill some time
			}
			if(i==1){
				//turn Port0 on
				PORTB |= (1 << PORTB0);		// set bit 0 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Port0 off
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
			}
			if(i==2){
				//turn Port1 on
				PORTB |= (1 << PORTB1);		// set bit 1 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Port1 off
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
			}
			if(i==3){
				//turn Ports 0 and 1 on
				PORTB |= (1 << PORTB0);		// set bit 0 to turn LED on
				PORTB |= (1 << PORTB1);		// set bit 1 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Ports 0 and 1 off
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
			}
			if(i==4){
				//turn Port2 on
				PORTB |= (1 << PORTB2);		// set bit 2 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Port2 off
				PORTB &= ~(1 << PORTB1);	// clear bit 2 to turn LED off
			}
			if(i==5){
				//turn Ports 0 and 2 on
				PORTB |= (1 << PORTB0);		// set bit 0 to turn LED on
				PORTB |= (1 << PORTB2);		// set bit 2 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Ports 0 and 1 off
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
				PORTB &= ~(1 << PORTB2);	// clear bit 2 to turn LED off
			}
			if(i==6){
				//turn Ports 1 and 2 on
				PORTB |= (1 << PORTB1);		// set bit 1 to turn LED on
				PORTB |= (1 << PORTB2);		// set bit 2 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Ports 0 and 1 off
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
				PORTB &= ~(1 << PORTB2);	// clear bit 2 to turn LED off
			}
			if(i==7){
				//turn all Ports on
				PORTB |= (1 << PORTB0);		// set bit 0 to turn LED on
				PORTB |= (1 << PORTB1);		// set bit 1 to turn LED on
				PORTB |= (1 << PORTB2);		// set bit 2 to turn LED on
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Ports 0 and 1 off
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
				PORTB &= ~(1 << PORTB2);	// clear bit 2 to turn LED off
			}
		}			
	}

	return 0;		/* never reached */
}

/* Code for your user-defined functions go here, if not in their own source files. */
