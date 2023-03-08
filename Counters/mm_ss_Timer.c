/*Timer for minutes and seconds
 * 
 *
 * Program name:	mm_ss_Timer.c
 * Engs 28
 *
 * Target device:	ATmega328p / Arduino UNO
 * Dependencies:	i2c.c, SevenSeg.c
 */

/* INCLUDE FILES */
#include <avr/io.h>					//All the port definitions are here
#include <util/delay.h>
#include "i2c.h"					// Williams' code
#include "SevenSeg.h"				// The new library
#include <avr/interrupt.h>			// interrupts
#include <USARTE28.h>   // UART initializations

void timer1_init(uint16_t timeout);		// Set up timer, with timeout value
volatile uint8_t timerFlag = 0; 		// Global variable from ISR

/* CODE */
int main(void) {
	uint16_t display_buffer[HT16K33_NBUF];

  	i2cInit();							// initialize the i2c
    SevenSeg_init();					// initialize the sevenseg driver
	timer1_init(62500);					// initialize the timer
	uint8_t brightness = 15;			// variable to store the brightness of the seven segment display
	uint16_t seconds_passed = 0;		// variable to store the number of seconds_passed
	uint16_t minutes_passed = 0;		// variable to store the number of minutes_passed
	
    
	// Opening message
	SevenSeg_dim(brightness);
	display_buffer[0] = numbertable[0];		    //0
	display_buffer[1] = numbertable[0];			//0
	display_buffer[2] = 0b11111111;				//colon
	display_buffer[3] = numbertable[0];			//0
	display_buffer[4] = numbertable[0];			//0
	SevenSeg_write(display_buffer);
	
	uint16_t x = 0;								//temporary variable
	
			
	while(1) {
		if(timerFlag){
			seconds_passed = seconds_passed + 1;		// currently slow counting mode; disable for fast counting
			timerFlag = 0;
			//SevenSeg_number(seconds_passed, display_buffer);//function to get the segments for i2c write of a four digit number
			//SevenSeg_write(display_buffer);					// write 4 segments out
		}
		//seconds_passed = seconds_passed + 1;			//enable for fast counting, else leave disabled
		if(seconds_passed == 60){
			minutes_passed = minutes_passed + 1;
			if(minutes_passed == 60)
				minutes_passed = 0;
			seconds_passed = 0;
		}
		x = minutes_passed * 100 + seconds_passed;		//trick method to calculate the number to display
		SevenSeg_number(x, display_buffer);//function to get the segments for i2c write of a four digit number
		
		// make colon blink every for better visual effect
		if((seconds_passed%2) == 0){
			display_buffer[2] = 0b11111111;
		}else{
			display_buffer[2] = 0;
		}
		SevenSeg_write(display_buffer);					// write 4 segments out

		
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

