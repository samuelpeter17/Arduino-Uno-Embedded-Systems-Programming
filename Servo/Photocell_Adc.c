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

void timer0_init(uint16_t timeout);	// Pings when it's time to update

volatile uint8_t timerFlag = 0;		// Don't forget the "volatile"!

// Main program
int main(void) {
	
	uint16_t pcell1 = 0;
	uint16_t pcell2 = 0;
	uint8_t times = 0;
	uint16_t mvoltage1 = 0;
	uint16_t mvoltage2 = 0;
  
	USART_Init();    
	ADC_Init(); 
	timer0_init(125);					// initialize the timer to measure 1/125th second, since we are using only 8 bits
	sei(); 
	printf("Photocell-detector \n\r");
  
  while(1) {
	if(timerFlag == 1){
		timerFlag = 0;
		times = times+1;
	}
 	if(times==125) {
		pcell1 = ADC_getValue();		// Pot value (0 to 1023)
		mvoltage1 = (uint16_t)(((uint32_t)5000*pcell1)/1023);
		ADC_setChannel(1);
		pcell2 = ADC_getValue();
		ADC_setChannel(0);
		mvoltage2 = (uint16_t)(((uint32_t)5000*pcell2)/1023);
		
		printf("photocell_1 voltage =%d mV, \tphotocell_2 voltage =%d mV\n\r", pcell1, pcell2);
 		times=0;					// Put the flag down
	}
	// Processor could sleep here
  }
  return 0;                            /* This line is never reached */
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

