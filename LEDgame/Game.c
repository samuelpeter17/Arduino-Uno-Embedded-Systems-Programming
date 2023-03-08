/* Template for C programs in Engs 28, Embedded Systems
 * 
 * Name:Samuel Peter
 * Assignment:Lab 2
 *
 * Program name:Game
 * Date created:1/14/22
 * Description:Game to test the reaction times of 2 player by having them press a button when they see a steady LED
 *
 * Dependencies: (what other source files are required)
 *n/a
 *
 * I/O pins:  (what ports are used, and what are they wired to?)
 * PB0(Pin 8): Output, LED on breadboard
 * PB1(Pin 9): Output, LED on breadboard
 * PB2(Pin 10): Output, LED on breadboard
 * PD7(Pin 7): Input, Switch on breadboard
 * PB5(Pin 13): Input, Switch on breadboard
 *
 * Revisions: (use this to track the evolution of your program)
 *
 *
 */
/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>

// If using UART
// #include "USARTE28.h"    // UART initializations
// #include "ioE28.h"       // tiny_printf as well as some reading
// If using interrupts
// #include <avr/interrupt.h>
/* FUNCTIONS */
/* Declare user-defined functions here (unless you have put the declarations in
 * their own #include file) */ 
/* CONSTANTS */
#define idle_time   50		/* milliseconds */
#define winner_time  1000     /* milliseconds*/
const uint8_t mask2 = (1 << PIND7);		//pinmask variable1
const uint8_t mask1 = (1 << PIND6);		//pinmask variable2

/* #define your constants here */
/* CODE */
int main(void)
{
   // Setup code (run once) goes here:
	DDRB |= (1 << DDB0);			// configure bit B0 as output
	DDRB |= (1 << DDB1);			// configure bit B1 as output
	DDRB |= (1 << DDB2);			// configure bit B2 as output
	DDRD &= ~(1 << DDD6);			// configure bit D6 as input
	PORTD |= (1 << PORTD6);			//enable the pullup resistor for D6
	DDRD &= ~(1 << DDD7);			// configure bit D7 as input
	PORTD |= (1 << PORTD7);			//enable the pullup resistor for D7
	typedef enum {IDLE, READY, PLAYER1, PLAYER2, WAIT} state_t;		//possible states
	state_t state = IDLE;					//starting state
	uint8_t button_waspressed = 0;			//variable to overcome button bouncing

// sei(); // Global interrupt enable (if using)
// Main code (runs in an infinite loop) goes here:
while(1) {
		//check if button was pressed
		if(((mask1 & PIND) == 0) || ((mask2 & PIND) == 0)){
			//check if it wasnt pressed before(not a bounce)
			if(button_waspressed == 0){
				//change states
				if(state == IDLE)
					state = READY;
				if(state == WAIT){
					//change state to player who presses the button first
					if((mask2 & PIND) == 0) 
						state = PLAYER2;
					if((mask1 & PIND) == 0) 
						state = PLAYER1;
				}
				button_waspressed = 1;		//variable to prevent overcome button bouncing
			}
		}else{
			button_waspressed = 0;		//reset button_waspressed
		}
		switch(state){
			case IDLE:
				//turn off both player LEDs
				PORTB &= ~(1 << PORTB0);
				PORTB &= ~(1 << PORTB2);
				//flash Green LED rapidly
				PORTB |= (1 << PORTB1);
				_delay_ms(idle_time);
				PORTB &= ~(1 << PORTB1);
				_delay_ms(idle_time);
				state = IDLE;		//if idle, stay idle
				break;
			case READY:
				//flash 1
				PORTB |= (1 << PORTB1);
				_delay_ms(winner_time);	
				PORTB &= ~(1 << PORTB1);
				_delay_ms(winner_time);
				//flash 2
				PORTB |= (1 << PORTB1);
				_delay_ms(winner_time);	
				PORTB &= ~(1 << PORTB1);
				_delay_ms(winner_time);
				//flash 3
				PORTB |= (1 << PORTB1);
				_delay_ms(winner_time);	
				PORTB &= ~(1 << PORTB1);
				_delay_ms(winner_time);
				
				state = WAIT;		//switch to waiting state
				break;
			case PLAYER1:
				//turn on Player 1 LED and then reset the game
				PORTB |= (1 << PORTB0);
				_delay_ms(winner_time);
				state = IDLE;
				break;
			case PLAYER2:
				//turn on Player 2 LED and then reset the game
				PORTB |= (1 << PORTB2);
				_delay_ms(winner_time);
				state = IDLE;
				break;
			case WAIT:
				PORTB |= (1 << PORTB1);		//steady flash if waiting
				break;
		}			
}
return 0; /* never reached */
}
/* Code for your user-defined functions go here, if not in their own source files. 
*/