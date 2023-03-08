/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <stdlib.h>				// for abs()
#include "USARTE28.h"
#include "ioE28.h"
#include "i2c.h"
#include <util/delay.h>
#include <avr/interrupt.h> // Pin Interupts
#include <ADC.h>		//ADC library

#define PWM_TIMER_MAX	  1250		// 1.6kHz with prescale 8 

volatile int16_t TB6612_PWMA;
volatile uint8_t TB6612_AIN1;
volatile uint8_t TB6612_AIN2;			// STOP mode

typedef enum{FWD, REV, BRAKE, STOP} state_t;
volatile state_t state = STOP;

// FUNCTION implementations
// Set up PWM
void PWMtimer1_init(void) {
  TCCR1A |= (1 << WGM11); 					// fast pwm, using ICR1 as TOP
  TCCR1B |= (1 << WGM12) | (1 << WGM13); 
  TCCR1B |= (1 << CS11); 					// /8 prescale --> 2MHz clock
  ICR1    = PWM_TIMER_MAX;  				// TOP --> 1.6kHz PWM frequency
  TCCR1A |= (1 << COM1A1); 					// clear on compare match, set at bottom
  OCR1A   = 0;    							// set it to stopped, initially 
  DDRB   |= (1 << DDB1); 					// set PB1/OC1A to output 
}

void PWMtimer1_set(uint16_t value) {
// Safety first: Allow no pulse values outside the (0, PWM_TIMER_MAX-1 range!
	if (value>PWM_TIMER_MAX-1) 
		value = PWM_TIMER_MAX-1;
	else if (value < 0)
		value = 0;		
	OCR1A = value;
}

void motor_init(void){
	PWMtimer1_init();
	TB6612_PWMA = 0;
	TB6612_AIN1 = 0;
	TB6612_AIN2 = 0;			// start at STOP mode
}

void motor_speed(uint16_t pwm_value){
		
	// Now set the magnitude
	TB6612_PWMA = abs(pwm_value);		// magnitude (speed)
	
	PORTD = (PORTD & ~((1 << PD2) | (1 << PD4))) 
				| (TB6612_AIN1 << PD4) | (TB6612_AIN2 << PD2);
	PWMtimer1_set(TB6612_PWMA);		// Update the PWM
		
	
}

void motor_mode(uint8_t mode){
	if(mode == 0){
		state = STOP;
		TB6612_AIN1 = 0;	//STOP
		TB6612_AIN2 = 0;		
	}
	if(mode == 3){
		state = BRAKE;
		TB6612_AIN1 = 1;	//BRAKE
		TB6612_AIN2 = 1;	
	}
	if((mode == 1) || (mode == 2)){
		if((state == FWD) || (state == REV)){
			TB6612_AIN1 = 1;	//BRAKE before going from FWD to REV and vice versa
			TB6612_AIN2 = 1;		
		}
		if(mode == 1){
			TB6612_AIN1 = 1;	//REV
			TB6612_AIN2 = 0;
		}
		if(mode == 2){
			TB6612_AIN1 = 0;	//FWD
			TB6612_AIN2 = 1;
		}
	}
}