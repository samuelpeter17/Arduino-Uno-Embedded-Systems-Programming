//! [Preamble]

/* Lab9
 * We created a car dashboard to control the direction and speed of 2 external DC motors. We also get whether the engine is on/off.
 * 
 * created September 1, 2022
 * by Sierra Lee, Arun Guruswamy, Samuel Peter
 */

#include <avr/io.h>				// All the port definitions are here
#include <util/delay.h>			// For the _delay_ms macro
#include "ioE28.h"
#include "USARTE28.h"
#include "wifiDrv.h"            // This contains the low level driver for the ESP32 wifi chip
#include "AdafruitIODrv.h"      // This contains the access points for the Adafruit IO service.
#include <tb6612.h>
#include <stdlib.h>				// for abs()
#include "i2c.h"

#define PWM_TIMER_MAX	  1250		// 1.6kHz with prescale 8 

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


//function prototypes to set up timers and pin change
void PWMtimer1_init(void); 
void PWMtimer1_set(uint16_t pulse);		// timer 1 is used to run the motor
		
// variables to store motor's switch states and pulse state
volatile int16_t TB6612_PWMA;
volatile uint8_t TB6612_AIN1;
volatile uint8_t TB6612_AIN2;

int main(void) {	
	uint16_t speed = 0;
	int16_t value2 = 0;		//variable to store input speed value
	motor_init();			//motor starts in stop state
	// variables to store whether the motor is currently moving forwards or backwards
	uint8_t f = 0;
	uint8_t r = 0;
	
	DDRB |= (1 << DDB1);
	DDRD |= ((1 << DDD2) | (1 << DDD4));	// Outputs to driver
	DDRD &= ~(1 << DDD3);			// input pin attached to light sensor
	PORTD |= (1 << PORTD3);			// enable the pullup resistor
	
	//! [Initialize]

	char aio_feedps[] = "ppm-speed";     // name of the AdafruitIO feed you are querying
	char aio_feed[] = "engine-status";     // name of the AdafruitIO feed you are querying
	char aio_feedr[] = "reverse";     // name of the AdafruitIO feed you are querying
	char aio_feedrpm[] = "rpm-speed";     // name of the AdafruitIO feed you are querying
	int8_t ledOnOff = 0;			//Engine on/off variable
	int8_t rev_status = 0;			//forwards/reverse variable
	uint8_t serverResponse = 1;  // 0 or 1 - whether to show server response
	
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
		//Connect to server and get engine status
		if (AdafruitIOConnect() != ESP32_CONNECT_SUCCESS) {
			printf("Unable to connect to server.\r\n");
		} else if (AdafruitIOGetInt(aio_usr, aio_key, aio_feed, &ledOnOff)) {
		    if (ledOnOff == 0) {
				ESP32setLEDs(255,255,255);	//turn on white LED
				printf("ENGINE OFF\r\n");
				motor(0,0);
				//reset f and r
				f = 0;
				r = 0;
			}
		}
		
		//Connect to server and get direction status
		if (AdafruitIOConnect() != ESP32_CONNECT_SUCCESS) {
			printf("Unable to connect to server.\r\n");
		} else if (AdafruitIOGetInt(aio_usr, aio_key, aio_feedr, &rev_status)) {
		    if (rev_status == 1) {
				printf("REVERSE \r\n");
				//reset f and r
				f = 0;
				r = 0;
			}
		}
		
		//Connect to server and get speed
		if (AdafruitIOConnect() != ESP32_CONNECT_SUCCESS) {
			printf("Unable to connect to server.\r\n");
		} else if(AdafruitIOGetInt(aio_usr, aio_key, aio_feedps, &value2)){
			if(ledOnOff == 1){
				speed = (uint16_t)(((uint32_t)value2*1250)/127);
				printf("Speed = %d \r\n", speed);
				if(speed>0){
					if(rev_status == 0){
						if(r==1){		//if motor is running in reverse, stop and then go forward
							motor(3,0);
						}
						motor(2,speed);
						ESP32setLEDs(0,255,0);		//turn on green LED
						f = 1;
					}else{
						if(f==1){		//if motor is running in forward, stop and then go backwards
							motor(3,0);
						}
						r = 1;
						motor(1,speed);
						ESP32setLEDs(0,0,255);		//turn on blue LED
					}
				}else{
					//STOP state
					motor(3,0);
					ESP32setLEDs(255,0,0);	//turn on red LED
					//reset f and r
					f = 0;
					r = 0;
				}
			}
		}
		
				//Connect to server and post speed
		if (AdafruitIOConnect() != ESP32_CONNECT_SUCCESS) {
			printf("Unable to connect to server.\r\n");
			return -1;
		}else{
			AdafruitIOPostInt(aio_usr, aio_key, aio_feedrpm, speed, serverResponse);
			
		}
		
		// This will seem slow, but for the initial demo, it will keep the query rate down when
		//  everyone wakes up at the same time and starts to hammer the account...
		_delay_ms(1000);

	} 
	//! [MainLoop]

	return 0;		/* never reached */
}
// helper function to connect to functions in tb6612
void motor(uint8_t mode, uint16_t speed){
	motor_mode(mode);
	motor_speed(speed);
}
