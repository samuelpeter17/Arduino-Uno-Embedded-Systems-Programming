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

/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>


/* CONSTANTS */
const uint8_t PIN_MASK = (1 << PIND7);
/* CONSTANTS */
#define LED_ON_TIME   200		/* milliseconds */
#define LED_OFF_TIME  200        /* milliseconds*/

int main(void)
{
  	// Setup code (run once) goes here
  	// Pins default to input (DDR=0)
	DDRB |= (1 << DDB0);			// configure bit 0 as output
	DDRB |= (1 << DDB1);			// configure bit 1 as output
	DDRB |= (1 << DDB2);			// configure bit 2 as output
  	DDRD  &= ~(1 << DDD7);			// configure bit 0 as input, explicitly
  	// Inputs default to floating (no internal pullup)
  	// Turn pullup on for the input pin (see Sec 18.2.6, "Unconnected Pins")
  	PORTD |= (1 << PORTD7);	

	// Main code (runs repeatedly) goes here
	while(1) {
	  // Read the pin and control the LED
	  if ( (PIND & PIN_MASK) == 0 ) 	// button input is low-true
	  {
	  	PORTB |= (1 << PORTB0);		// set bit 0 to turn LED on
		PORTB |= (1 << PORTB1);		// set bit 1 to turn LED on
		PORTB |= (1 << PORTB2);		// set bit 2 to turn LED on
		_delay_ms(LED_ON_TIME);		// kill some time
	  }		
	  else{
	  	for(int i=0;i<3;i++){
			if(i==0){
				//turn Port0 on
				PORTB |= (1 << PORTB0);		// set bit 0 to turn LED on
				PORTB &= ~(1 << PORTB2);	// clear bit 2 to turn LED off
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Port0 off
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
			}
			if(i==1){
				//turn Port1 on
				PORTB |= (1 << PORTB1);		// set bit 1 to turn LED on
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
				PORTB &= ~(1 << PORTB2);	// clear bit 2 to turn LED off
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Port1 off
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
			}
			if(i==2){
				//turn Port0 on
				PORTB |= (1 << PORTB2);		// set bit 2 to turn LED on
				PORTB &= ~(1 << PORTB1);	// clear bit 1 to turn LED off
				PORTB &= ~(1 << PORTB0);	// clear bit 0 to turn LED off
				_delay_ms(LED_ON_TIME);		// kill some time
				//turn Port0 off
				PORTB &= ~(1 << PORTB2);	// clear bit 2 to turn LED off
			}
		} 
	  }
	}
	return 0;		/* never reached */
}
