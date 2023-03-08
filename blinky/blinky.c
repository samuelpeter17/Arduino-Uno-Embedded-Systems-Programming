/* Blinky
 * Turns on LED for some period, then off again, repeatedly
 */

#include <avr/io.h>			// All the port definitions are here (DDRB, PORTB)
#include <util/delay.h>

/*  FUNCTIONS */
/* No user defined functions */

/* CONSTANTS */
#define LED_ON_TIME   250		/* milliseconds */
#define LED_OFF_TIME  750       /* milliseconds */
//int count = -50;

int main(void)
{
  	// Setup code (run once) goes here
  	DDRB |= (1 << DDB5);			// configure bit 5 as output

	// Main code (runs repeatedly) goes here
	while(1) {
	  PORTB |= (1 << PORTB5);		// set bit 5 to turn LED on
	  _delay_ms(LED_ON_TIME);		// kill some time

	  PORTB &= ~(1 << PORTB5);		// clear bit 5 to turn LED off
	  _delay_ms(LED_OFF_TIME);		// kill some time
	  
	  /*
	  LED_OFF_TIME += count;
	  if(LED_OFF_TIME <=0){
		  count = 50;
	  }else{
		  if(LED_OFF_TIME >= 750){
			  count = -50;
		  }
	  }*/
	}

	return 0;		/* never reached */
}
