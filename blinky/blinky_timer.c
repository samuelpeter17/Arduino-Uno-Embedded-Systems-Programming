/* Blinky_timer
 * Turns on LED for some period, then off again, repeatedly
 * This version uses a timer interrupt rather than _delay_ms()
 * ENGS 28
 *
 * For a 500ms delay (1s flash period), prescale by 256:
 *		16E6 cycles/sec * 0.500 sec / 256 = 31250
 * Use TIMER1 because it has enough capacity for 31250
 */

#include <avr/io.h> 
#include <avr/interrupt.h>

void timer1_init(uint16_t timeout);		// Set up timer, with timeout value
volatile uint8_t timerFlag = 0; 		// Global variable from ISR

int main(void) {
    DDRB |= (1 << DDB5);    			// configure bit 5 as output
    timer1_init(31250);					// initialize the timer
    sei();         						// enable interrupts
    
    while (1) {    
        if (timerFlag) {
            timerFlag = 0;				// put the flag down
            PORTB ^= (1 << PORTB5);		// toggle the LED
        }
		// This is where the processor could be put to sleep
    }
    return 0;
}
void timer1_init(uint16_t timeout) {
  TCCR1B |= (1 << WGM12);				// Set mode to CTC
  TIMSK1 |= (1 << OCIE1A);				// Enable timer interrupt
  OCR1A = timeout;						// Load timeout value
  TCCR1B |= (1 << CS12);				// Set prescaler to 256
}

ISR(TIMER1_COMPA_vect) {				
  timerFlag = 1; // time for another sample
}
