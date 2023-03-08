/* counter_reset_button_intmod
 * Simple demonstration of counter with reset, interrupts prevent using delay_ms and notify of buttonPushed
 * Engs 28
 * 
 * Button connected to PD7
 * PORTD pins are pin-change interrupt channel 2
 */
  
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <USARTE28.h>
#include <ioE28.h>
#include <util/delay.h>


#define MAXCOUNT 100		


/* FUNCTIONS */
void initPinChangeInterrupt(void);
void timer1_init(uint16_t timeout);		// Set up timer, with timeout value

/* GLOBAL VARIABLES */
volatile uint8_t buttonPushed = 0;
volatile uint8_t timerFlag = 0; 		// Global variable from ISR

	
int main(void) {
	uint8_t	count = 0;

  	DDRD  &= ~(1 << DDD7);			// Configure bit 0 as input, explicitly
  	PORTD |= (1 << PORTD7);			// Turn pullup on
  	
	USART_Init();            		// Start serial comm module
	initPinChangeInterrupt();		// Start  the interrupt system
	timer1_init(62500);					// initialize the timer
	printf("Counter with serial output demo\r\n");
	printf("Binary \tHex \tDecimal\r\n");
	

	
	while(1) {
		// if 1s passed, increment count and reset timerFlag
		if(timerFlag){
			printf("%b\t%x\t%d\r\n", count, count, count);
			timerFlag = 0;
			count += 1;
		}	
		
		// if button is pushed, reset counter and variable buttonPushed
		if (buttonPushed) {
			count = 0;				// Reset
			buttonPushed = 0;		// Lower the flag
	  	}
		// if max count is reached, reset counter to 0
	  	if (count == MAXCOUNT-1) {
	  		count = 0;				// Roll over
	  	}
	}
	return(0);
}

ISR(PCINT2_vect) {
   buttonPushed = 1; 				// Set flag to notify main
}

ISR(TIMER1_COMPA_vect) {				
  timerFlag = 1; // time for another sample
}

void initPinChangeInterrupt(void) {
    PCICR  |= (1 << PCIE2); 		// Enable pin-change interrupt for D-pins
    PCMSK2 |= (1 << PD7); 			// Set pin mask for bit 7 of Port D
    sei();							// Global interrupt enable
}

void timer1_init(uint16_t timeout) {
  TCCR1B |= (1 << WGM12);				// Set mode to CTC
  TIMSK1 |= (1 << OCIE1A);				// Enable timer interrupt
  OCR1A = timeout;						// Load timeout value
  TCCR1B |= (1 << CS12);				// Set prescaler to 256
}


