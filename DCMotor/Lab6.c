/* Lab6.c -> Use the potentiometer as a throttle to control the direction and speed of the motor
 * ENGS 28
 * Description:		Potentiometer --> PWM --> TTmotor. 
 *					Mimicking PWM from the Adafruit motor shield, use a PWM frequency
 *					≈ 1.6kHz (625µs period).
 *
 * Target device:	Arduino UNO / TT motor / TB6612FNG driver
 * Tool version:	AVR-GCC
 * Dependencies:	USARTE28.c, ioE28.c
 *
 * TB6612:			AIN1 --> PB3
 *					AIN2 --> PB2
 *					PWMA --> PB1
 */

/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <stdlib.h>				// for abs()
#include "USARTE28.h"
#include "ioE28.h"
#include <util/delay.h>
#include <avr/interrupt.h> // Pin Interupts
#include <ADC.h>		//ADC library
#include <SevenSeg.h>	// The new library for seven segment display
#include "i2c.h"
#include <tb6612.h>

#define PWM_TIMER_MAX	  1250		// 1.6kHz with prescale 8 		

//function prototypes to set up timers and pin change
void PWMtimer1_init(void); 
void PWMtimer1_set(uint16_t pulse);		// timer 1 is used to run the motor
void timer2_init(uint16_t timeout);		// Set up timer, with timeout value to notify us when one second has passed
void initPinChangeInterrupt(void);		//interupt to detect pulses from light sensor
volatile uint8_t timerFlag = 0;		
// variables to store motor's switch states and pulse state
volatile int16_t TB6612_PWMA;
volatile uint8_t TB6612_AIN1;
volatile uint8_t TB6612_AIN2;		

volatile uint8_t buttonPushed = 0;		//variable to detect falling edges of pulses from light sensor


// Main program
int main(void) {
	int16_t value2 = 0;		//variable to store adc value
	

  DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB3);	// Outputs to driver
  DDRD &= ~(1 << DDD3);			// input pin attached to light sensor
  PORTD |= (1 << PORTD3);		// enable the pullup resistor
  
  i2cInit();
  ADC_Init();
  USART_Init();    
  SevenSeg_init();
  initPinChangeInterrupt();			//pin change interupt to detect falling edge of button pressed
  timer2_init(125);					// initialize the timer to measure 1/125th second, since we are using only 8 bits
  motor_init();						//motor starts in stop state
  
  uint8_t times = 0;
  uint16_t speed = 0;
  int16_t rpm = 0;
  uint16_t pps = 0;
  uint16_t display_buffer[HT16K33_NBUF];	// array to store the item to be displayed on the seven segment display
  uint8_t brightness = 15;		//brightness of display, can be changed by user
  
  	// Opening message
	SevenSeg_dim(brightness);
	display_buffer[0] = 0;		    //blank
	display_buffer[1] = 0;			//blank
	display_buffer[2] = 0;			//no colon
	display_buffer[3] = 0;			//blank
	display_buffer[4] = 0;			//blank
	SevenSeg_write(display_buffer);
  
  while(1) {
	if(timerFlag){
		timerFlag = 0;
		times = times + 1;				//store how many 1/125th of a second has passed
		value2 = ADC_getValue(); 		//get ADC value at 125Hz
	}
	
	if(buttonPushed){
		pps = pps + 1;		//store the number of pulses recorded by sensor
		buttonPushed = 0;	//reset buttonPushed to record again
	}
  
 	if(times == 125){	//runs once a second
		times = 0;		//reset variable
		
		//calculate the speed and rpm value of motor; speed has a linear relationship for adc values and speeds from -1250 to 1250 ppm
		speed = (uint16_t)(((uint32_t)value2*1250)/511) - 1250;
		rpm = (60/20) * pps;
		if(value2>520){
			motor_mode(2); 			//FWD mode
		}else if(value2<500){
			motor_mode(1);			//REV mode
		}else{
			motor_mode(3);			//BRAKE in dead zone between 500-520 adc values
		}
		motor_speed(speed);			//update motor speed
		printf("%d is the value of speed and %d is rpm \r\n",speed, rpm);	//print pulse rate of light sensor and rpm of wheel
		pps = 0;					//reset pulse rate for next second
		
		SevenSeg_number(rpm, display_buffer); //calculate array to display rpm in seven segment display
		if(value2<500){
			display_buffer[0] = 0b01000000;		//display negative sign if rpm is in REV mode
		}
		SevenSeg_write(display_buffer);		//display the array in seven segment display
	}
	
  }
  return 0;                            /* This line is never reached */
}

void timer2_init(uint16_t timeout) {
	TCCR0A |= (1 << WGM01);				// Set mode to CTC
	TIMSK0 |= (1 << OCIE0A);				// Enable timer interrupt
	OCR0A = timeout;						// Load timeout value
	TCCR0B |= ((1 << CS02)|(1 << CS00));	// Set prescaler to 1024
	sei();
}
	
ISR(TIMER0_COMPA_vect) {				
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