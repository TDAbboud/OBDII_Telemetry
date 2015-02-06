/* main.c
 *
 * Developed by: Tony Abboud
 *
 * Main application code for the OBDII Telemetry system
 * Sets up system (RTOS, USART, Bluetooth, ...)
 * 	Contains code for string to int, hex to int, conversions, etc
 *	- Telemetry Application
 *	- 0 to 60 Application
 */

#include "gfx.h"
#include "ch.h"
#include "hal.h"
#include "pal.h"
#include "gfx.h"
#include "gdisp_lld_panel.h"
#include <string.h>
#include <stdlib.h>
#include "main.h"


int main(void) {
	halInit();
	chSysInit();        // Initialize RTOS
	init_USART();		// Configure USART
	gfxInit();          // Initialize display library

	ginputGetMouse(0);	// Force screen calibration
	gdispSetOrientation(GDISP_ROTATE_90);
	width = gdispGetWidth();		
	height = gdispGetHeight();
	
    // Display home screen
	home_screen(width, height);
	update_display();
	
	// Create serial threads for Telemetry and 0-60 Apps
	chThdCreateStatic(waTelemetry, sizeof(waTelemetry), NORMALPRIO, Telemetry_info, NULL);
	
	chSemInit(&tel, 0);
	chSemInit(&app, 0);
    
 while(1){
        // Do Nothing!
	}
}


/**********************
    Helper Functions
*********************/

/*
 * @brief  Send a string to OBDII
 *
 * @param  str: pointer to string that is being sent
 * @retval None
 */
void print(const char *str){
    while (*str)
        sdPut(&SD2, *(str++));	//Send string on USART2 Tx to OBDII
}


/*
 * @brief  Initialize the USART2 Peripheral
 *
 * @param   None
 * @retval None
 */
void init_USART(void){
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7)); // PA2	->	USART2_TX
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7)); // PA3	->	USART2_RX	
	sdStart(&SD2, NULL); // starts the serial driver for USART2 with default config (384008n1)
	}


/*
 * @brief  Stores the correct bytes from car in a string
 *
 * @param  bytes: Number of bytes to store
 *
 * @retval Actual hexadecimal number returned from OBDII
 */
int get_value(int bytes){
    int j = 0;
	char str[5];
	for (j = 0; Rx_copy[j]!='\0'; j++){		
		if(Rx_copy[j]=='4' && Rx_copy[j+1] == '1'){
			str[0] = Rx_copy[j+6];		// Store vehicle speed into global array
			str[1] = Rx_copy[j+7];
			if(bytes == 4){
				str[2] = Rx_copy[j+9];
				str[3] = Rx_copy[j+10];
				str[4] = '\0';
			}
			else{
				str[2] = '\0';}
			break;												
			}
	}
	return(strtol(str, NULL, 16));      // Convert string Hex to integer
}	


/*
 * @brief   Converts integer into string
 *
 *  @param  temp: Pointer to string for result
 *  @param  num: integer of number of sig figs
 *  @param  command: integer cooresponding to which command is sent
 *  
 *  @retval pointer to constructed string
 */
char *itoSt(char *temp, int num, int command){
	int val=1, i;
	if(num>=1000)	
		val = 1000;		// num is 4 sig figs	(RPM)
	else
		val = 100;		// num is 3 sig figs	(Temp)
	if(command == 1)	// Passed in 1 from MPG, Fuel or Speed
		val = 10;		// num is 2 sig figs (MPG, Fuel or Speed)

	for(i=0;val!=0;i++){
		temp[i] = (num/val)%10 +'0';
		val/=10;
	}
	temp[i] = '\0';

	return(temp);
}


