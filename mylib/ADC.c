/* SevenSeg.h
 * Device driver for analog to digital conversion
 * Engs 28
 */
 
#include <avr/io.h>		// All the port definitions are here

// Initiate a conversion and return the result
uint16_t ADC_getValue(void) {
    ADCSRA |= (1 << ADSC);       // Start conversion
    while ((ADCSRA & (1 << ADSC)) != 0){}  
// Wait for completion
    uint16_t value = ADC;       // Read the result
    return value;
}

void ADC_setChannel(uint8_t channel){
	if(channel == 0)
		ADMUX  &= 0xF0;         // Use analog channel 0
	if(channel == 1){
		//ADMUX  &= 0xF0;         // Clear previous channel
		ADMUX |= (1 << MUX0);	// Use channel 1
	}
	if(channel == 2){
		//ADMUX  &= 0xF0;         // Clear previous channel
		ADMUX |= (1 << MUX1);	// Use channel 2
	}
	if(channel == 3){
		//ADMUX  &= 0xF0;         // Clear previous channel
		ADMUX |= ((1 << MUX0)|(1 << MUX1));	// Use channel 3
	}
	if(channel == 4){
		//ADMUX  &= 0xF0;         // Clear previous channel
		ADMUX |= (1 << MUX2);	// Use channel 4
	}
	if(channel == 5){
		//ADMUX  &= 0xF0;         // Clear previous channel
		ADMUX |= ((1 << MUX0)|(1 << MUX2));	// Use channel 5
	}
}

void ADC_setReference(uint8_t Vref){
	if(Vref == 5){
		ADMUX |= ((1 << REFS0));
		ADMUX &= ~(1 << REFS1);
	}else{
		ADMUX |= ((1 << REFS0) | (1 << REFS1));
	}
}

// Initialize the ADC
void ADC_Init(void) {
  ADC_setReference(5);
  ADC_setChannel(0);
  ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2); // Prescale by 128
  ADCSRA |= (1 << ADEN);     // Enable ADC
}