/* Use a potentiometer in a variable voltage source to explore the ADC
 * Engs 28
 *
 * Pot is wired to Arduino Analog Pin 0
 * See datasheet, Chapter 21, for ADC register programming
 */

#include <avr/io.h>		// All the port definitions are here
#include <util/delay.h> // So that you can use _delay...
#include <USARTE28.h>   // UART initializations
#include <ioE28.h>      // Basic read/write and tiny printf

#define TSAMPLE 1000	// milliseconds

void ADC_init(void);
uint16_t ADC_getValue(void);

int main(void) {
  uint16_t value;
  
  USART_Init();
  ADC_init();

  //printf("Testing ADC with a variable voltage source\n\r");
  int16_t voltage = 0;
  
  while (1) {  	  
	  value = ADC_getValue();
	  voltage = (value * 5) / 1024;
	  printf("value = %d, \t voltage = %dV\r\n",  value, voltage);
	  _delay_ms(TSAMPLE);
  }
  return 0;
}

// Initialize the ADC
void ADC_init(void) {
  ADMUX  |= (1 << REFS0);        // AVCC reference
  ADMUX  |= (1 << MUX0);         // Use analog channel 1
  ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2); // Prescale by 128
  //ADMUX = (1 << ADLAR);
  ADCSRA |= (1 << ADEN);     // Enable ADC
}

// Initiate a conversion and return the result
uint16_t ADC_getValue(void) {
    ADCSRA |= (1 << ADSC);       // Start conversion
    while ((ADCSRA & (1 << ADSC)) != 0){}  
// Wait for completion
    uint16_t value = ADC;       // Read the result
    return value;
}

