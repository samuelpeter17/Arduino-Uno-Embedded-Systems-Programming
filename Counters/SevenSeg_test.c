/* Test the device drivers for seven segment display
 * 
 *
 * Program name:	SevenSeg_test.c
 * Engs 28
 *
 * Target device:	ATmega328p / Arduino UNO
 * Dependencies:	i2c.c, SevenSeg.c
 */

/* INCLUDE FILES */
#include <avr/io.h>					// All the port definitions are here
#include <util/delay.h>
#include "i2c.h"					// Williams' code
#include "SevenSeg.h"				// The new library

/* CODE */
int main(void) {
	uint16_t display_buffer[HT16K33_NBUF];

  	i2cInit();
    SevenSeg_init();
    
	// Opening message
	SevenSeg_dim(0);
	display_buffer[0] = 0b01110110;		    // H
	display_buffer[1] = numbertable[0];		// O
	display_buffer[2] = 0;					// no colon
	display_buffer[3] = 0b00111000;			// L
	display_buffer[4] = numbertable[10];	// A
	SevenSeg_write(display_buffer);
			
	while(1) {
	// Brightness
	for(uint8_t brightness=0; brightness<16; brightness++) {
			SevenSeg_dim(brightness);
			_delay_ms(200);
	}
		
	// Flash it 		
	SevenSeg_blink(HT16K33_BLINK_2HZ);
	_delay_ms(4900);	
	SevenSeg_blink(HT16K33_BLINK_OFF);
	}

	return 0;		/* never reached */
}

