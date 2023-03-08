//! [Preamble]

/* AitLift_TestWifi
 * This example connects to Wifi and prints out the status
 * of the connection.
 * 
 * created September 1, 2022
 * by Petra Bonfert-Taylor.
 */

#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>			// For the _delay_ms macro
#include "ioE28.h"
#include "USARTE28.h"
#include "wifiDrv.h"            // This contains the low level driver for the ESP32 wifi chip
#include "AdafruitIODrv.h"

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

//! [Credentials]

int main(void) {	

	//! [Initialize]
	
	USART_Init();
	SPIinit();	
	
	//! [Initialize]


	printf("\n\rConnecting to wifi network %s.\n\r", networkName);

	//! [Netconnect]

	if (!WifiInit(networkName, networkPwd)) {
		return -1;
	}

	//! [Netconnect]

	printf("\n\rConnected. Wifi status: \n\r");
	
	printWifiStatus();

	return 0;		/* never reached */
}
