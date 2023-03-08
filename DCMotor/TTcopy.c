/* TTmotor_pot.c -- Demonstrate PWM control of a TT motor
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
#include <util/delay.h>
#include <avr/interrupt.h> // Pin Interupts
#include <ADC.h>		//ADC library
#include "SevenSeg.h"	// The new library for seven segment display

#define PWM_TIMER_MAX	  1250		// 1.6kHz with prescale 8 		
#define SPEED_UPDATE	  2000		// 5 seconds

void PWMtimer1_init(void); 
void PWMtimer1_set(uint16_t pulse);
volatile uint8_t buttonPushed = 0;


// Main program
int main(void) {
  DDRD  &= ~(1 << DDD3);			// Configure bit 3 as input, explicitly
  PORTD |= (1 << PORTD3);			// Turn pullup on for pin D3
  int16_t TB6612_PWMA = 0;
  uint8_t TB6612_AIN1 = 0;
  uint8_t TB6612_AIN2 = 0;			// STOP mode
  
  int16_t counter_new = 0;			
  int16_t counter_old = 0;
  const int16_t counter_step = 200;
  const int16_t counter_max = PWM_TIMER_MAX;
  
  typedef enum{UP, DOWN} direction_t;
  direction_t direction = UP;
  
  DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB3);	// Outputs to driver
  
  i2cInit();
  ADC_Init();
  USART_Init();    
  PWMtimer1_init();
  SevenSeg_init();
  initPinChangeInterrupt();			//pin change interupt to detect falling edge of button pressed

  printf("PWM / TT motor demo\n\r");
  
  while(1) {
 		_delay_ms(SPEED_UPDATE);
 		counter_old = counter_new;
 		
		// Speed update.  Ramp up to +limi6, back down to -limit, repeat.
		switch(direction) {
			case UP:
				counter_new = counter_old + counter_step;	// Update the speed
				if (counter_new > counter_max) {
					counter_new = counter_max;
					direction = DOWN;
				}
				break;
			
			case DOWN:
				counter_new = counter_old - counter_step;
				if (counter_new < -counter_max) {
					counter_new = -counter_max;
					direction = UP;
				}
				break;
		}
		
		// First set direction
		if(counter_new > 0) {
			if (counter_old < 0) {			// changing direction
				TB6612_AIN1 = 1;
				TB6612_AIN2 = 1;			// short brake 
			} else {
				TB6612_AIN1 = 1;
				TB6612_AIN2 = 0;			// CW
			}
		} else {
			if (counter_old > 0) {			// changing direction
				TB6612_AIN1 = 1;
				TB6612_AIN2 = 1;			// short brake 
			} else {
				TB6612_AIN1 = 0;
				TB6612_AIN2 = 1;			// CCW
			}
		}
		
		// Now set the magnitude
		TB6612_PWMA = abs(counter_new);		// magnitude (speed)
				
		PORTB = (PORTB & ~((1 << PB2) | (1 << PB3))) 
					| (TB6612_AIN1 << PB3) | (TB6612_AIN2 << PB2);
		PWMtimer1_set(TB6612_PWMA);		// Update the PWM
		
		printf("counter=%d, \t(IN1,IN2)=(%u,%u), PWM=%d\n\r", 
					counter_new, TB6612_AIN1, TB6612_AIN2, TB6612_PWMA);

  }
  return 0;                            /* This line is never reached */
}

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

ISR(INT1_vect) {
   buttonPushed = 1; 				// Set flag to notify main
}

void initPinChangeInterrupt(void) {
	EIMSK |= (1 << INT1);			//turn on external interrupt 1
	EICRA |= (1 << ISC11);			//detect falling edge
	sei();							//global interupt enable
}

