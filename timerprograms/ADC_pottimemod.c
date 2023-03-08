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
#include <avr/interrupt.h> 
#include <i2c.h>

#define TSAMPLE 1000	// milliseconds


void ADC_init(void);
uint16_t ADC_getValue(void);
void timer1_init(uint16_t timeout);		// Set up timer, with timeout value
volatile uint8_t timerFlag = 0; 		// Global variable from ISR

int main(void) {
  uint16_t value;
  
  USART_Init();
  ADC_init();
  timer1_init(62500);					// initialize the timer

  printf("Testing ADC with a variable voltage source\n\r");
  
  while (1) { 
	  if(timerFlag){
		value = ADC_getValue();
		printf("ADC value = %x, \t  %u \r\n",  value, value);
		timerFlag = 0;
	  }
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
    while ((ADCSRA & (1 << ADSC)) != 0){}  // Wait for completion
    uint16_t value = ADC;       // Read the result
    return value;
}

void timer1_init(uint16_t timeout) {
  TCCR1B |= (1 << WGM12);				// Set mode to CTC
  TIMSK1 |= (1 << OCIE1A);				// Enable timer interrupt
  OCR1A = timeout;						// Load timeout value
  TCCR1B |= (1 << CS12);				// Set prescaler to 256
  sei();
}

ISR(TIMER1_COMPA_vect) {				
  timerFlag = 1; // time for another sample
}

