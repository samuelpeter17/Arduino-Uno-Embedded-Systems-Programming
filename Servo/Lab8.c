/* Solartracker using a servomotor and 2 photocells
 * ENGS 28
 * 
 * Description:		Photocells --> ADC --> PWM --> servomotor
 *
 * Target device:	Arduino UNO / SG92R servomotor
 * Tool version:	AVR-GCC
 * Dependencies:	ADC.c, USARTE28.c, ioE28.c
 *
 * Photocell1:  	Analog channel 0 (A0)
 * Photocell2:  	Analog channel 1 (A1)
 * Servo PWM input:	PD9
 *
 */

#include <avr/io.h>				
#include <avr/interrupt.h>
#include "ADC.h"
#include "USARTE28.h"
#include "ioE28.h"
#include <math.h>
#include <stdlib.h>				// for abs()
#include "i2c.h"
#include <SevenSeg.h>	// The new library for seven segment display

/******** COMPLETE THESE DEFINITIONS ********/
#define TIMER1_TOP  40000	  		     // 20 ms 
#define SERVO_MIN   2000              // 1 ms 
#define SERVO_MAX   4000              // 2 ms
#define SERVO_MID   3000              // 1.5 ms 
/********************************************/

#define MAXADC 			  1024		// 10-bit converter	

//Timer function prototypes
void Timer0_init();	
void PWMtimer1_init(); 
void PWMtimer1_set(uint16_t pulse);

volatile uint8_t timerFlag = 0;		// timerFlag


// Main program
int main(void) {
  int16_t pot_value = 0;
  int16_t pwm_value = SERVO_MIN;
	uint16_t pcell1 = 0;
	uint16_t pcell2 = 0;
	int16_t e = 0;
	//int16_t Ke = 0;
	uint8_t times = 0;
	int16_t angle = 0;
	uint32_t diff = 0;
  
  //Initialize functions to use
  USART_Init();    
  ADC_Init();
  PWMtimer1_init(); 
  Timer0_init();
  i2cInit();
  SevenSeg_init();
  sei();
  
  
  uint16_t display_buffer[HT16K33_NBUF];	// array to store the item to be displayed on the seven segment display
	uint8_t brightness = 15;		//brightness of display, can be changed by user
	
  	// Opening message
	SevenSeg_dim(brightness);
	display_buffer[0] = 0;		    //blank
	display_buffer[1] = 0;			//blank
	display_buffer[2] = 0;			//no colon
	display_buffer[3] = 0;			//blank
	display_buffer[4] = 0;			//blank
	SevenSeg_write(display_buffer);
  
  while(1) {
	if(timerFlag==1){
		times = times+1;	//variable to store every other time timerFlag is raised
	}
 	if(times==2) {
		pcell1 = ADC_getValue()+97;		// Photocell1 value (0 to 1023), 97 added for error between two cells
		ADC_setChannel(1);	//change channel for input
		pcell2 = ADC_getValue();		// Photocell2 value (0 to 1023)
		ADC_setChannel(0);	//change channel for input
		e = pcell2 - pcell1;	//difference between the two photocells
		
		if(abs(e)>5){	//deaband of size 5 implemented
/*******************/
			pwm_value =	pwm_value - e;					// update pwm_value based on difference between 2 photocells
			//upper and lower bounds for pwm_value
			if(pwm_value<2000)
				pwm_value = 2000;
			if(pwm_value > 4000)
				pwm_value = 4000;
/*******************/
			PWMtimer1_set(pwm_value);		// Update the PWM
			
		}
		
		//calculate angle of solar detector
		diff = (120*(uint32_t)pwm_value)/2000;
		angle = (int16_t)diff - 180;
		//print angle value on seven segment display
		SevenSeg_number(abs(angle),display_buffer);
		if(angle<0){
			display_buffer[0] = 0b01000000;	//-ve sign if angle is negative
			SevenSeg_write(display_buffer);
		}else{
			SevenSeg_write(display_buffer);
		}		
		
		printf("e_value=%d, \t angle = %d, \t pwm_value=%d\n\r", e, angle, pwm_value);
 		times=0;					// Put the flag down
	}
	// Processor could sleep here
  }
  return 0;                            /* This line is never reached */
}

// PWM setup function
void PWMtimer1_init(void) {
  TCCR1A |= (1 << WGM11); 					// fast pwm, using ICR1 as TOP
  TCCR1B |= (1 << WGM12) | (1 << WGM13); 
  
/**********/
  TCCR1B |= (1 << CS11);								// Prescaler
/**********/
  
  ICR1    = TIMER1_TOP; 					// TOP --> 50 Hz PWM frequency
  TCCR1A |= (1 << COM1A1); 					// clear on compare match, set at bottom
  OCR1A   = SERVO_MIN;    					// set it to minimum position initially 
  DDRB   |= (1 << DDB1); 					// set PB1/OC1A to output 
}

void PWMtimer1_set(uint16_t value) {
// Safety first: Allow no pulse values outside the (SERVO_MIN, SERVO_MAX) range!
	if (value>SERVO_MAX) 
		value = SERVO_MAX;
	else if (value < SERVO_MIN)
		value = SERVO_MIN;		
	OCR1A = value;
}

// Set timer for slowest possible sampling rate (approx 61Hz), running wide-open.
// Faster than the PWM (50Hz) but works.
void Timer0_init() {
  // normal mode, counts from 0 to 255 then resets 
  TCCR0B |= (1 << CS02) | (1 << CS00);				// Set prescaler to 1024
  TIMSK0 |= (1 << TOIE0);							// interrupt on overflow
}

// Overflow interrupt handling
ISR(TIMER0_OVF_vect) {				
  timerFlag = 1; 									// time for another sample
}


