/* counter_reset_button
 * Simple demonstration of counter with reset, interrupt using external interupts
 * Engs 28
 * 
 * Button connected to PD3
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
	uint8_t	count = 0;				//variable to store count

  	DDRD  &= ~(1 << DDD3);			// Configure bit 0 as input, explicitly
  	PORTD |= (1 << PORTD3);			// Turn pullup on
  	
	USART_Init();            		// Start serial comm module
	initPinChangeInterrupt();		// Start  the interrupt system
	
	buttonPushed = 0;				//variable to store whether button is pushed
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

ISR(INT1_vect) {
   buttonPushed = 1; 				// Set flag to notify main
}

void initPinChangeInterrupt(void) {
	EIMSK |= (1 << INT1);			//turn on external interrupt 1
	EICRA |= (1 << ISC11);			//detect falling edge
	sei();							//global interupt enable
}


