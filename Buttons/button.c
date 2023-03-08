/*
 * button
 * Polls a pin and turns LED on or off in response.  Pin is pulled up internally,
 * grounded with a pushbutton.
 * EWH, Engs 28, Jan 6 2019
 */

//#include "Arduino.h"
#include <avr/io.h>			// All the port definitions are here
// #include <util/delay.h>

/*  FUNCTIONS */
/* No user defined functions */

/* CONSTANTS */
const uint8_t PIN_MASK = (1 << PIND7);

int main(void)
{
  	// Setup code (run once) goes here
  	// Pins default to input (DDR=0)
  	DDRB  |=  (1 << DDB5);			// configure bit 5 as output
  	DDRD  &= ~(1 << DDD7);			// configure bit 0 as input, explicitly
  	// Inputs default to floating (no internal pullup)
  	// Turn pullup on for the input pin (see Sec 18.2.6, "Unconnected Pins")
  	PORTD |= (1 << PORTD7);	

	// Main code (runs repeatedly) goes here
	while(1) {
	  // Read the pin and control the LED
	  if ( (PIND & PIN_MASK) == 0 ) 	// button input is low-true
	  	PORTB |=  (1 << PORTB5);		// set bit 5 to turn LED on
	  else
	  	PORTB &= ~(1 << PORTB5);		// clear bit 5 to turn LED off
	}
	return 0;		/* never reached */
}
