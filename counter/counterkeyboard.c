/* counter.c
 * Simple demonstration of counter with printout
 * 
 * Check USARTE85.h for the default baud rate
 */
  
#include <avr/io.h>
#include <util/delay.h>
#include "USARTE28.h"
#include "ioE28.h"
#define LED_ON_TIME   300		/* milliseconds */

int main(void) {
	int32_t count=0;
	USART_Init();            // Start the serial communication hardware
	printf("Counter with serial output demo\r\n");
	printf("Binary     \tHex \tDecimal\r\n");
	printf("%b\t%x\t%d\r\n", count, count, count);   // Print the starting count under description
	
	uint8_t r = 0;
	
	DDRB |= (1 << DDB0);			// configure bit 0 as output
	

	while(1) {
		PORTB |= (1 << PORTB0);
		_delay_ms(LED_ON_TIME);
		PORTB &= ~(1 << PORTB0);
		_delay_ms(LED_ON_TIME);
		if((UCSR0A & (1 << RXC0)) != 0){
			r = receiveByte();						// Receive byte 
			if((r == 'u') || (r == 'U'))			// Increment/Decrement count based on button press
				count++;
			if((r == 'd') || (r == 'D'))
				count --;
			printf("%b\t%x\t%d\r\n", count, count, count);	// Print result
		} 			
	}
	return 0;
}
