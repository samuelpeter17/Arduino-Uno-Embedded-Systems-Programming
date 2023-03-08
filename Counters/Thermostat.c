/* Create a thermostat interface with seven segment display showing temperature in C and F 
	* Engs 28
	*
	* Temperature sensor is wired to Analog Pin 0
	* Pot is wired to Arduino Analog Pin 1
	* Pushbutton switch is wired to Pin D3
	* Red LED is wired to Pin D6
	* Green LED is wired to Pin D7
	* See datasheet, Chapter 21, for ADC register programming
*/

#include <avr/io.h>		// All the port definitions are here
#include <USARTE28.h>   // UART initializations
#include <ioE28.h>      // Basic read/write and tiny printf
#include <avr/interrupt.h> // Pin Interupts 
#include <i2c.h>		//i2c library
#include <ADC.h>		//ADC library
#include "SevenSeg.h"	// The new library for seven segment display

#define VDD 5000		//VDD of the system for reference voltage = 5V
#define VD1 1100		//Vref if reference voltage is 1.1V


void timer1_init(uint16_t timeout);		// Set up timer, with timeout value
volatile uint8_t timerFlag = 0;
volatile uint8_t buttonPushed = 0;

int main(void) {
	DDRD |= (1 << DDD7);			// Configure bit 7 as output
	DDRD |= (1 << DDD6);			// Configure bit 6 as output
	DDRD  &= ~(1 << DDD3);			// Configure bit 3 as input, explicitly
  	PORTD |= (1 << PORTD3);			// Turn pullup on for pin D3
	
	uint16_t display_buffer[HT16K33_NBUF];	// array to store the item to be displayed on the seven segment display
	uint16_t value;					// variable to store the input voltage of ADC from temperature sensor
	int16_t tempc = 0;				// temperature in celcius
	int16_t tempf = 0;				//temperature in farenheit
	uint8_t brightness = 15;		//brightness of display, can be changed by user
	int16_t voltagefin = 0;			//temporary calculation variable
	int16_t voltagefint = 0;		//temporary calculation variable
	int16_t lower_climit = -40;		//lower temperature limit in celcius, can be changed by user
	int16_t lower_flimit = -40;		//lower temperature limit in farenheit, can be changed by user
	uint8_t seconds_passed = 0;		//variable to store the number of seconds passed, used for timed LED blinking
	uint16_t hysterisis = 5;		//hysterisis amount, can be set by the user
	int16_t stable_tempc;			//upper limit temperature in celcius, set by potentiometer
	int16_t stable_tempf;			//upper limit temperature in farenheit, set by potentiometer
	int16_t stable_tempcl = lower_climit;// lower limit temperature in celcius
	int16_t stable_tempfl = lower_flimit;// lower limit temperature in farenheit
	uint8_t updater = 0;			//variable to store whether seven segment display has been updated or not
	uint16_t pot_limit = 0;			//variable to store the voltage from potentiometer
	int16_t pot_limit_tempc = 0;	// temporary calculation variable
	
	//Initializations
	i2cInit();
	USART_Init();
	ADC_Init();
	ADC_setReference(1.1);				//set reference to 1.1V for better accuracy
	timer1_init(62500);					// initialize the timer to measure 1 second
	SevenSeg_init();
	initPinChangeInterrupt();			//pin change interupt to detect falling edge of button pressed
	
	// Opening message
	SevenSeg_dim(brightness);
	display_buffer[0] = numbertable[0];		    //0
	display_buffer[1] = numbertable[0];			//0
	display_buffer[2] = 0;						//no colon
	display_buffer[3] = numbertable[0];			//0
	display_buffer[4] = numbertable[0];			//0
	SevenSeg_write(display_buffer);
	

	
	uint8_t k = 0;				// variable to store keyboard input
	typedef enum {CELCIUS, FARENHEIT} state_t;		//states to decide which temperatures to display
	state_t state = CELCIUS;			//starting state
	
	
	while (1) {
		if(timerFlag){
			value = ADC_getValue();									//get ADC value
			int32_t voltage = ((int32_t)value * VD1) / 1024;		//convert ADC value to a voltage with respective Vref
			voltagefin = (int16_t)voltage;				//convert to int16_t for less storage space
			tempc = (voltagefin - 500)/10;				//convert voltage into temperature in celcius
			tempf = (tempc * 1.8) + 32;
			timerFlag = 0;
			seconds_passed = seconds_passed + 1;
			
			// calculation of upper temperature limit in both celcius and farenheit
			ADC_setChannel(1);
			pot_limit = ADC_getValue();
			int32_t voltaget = ((int32_t)pot_limit * VD1) / 1024;		//convert ADC value to a voltage with respective Vref
			voltagefint = (int16_t)voltaget;				//convert to int16_t for less storage space
			pot_limit_tempc = (voltagefint - 500)/10;				//convert voltage into temperature in celcius
			ADC_setChannel(0);
			
			stable_tempc = pot_limit_tempc;
			stable_tempf = (stable_tempc * 1.8) + 32;
			
			//update array to be displayed on seven segment display
			SevenSeg_number(tempc, display_buffer);
			
			//print appropriate statements based on current state
			if(state == FARENHEIT){
				//printf("ADC value = %d, \t voltage = %dmV, \t Temp(F) = %dF \r\n",  value, voltagefin, tempf);
				if(tempf<=stable_tempf && tempf >= stable_tempfl){
					updater = 0;
					printf("%d farenheit\r\n",tempf);
				}
				if(tempf > stable_tempf && updater == 0){
					stable_tempf = stable_tempf - hysterisis;
					printf("temp to be updated to %d ......... from %d \r\n",stable_tempf,tempf);
				}
				if (tempf < lower_flimit && updater == 0){
					stable_tempfl = stable_tempfl + hysterisis;
					printf("temp to be updated to %d .......... from %d \r\n",stable_tempfl,tempf);
				}
			}else{
				//printf("ADC value = %d, \t voltage = %dmV, \t Temp(C) = %dC \r\n",  value, voltagefin, tempc);
				if(tempc<=stable_tempc && tempc >= stable_tempcl){
					updater = 0;
					printf("%d celcius\r\n",tempc);
				}
				if(tempc > stable_tempc && updater == 0){
					stable_tempc = stable_tempc - hysterisis;
					printf("temp to be updated to %d ......... from %d \r\n",stable_tempc,tempc);
				}
				if (tempc < lower_climit && updater == 0){
					stable_tempcl = stable_tempcl + hysterisis;
					printf("temperature to be updated to %d .......... from %d \r\n",stable_tempcl,tempc);
				}
			}
			
			//change states based on button press
			if(buttonPushed){
				if(state == FARENHEIT){
					state = CELCIUS;
				}else{
					state = FARENHEIT;
				}
				buttonPushed = 0;
			}
		}
		
		// toggle LEDS and display temperature in seven segment display
		if(state == FARENHEIT){
			if(tempf<=stable_tempf && tempf>=stable_tempfl){
				PORTD |= (1 << PORTD7);
				PORTD &= ~(1 << PORTD6);
				SevenSeg_number(tempf*10, display_buffer);// display temperature in appropriate digits
				display_buffer[4] = 0b01110001;		//to display F
				SevenSeg_write(display_buffer);
			}else{
				//flash red LED at 1Hz
				PORTD &= ~(1 << PORTD7);
				if((seconds_passed%2) == 0){
					PORTD |= (1 << PORTD6);
				}else{
					PORTD &= ~(1 << PORTD6);
				}
			}
		}else{
			//toggle LEDS and display temperatures in seven segment display
			if(tempc<=stable_tempc && tempc>=stable_tempcl){
				PORTD |= (1 << PORTD7);
				PORTD &= ~(1 << PORTD6);
				SevenSeg_number(tempc*10, display_buffer);// display temperature in appropriate digits
				display_buffer[4] = 0b00111001;			// to display C
				SevenSeg_write(display_buffer);
			}else{
				//flash red LED at 1 Hz
				PORTD &= ~(1 << PORTD7);
				if((seconds_passed%2) == 0){
					PORTD |= (1 << PORTD6);
				}else{
					PORTD &= ~(1 << PORTD6);
				}
			}
		}
		if(tempc>stable_tempc && tempf>stable_tempf){
			display_buffer[0] = 0b01110110;	//H
			display_buffer[1] = 0b01111001; //E
			display_buffer[3] = 0b00111000;	//L
			display_buffer[4] = 0b01110011;	//P
			SevenSeg_write(display_buffer); //if temperature is too high, display HELP
		}
		if(tempc<stable_tempcl && tempf<stable_tempfl){
			display_buffer[0] = 0b01111100;	//b
			display_buffer[1] = 0b00110001;	//r
			display_buffer[3] = 0b00110001;	//r
			display_buffer[4] = 0b00110001;	//r
			SevenSeg_write(display_buffer);	// if temperature is too low, display brrr
		}
	}
	return 0;
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

ISR(INT1_vect) {
   buttonPushed = 1; 				// Set flag to notify main
}

void initPinChangeInterrupt(void) {
	EIMSK |= (1 << INT1);			//turn on external interrupt 1
	EICRA |= (1 << ISC11);			//detect falling edge
	sei();							//global interupt enable
}