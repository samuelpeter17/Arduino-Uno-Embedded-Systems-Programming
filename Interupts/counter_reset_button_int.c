/* counter_reset_button
 * Simple demonstration of counter with reset, interrupt
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

#define TUPDATE	2000		// milliseconds
#define MAXCOUNT 100		

/* CONSTANTS */
const uint8_t PIN_MASK = (1 << PIND7);

/* FUNCTIONS */
void initPinChangeInterrupt(void);

/* GLOBAL VARIABLES */
volatile uint8_t buttonPushed;
	
int main(void) {
	uint8_t	count = 0;

  	DDRD  &= ~(1 << DDD7);			// Configure bit 0 as input, explicitly
  	PORTD |= (1 << PORTD7);			// Turn pullup on
  	
	USART_Init();            		// Start serial comm module
	initPinChangeInterrupt();		// Start  the interrupt system
	
	buttonPushed = 0;
	printf("Counter with serial output demo\r\n");
	printf("Binary \tHex \tDecimal\r\n");
	
	while(1) {
		printf("%b\t%x\t%d\r\n", count, count, count);
		_delay_ms(TUPDATE);
		
		if (buttonPushed) {
			count = 0;				// Reset
			buttonPushed = 0;		// Lower the flag
	  	}  
	  	else if (count == MAXCOUNT-1) {
	  		count = 0;				// Roll over
	  	}
	  	else {
	  		count++;				// Increment
	  	}
	}
	return(0);
}

ISR(PCINT2_vect) {
   buttonPushed = 1; 				// Set flag to notify main
}

void initPinChangeInterrupt(void) {
    PCICR  |= (1 << PCIE2); 		// Enable pin-change interrupt for D-pins
    PCMSK2 |= (1 << PD7); 			// Set pin mask for bit 7 of Port D
    sei();							// Global interrupt enable
}


