/* Day15_servo_template.c -- Control a servo motor with a potentiometer
 * ENGS 28
 * 
 * Description:		Potentiometer --> ADC --> PWM --> servomotor
 *
 * Target device:	Arduino UNO / SG92R servomotor
 * Tool version:	AVR-GCC
 * Dependencies:	ADC.c, USARTE28.c, ioE28.c
 *
 * Potentiometer:  	Analog channel 0 (A0)
 * Servo PWM input:	PD9
 *
 */

#include <avr/io.h>				
#include <avr/interrupt.h>
#include "ADC.h"
#include "USARTE28.h"
#include "ioE28.h"

/******** COMPLETE THESE DEFINITIONS ********/
#define TIMER1_TOP  40000	  		     // 20 ms 
#define SERVO_MIN   2000              // 1 ms 
#define SERVO_MAX   4000              // 2 ms
#define SERVO_MID   3000              // 1.5 ms 
/********************************************/

#define MAXADC 			  1024		// 10-bit converter	

void Timer0_init();	// Pings when it's time to update
void PWMtimer1_init(); 
void PWMtimer1_set(uint16_t pulse);

volatile uint8_t timerFlag = 0;		// Don't forget the "volatile"!

// Main program
int main(void) {
  int16_t pot_value = 0;
  int16_t pwm_value = SERVO_MIN;
  
  USART_Init();    
  ADC_Init();
  ADC_setChannel(3);
  PWMtimer1_init(); 
  Timer0_init();
  sei(); 
  printf("Potentiometer-controlled servo\n\r");
  
  while(1) {
 	if(timerFlag==1) {
		pot_value = ADC_getValue();		// Pot value (0 to 1023)

/*******************/
pwm_value =	(int16_t)((((int32_t)2000*pot_value)/1023)+2000);					// Map pot_value to pwm_value
/*******************/
		
		PWMtimer1_set(pwm_value);		// Update the PWM
		printf("pot_value=%d, \tpwm_value=%d\n\r", pot_value, pwm_value);
 		timerFlag=0;					// Put the flag down
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

