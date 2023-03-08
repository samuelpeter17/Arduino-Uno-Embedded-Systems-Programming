/* Blinky
 * Turns on LED for some period, then off again, repeatedly
 */

#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>			// For the _delay_ms macro
#include "ioE28.h"
#include "USARTE28.h"
#include "wifiDrv.h"

/*  FUNCTIONS */
/* No user defined functions */

/* CONSTANTS */
#define LED_ON_TIME   500		/* milliseconds */

int main(void) {
	USART_Init();
	printf("Wifi Shield initialization test \n\r");

	// Initialize a SPI link to the ESP32 on the Wifi Shield
	SPIinit();

	// Keep trying to connect.
	while (ESP32getConnectionStatus() == WL_NO_MODULE) {
		printf("Communication with WiFi module failed!\n\r");
		_delay_ms(1000);
	}

	// Read the firmware revision
	const char *fv = ESP32getFwVersion();
	if (*fv < '1') {
		printf("Please upgrade the firmware\r\n");
	} else {
		printf("Firmware version %s.\r\n", fv);
	}

	while (1) {
		// The Wifi shield has an RGB LED, this function control it.
		ESP32setLEDs(255, 0, 0);
		_delay_ms(LED_ON_TIME);		// kill some time

		ESP32setLEDs(0, 255, 0);
		_delay_ms(LED_ON_TIME);		// kill some time

		ESP32setLEDs(0, 0, 255);
		_delay_ms(LED_ON_TIME);		// kill some time

		ESP32setLEDs(255, 255, 0);
		_delay_ms(LED_ON_TIME);		// kill some time

		ESP32setLEDs(0, 255, 255);
		_delay_ms(LED_ON_TIME);		// kill some time

		ESP32setLEDs(255, 0, 255);
		_delay_ms(LED_ON_TIME);		// kill some time

	}

	return 0; /* never reached */
}
