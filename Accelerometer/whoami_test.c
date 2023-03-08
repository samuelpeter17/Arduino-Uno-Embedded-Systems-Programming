/* Accelerometer test: Confirm that the device is online
 *
 * ENGS 28
 *
 * Target device:	ATmega328p (Arduino UNO), Adafruit LSM303AGR breakout
 * Tool version:
 * Dependencies:	i2c.c, ioE28.c, USARTE28.c
 *
 *
 */

/* INCLUDE FILES */
#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>			// For the _delay_ms macro
#include <ioE28.h>
#include <USARTE28.h>
#include <i2c.h>

/* MACROS */
#define LSM303_ADDRESS_ACCEL       (0x32 >> 1)  // 7-bit address 0011001b
#define LSM303_WHO_AM_I_A		   0x0F	  		// default value: 00110011

/* FUNCTIONS */
uint8_t lsm303_AccelRegisterRead(uint8_t RegisterAddress);

int main(void) {
	i2cInit();
	USART_Init();
	printf("LSM303AGR initialization test \n\r");
	
	uint8_t who_am_i = 0;
	
	while(1) {
		who_am_i = lsm303_AccelRegisterRead(LSM303_WHO_AM_I_A);
		if( who_am_i == 0x33 ) {
			printf("Accelerometer initialized! \n\r");
		}
		else {
			printf("Could not connect to accelerometer \n\r");
		}
		_delay_ms(1000);
	}
	return 0;
}
 
/* Read one byte from an accelerometer register (Datasheet, Table 13) */
uint8_t lsm303_AccelRegisterRead(uint8_t RegisterAddress) {		
	i2cStart();
	i2cSend(LSM303_ADDRESS_ACCEL << 1);  // address with write
    i2cSend(RegisterAddress);			 // register (subaddress)
    i2cStart();				 			 // repeated start
    i2cSend(LSM303_ADDRESS_ACCEL << 1 | 0x01);  // address with read
    uint8_t data = i2cReadNoAck();
    i2cStop();
 	return data;
}