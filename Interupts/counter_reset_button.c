/* counter_reset_button
 * Simple demonstration of counter with reset
 * Engs 28
 * 
 * Button connected to PD7
 */
  
#include <avr/io.h>
#include <USARTE28.h>
#include <ioE28.h>
#include <util/delay.h>

#define TUPDATE	2000		// milliseconds
#define MAXCOUNT 100		

/* CONSTANTS */
const uint8_t PIN_MASK = (1 << PIND7);

int main(void) {
	uint8_t	count = 0;
	uint8_t buttonPushed = 0;
	
  	DDRD  &= ~(1 << DDD7);			// Configure bit 0 as input, explicitly
  	PORTD |= (1 << PORTD7);			// Turn pullup on
  	
	USART_Init();            			// Start serial comm module
	printf("Counter with serial output demo\r\n");
	printf("Binary \tHex \tDecimal\r\n");
	
	while(1) {
		printf("%b\t%x\t%d\r\n", count, count, count);
		_delay_ms(TUPDATE);
		
		buttonPushed = ( (PIND & PIN_MASK) == 0 );
		
		if ( (buttonPushed) || (count == MAXCOUNT-1) )  {
			count = 0;		// Reset the counter
	  	}  
	  	else {
	  		count++;
	  	}
	}
	return(0);
}
