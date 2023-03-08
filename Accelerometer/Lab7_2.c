/* ENGS 28
 * Description:	 	Program to detect accelerometer tilt and print it on seven segment display 
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
#include <math.h>

#define PI 3.14159265

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
	
	//variables to store accelerometer readings
	int16_t accel_x, accel_y, accel_z;
	lsm303AccelData_s accel_raw;
	
	uint16_t times = 0;
	uint16_t display_buffer[HT16K33_NBUF];	// array to store the item to be displayed on the seven segment display
	uint8_t brightness = 15;		//brightness of display, can be changed by user
	int16_t tilt = 0;
	float val = 180.0/PI;
  
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
		
		//read data from accelerometer along x,y,z axes
		lsm303_AccelReadRaw(&accel_raw);
		accel_x = (int16_t) ( ((int32_t) accel_raw.x * full_range_max) >> bits );
		accel_y = (int16_t) ( ((int32_t) accel_raw.y * full_range_max) >> bits );
		accel_z = (int16_t) ( ((int32_t) accel_raw.z * full_range_max) >> bits );
		
		tilt = (int16_t)round(atan2((float)accel_x,(float)accel_y) * val);		//calculate angle of tilt
		
		if(times == 125){	//check if one second has passed
			times = 0;
			printf("Value of Ax is: %d, value of Ay is %d, and the tilt is: %d \r\n",accel_x, accel_y, tilt);	//print output
			
			//print tilt value on seven segment display
			SevenSeg_number(abs(tilt),display_buffer);
			if(tilt<0){
				display_buffer[0] = 0b01000000; //-ve sign if angle is negative
				SevenSeg_write(display_buffer);
			}else{
				SevenSeg_write(display_buffer);
			}
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