/* ENGS 28
 * Description:	 	Program to read who_am_i register and print it on seven segment display 
 *
 * Target device:	ATmega329p (Arduino UNO), Adafruit LSM303AGR breakout
 * Tool version:
 * Dependencies:	lsm303agr, i2c, uart
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
	
	//variables to store
	int16_t tilt = 0;		//accelerometer tilt with 2 axis
	int16_t tilt_x = 0;		//angle between x-axis and horizon
	int16_t tilt_y = 0;		//angle between y-axis and horizon
	int16_t tilt_z = 0;		//angle between z-axis and horizon
	
	float val = 180.0/PI;	//variable to convert radian to degrees
  
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
	
	DDRD |= (1 << PORTD7);			// Configure D7 as output
	DDRD |= (1 << PORTD6);			// Configure D6 as output
	DDRD |= (1 << PORTD5);			// Configure D5 as output
	DDRD |= (1 << PORTD4);			// Configure D4 as output
	
	//states of image orientation/where the top of the phone is on 2d plane
	typedef enum{UP,DOWN,LEFT,RIGHT} direction_t;		//iOS modes as in enum{Portrait,Upside_down,Landscape_right,Landscape_left}
	direction_t direction = UP;		//start direction
	
	while(1) {
		if(timerFlag){
			timerFlag = 0;
			times = times + 1;				//store how many 1/125th of a second has passed
		}

		//calculate data from accelerometer
		lsm303_AccelReadRaw(&accel_raw);
		accel_x = (int16_t) ( ((int32_t) accel_raw.x * full_range_max) >> bits );
		accel_y = (int16_t) ( ((int32_t) accel_raw.y * full_range_max) >> bits );
		accel_z = (int16_t) ( ((int32_t) accel_raw.z * full_range_max) >> bits );
		
		tilt = (int16_t)round(atan2((float)accel_x,(float)accel_y) * val);	//calculate tilt to be displayed on seven segment display	
		
		//calculate angle between axes and horizon
		tilt_x = (int16_t)round(atan2((float)accel_x,sqrt((float)pow(accel_y,2)+(float)pow(accel_z,2))) * val);
		tilt_y = (int16_t)round(atan2((float)accel_y,sqrt((float)pow(accel_x,2)+(float)pow(accel_z,2))) * val);
		tilt_z = (int16_t)round(atan2(sqrt((float)pow(accel_x,2)+(float)pow(accel_y,2)),(float)accel_z) * val);
		
		if(times == 125){	//if one second has passed
			times = 0;
			printf("Value of tilt_x is: %d, value of tilt_y is %d, and value of tilt_z is: %d \r\n",tilt_x, tilt_y, tilt_z);	//print output

			//print 2d tilt value on seven segment display
			SevenSeg_number(abs(tilt),display_buffer);
			if(tilt<0){
				display_buffer[0] = 0b01000000;	//-ve sign if angle is negative
				SevenSeg_write(display_buffer);
			}else{
				SevenSeg_write(display_buffer);
			}
			
			if(abs(accel_y) > abs(accel_x)){		//check if we are changing orientation along y or x axis
				if(accel_y < 0){
					direction = UP;			// set direction to UP since phone would be right side up
				}else{
					direction = DOWN;		// else set direction to be DOWN
				}
			}else if(abs(accel_z) > abs(accel_x)){	//set image orientation to up if phone is parallel to ground
				direction = UP;
			}else if(accel_x < 0){		//check if top of phone is on our left or right
				direction = RIGHT;		//set direction to be RIGHT 
			}else{
				direction = LEFT;		//else set direction to left
			}
		}
		
		//turn on respective LED to show where the top of the phone is
		switch(direction){
			case UP:
				PORTD |= (1 << PORTD6);			// Turn on LED connected to D6
				PORTD &= ~((1 << PORTD7)|(1 << PORTD5)|(1 << PORTD4));			// Turn off other LEDs
				break;
			case DOWN:
				PORTD |= (1 << PORTD5);			// Turn on LED connected to D6
				PORTD &= ~((1 << PORTD7)|(1 << PORTD6)|(1 << PORTD4));			// Turn off other LEDs
				break;	
			case LEFT:
				PORTD |= (1 << PORTD4);			// Turn on LED connected to D6
				PORTD &= ~((1 << PORTD7)|(1 << PORTD5)|(1 << PORTD6));			// Turn off other LEDs
				break;
			case RIGHT:
				PORTD |= (1 << PORTD7);			// Turn on LED connected to D6
				PORTD &= ~((1 << PORTD6)|(1 << PORTD5)|(1 << PORTD4));			// Turn off other LEDs
				break;				
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