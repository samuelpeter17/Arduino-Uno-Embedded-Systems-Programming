/* TTmotor driver
 * ENGS 28
 * Description:		Counter --> PWM --> TTmotor. 
 *					Mimicking PWM from the Adafruit motor shield, use a PWM frequency
 *					≈ 1.6kHz (625µs period).
 *
 * Target device:	Arduino UNO / TT motor / TB6612FNG driver
 * Tool version:	AVR-GCC
 * Dependencies:	USARTE28.c, ioE28.c
 *
 * TB6612:			AIN1 --> PB3
 *					AIN2 --> PB2
 *					PWMA --> PB1
 */

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

void PWMtimer1_init(void); 		//function to initialize the timer
void PWMtimer1_set(uint16_t pulse);	//funtion to set the pulse rate
void  motor_init(void);				//function to initialize the motor
void motor_speed(uint16_t pwm_value);	//funtion to update motor speed
void motor_mode(uint8_t mode);		//funtion to update the direction wheel rotates in 