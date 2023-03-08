/* ENGS 28
 * Description:	 	Program to read who_am_i register and print it on seven segment display 
 *
 * Target device:	ATmega329p (Arduino UNO), Adafruit LSM303AGR breakout
 * Tool version:
 * Dependencies:	lsm303agr, i2c, uart, SevenSeg
 *
 * Revisions:
 *
 */

/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <stdlib.h>				// for abs()
#include "USARTE28.h"
#include "ioE28.h"
#include <avr/interrupt.h> // Pin Interupts
#include <SevenSeg.h>	// The new library for seven segment display
#include "i2c.h"
#include <lsm303agr.h>

/* FUNCTIONS */
void timer2_init(uint16_t timeout);		// Set up timer, with timeout value to notify us when one second has passed

/* CONSTANTS */
const uint8_t bits = 15;
const uint16_t full_range_max = 2000;	// milli g


volatile uint8_t timerFlag = 0;	

/* CODE */
int main(void) {
	timer0_init(125);					// initialize the timer to measure 1/125th second, since we are using only 8 bits
	i2cInit();
	USART_Init();    
	SevenSeg_init();
	
	uint16_t times = 0;
	uint16_t display_buffer[HT16K33_NBUF];	// array to store the item to be displayed on the seven segment display
	uint8_t brightness = 15;		//brightness of display, can be changed by user
	uint8_t who_am_i = 0;
  
  	// Opening message
	SevenSeg_dim(brightness);
	display_buffer[0] = 0;		    //blank
	display_buffer[1] = 0;			//blank
	display_buffer[2] = 0;			//no colon
	display_buffer[3] = 0;			//blank
	display_buffer[4] = 0;			//blank
	SevenSeg_write(display_buffer);
	
	if( lsm303_AccelInit() ) {
		printf("Accelerometer initialized! \n\r");
	}
	else {
		printf("Could not connect to accelerometer \n\r");
	}
	
	while(1) {
		if(timerFlag){
			timerFlag = 0;
			times = times + 1;				//store how many 1/125th of a second has passed
		}
		
		if(times == 125){		//if 1 second has passed
			times = 0;			//reset times
			
			who_am_i = lsm303_AccelRegisterRead(LSM303_WHO_AM_I_A);			//read who_am_i register
			printf("Value of the who_am_i register is: %d \r\n",who_am_i);	//print it
			SevenSeg_number(who_am_i,display_buffer);						//display the value on the seven segment display
			SevenSeg_write(display_buffer);
		}
	}

	return 0;		/* never reached */
}

void timer0_init(uint16_t timeout) {
	TCCR0A |= (1 << WGM01);				// Set mode to CTC
	TIMSK0 |= (1 << OCIE0A);				// Enable timer interrupt
	OCR0A = timeout;						// Load timeout value
	TCCR0B |= ((1 << CS02)|(1 << CS00));	// Set prescaler to 1024
	sei();
}
	
ISR(TIMER0_COMPA_vect) {				
	timerFlag = 1; // time for another sample
}