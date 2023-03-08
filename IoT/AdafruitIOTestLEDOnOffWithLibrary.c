//! [Preamble]

/* AdafruitIOTestLEDOnOff
 * This example connects to an on/off feed on AdafruitIO and
 * turns on/off the on-board LED as indicated by the feed.
 * 
 * created September 1, 2022
 * by Petra Bonfert-Taylor.
 */

#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>			// For the _delay_ms macro
#include "ioE28.h"
#include "USARTE28.h"
#include "wifiDrv.h"            // This contains the low level driver for the ESP32 wifi chip
#include "AdafruitIODrv.h"      // This contains the access points for the Adafruit IO service.

/*
 * This file is key!  It segregates your "secret" information out of the main file.
 * It contains the network name and password for the wireless lan you are connecting
 * to.
 *
 * It also contains your Adafruit username and the key to your feeds.
 */
#include "mySecrets.h"

//! [Preamble]


//! [Credentials]

char networkName[] = MY_WLAN_SSID;      // your network SSID (name)
char networkPwd[]  = MY_WLAN_PASS;      // your network password (use for WPA, or use as key for WEP)

char aio_key[] = IO_KEY;             // your AdafruitIO key
char aio_usr[] = IO_USERNAME;        // your AdafruitIO username

//! [Credentials]

int main(void) {	

	//! [Initialize]

	char aio_feed[] = "onoff";     // name of the AdafruitIO feed you are querying
	int8_t ledOnOff = 0;
	
	USART_Init();
	SPIinit();	
	
	//! [Initialize]


	printf("\n\r connecting to wifi network %s.\n\r", networkName);

	//! [Netconnect]

	if (!WifiInit(networkName, networkPwd)) {
		return -1;
	}

	//! [Netconnect]

	printWifiStatus();

	//! [MainLoop]
	while (1) {
		if (AdafruitIOConnect() != ESP32_CONNECT_SUCCESS) {
			printf("Unable to connect to server.\r\n");
		} else if (AdafruitIOGetInt(aio_usr, aio_key, aio_feed, &ledOnOff)) {
		    if (ledOnOff == 1) {
				ESP32setLEDs(255,0,0);
				printf("LED ON\r\n");
			} else if (ledOnOff == 0) {
				ESP32setLEDs(0,0,0);
				printf("LED OFF\r\n");
			} else {
			// you must be in a non-binary feed... You could do something else here!
			}
		}
		// This will seem slow, but for the initial demo, it will keep the query rate down when
		//  everyone wakes up at the same time and starts to hammer the account...
		_delay_ms(5000);

	} 
	//! [MainLoop]

	return 0;		/* never reached */
}
