/* counter.c
 * Simple demonstration of counter with printout
 * 
 * Check USARTE85.h for the default baud rate
 */
  
#include <avr/io.h>
#include <util/delay.h>
#include "USARTE28.h"
#include "ioE28.h"

int main(void) {
	uint8_t count=0;
	USART_Init();            // Start the serial communication hardware
	printf("Counter with serial output demo\r\n");
	printf("Binary     \tHex \tDecimal\r\n");

	while(1) { 
		printf("%b\t%x\t%d\r\n", count, count, count);
		_delay_ms(500);
		count++;            // Will roll over 255->0   

	}
	return 0;
}
