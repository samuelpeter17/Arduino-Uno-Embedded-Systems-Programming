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
#define VDD 5020		//VDD of the system

void ADC_init(void);
uint16_t ADC_getValue(void);

int main(void) {
  uint16_t value;
  
  USART_Init();
  ADC_init();

  printf("Testing ADC with a variable voltage source\n\r");
  
  while (1) {  	  
	  value = ADC_getValue();
	  int32_t voltage = ((int32_t)value * VDD) / 1024;		//convert ADC value to voltage
	  int16_t voltagefin = (int16_t)voltage;				//convert to int16_t for less storage space
	  int16_t tempc = (voltagefin - 500)/10;				//calculate appropriate temperature in celcius from voltage
	  int16_t tempf = (tempc * 1.8) + 32;					//convert celcius to farenheit
	  printf("ADC value = %d, \t voltage = %dmV, \t Temp(C) = %dC, \t Temp(F) = %dF \r\n",  value, voltagefin, tempc, tempf);
	  _delay_ms(TSAMPLE);
  }
  return 0;
}

// Initialize the ADC
void ADC_init(void) {
  ADMUX  |= (1 << REFS0);        // AVCC reference
  ADMUX  &= 0xF0;         // Use analog channel 0
  ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2); // Prescale by 128
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

