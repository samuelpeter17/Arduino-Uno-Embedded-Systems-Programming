/* SevenSeg.h
 * Device driver for analog to digital conversions
 * Engs 28
 */
 
#include <avr/io.h>		// All the port definitions are here

//function prototypes
void ADC_setChannel(uint8_t channel);	//Set the ADC input(0-5)
void ADC_setReference(uint8_t Vref);	//Set the ADC voltage reference
void ADC_Init(void);					//Initialize the ADC
uint16_t ADC_getValue(void);			//Initialize conversion, return result