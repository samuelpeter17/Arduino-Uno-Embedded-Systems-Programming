/* Display ES.28. in given pattern
 * 
 *
 * Program name:	ES28.c
 * Engs 28
 *
 * Target device:	ATmega328p / Arduino UNO
 * Dependencies:	i2c.c, SevenSeg.c
 */

/* INCLUDE FILES */
#include <avr/io.h>					// All the port definitions are here
#include "i2c.h"					// Williams' code
#include "SevenSeg.h"				// The new library
#include <avr/interrupt.h>

void timer1_init(uint16_t timeout);		// Set up timer, with timeout value
volatile uint8_t timerFlag = 0; 		// Global variable from ISR

/* CODE */
int main(void) {
	uint16_t display_buffer[HT16K33_NBUF];

  	i2cInit();							// initialize i2c
    SevenSeg_init();					// initialize sevenseg driver
	timer1_init(62500);					// initialize the timer
	uint8_t brightness = 15;			// variable to store brightness level of seven segment display
	uint8_t seconds_passed = 0;			// variable to store the number of seconds passed
    
	// Opening message
	SevenSeg_dim(brightness);				// set brightness
	display_buffer[0] = 0b01111001;		    // E
	display_buffer[1] = 0b11101101;			// S.
	display_buffer[2] = 0;					// no colon
	display_buffer[3] = numbertable[2];		// 2
	display_buffer[4] = 0b11111111;			// 8.
	SevenSeg_write(display_buffer);			// send item to be displayed to i2c
			
	while(1) {
		if(timerFlag){
			seconds_passed = seconds_passed + 1;	//update number of seconds passed
			timerFlag = 0;							//reset timerFlag
		}
		if(seconds_passed == 1){
			SevenSeg_blink(1);						// start blinking when 1 second has passed
		}
		if(seconds_passed == 6){
			brightness = 0;					// lower brightness
			SevenSeg_dim(brightness);		
			SevenSeg_blink(0);				// stop blinking
		}
		if(seconds_passed == 11){			// reset
			SevenSeg_dim(15);				// reset brightness
			seconds_passed = 0;				// reset blinking
		}
	}

	return 0;		/* never reached */
}

void timer1_init(uint16_t timeout) {
  TCCR1B |= (1 << WGM12);				// Set mode to CTC
  TIMSK1 |= (1 << OCIE1A);				// Enable timer interrupt
  OCR1A = timeout;						// Load timeout value
  TCCR1B |= (1 << CS12);				// Set prescaler to 256
  sei();
}

ISR(TIMER1_COMPA_vect) {				
  timerFlag = 1; // time for another sample
}

