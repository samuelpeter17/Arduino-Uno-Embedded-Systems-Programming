/* Shell program to test LSM303AGR driver code
 *
 * ENGS 28
 * Description:	 	Simple calls to the driver functions for my LSM303AGR accelerometer, 
 * 					to test basic functionality.
 *
 * Target device:	ATmega329p (Arduino UNO), Adafruit LSM303AGR breakout
 * Tool version:
 * Dependencies:	lsm303agr, i2c, uart
 *
 * Revisions:
 *
 */

/* INCLUDE FILES */
#include <avr/io.h>			// All the port definitions are here
#include <util/delay.h>		// For the _delay_ms macro
#include "i2c.h"
#include "lsm303agr.h"
#include "USARTE28.h"  		// UART initializations
#include "ioE28.h"      	// tiny_printf as well as some reading

/* FUNCTIONS */
// None

/* CONSTANTS */
const uint8_t bits = 15;
const uint16_t full_range_max = 2000;	// milli g

/* CODE */
int main(void) {
  	// Setup code (run once) goes here:
	int16_t accel_x, accel_y, accel_z;
	lsm303AccelData_s accel_raw;
	
	//printf("LSM303AGR test program \n\r");
	i2cInit();
	USART_Init();
	
	if( lsm303_AccelInit() ) {
		//printf("Accelerometer initialized! \n\r");
	}
	else {
		//printf("Could not connect to accelerometer \n\r");
	}
  	
  	//printf("Control Register 1 = %x \n\r", lsm303_AccelRegisterRead(LSM303_CTRL_REG1_A));
  	//printf("Control Register 4 = %x \n\r", lsm303_AccelRegisterRead(LSM303_CTRL_REG4_A));

	// Main code (runs in an infinite loop) goes here:
	
	printf("accel_x, accel_y, accel_z\n");
	while(1) {
		lsm303_AccelReadRaw(&accel_raw);
		accel_x = (int16_t) ( ((int32_t) accel_raw.x * full_range_max) >> bits );
		accel_y = (int16_t) ( ((int32_t) accel_raw.y * full_range_max) >> bits );
		accel_z = (int16_t) ( ((int32_t) accel_raw.z * full_range_max) >> bits );
		
		printf("%d\t%d\t%d\n\r", accel_x, accel_y, accel_z);

	//printf("accel_raw_x=%x, accel_raw_y=%x, accel_raw_z=%x \n\r", accel_raw.x, accel_raw.y, accel_raw.z);
	//printf("accel_x=%d mg, accel_y=%d mg, accel_z=%d mg \n\n\r", accel_x, accel_y, accel_z);
	
		_delay_ms(100);
	}

	return 0;		/* never reached */
}