/* AdafruitIOTestGetDataWithLibrary
 * This example connects to a feed on AdafruitIO and
 * reads the most recent data point.
 * This example uses the AdafruitIODrv library.
 * 
 * created January 15, 2023
 * by Petra Bonfert-Taylor.
 */

#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>			// For the _delay_ms macro
#include "ioE28.h"
#include "USARTE28.h"
#include "wifiDrv.h"
#include "AdafruitIODrv.h"

#include "mySecrets.h"


/* variables */
// please enter your sensitive data in mySecrets.h
char networkName[] = MY_WLAN_SSID;      // your network SSID (name)
char networkPwd[]  = MY_WLAN_PASS;      // your network password (use for WPA, or use as key for WEP)

char aio_key[] = IO_KEY;             // your AdafruitIO key
char aio_usr[] = IO_USERNAME;        // your AdafruitIO username

int main(void) {	
	char aio_feed[] = "temperature";     // name of the AdafruitIO feed you are querying

	int8_t fahrenheit;
	
	USART_Init();
	SPIinit();	
	
	printf("\n\r connecting to wifi network %s.\n\r", networkName);	
	if (!WifiInit(networkName, networkPwd)) {
		return -1;
	}
	printWifiStatus();
	
    printf("\n\rConnecting to server %s \r\n", AdafruitIOGetServerName());
    
    if (AdafruitIOConnect() != ESP32_CONNECT_SUCCESS) {
    	return -1;
    }
    
    printf("\r\nGetting integer from feed %s.\r\n", aio_feed);
    if (!AdafruitIOGetInt(aio_usr, aio_key, aio_feed, &fahrenheit)){
    	return -1;
    };
       
    printf("Temperature = %d F.\r\n", fahrenheit);
	  	  
	return 0;		/* never reached */
}

