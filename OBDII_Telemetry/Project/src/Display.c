/*  Display Functions
 *
 * Developed by: Tony Abboud
 *
 *
 * Description:
 *      - This contains all functions which are used to update the display screen
 */

#include <string.h>
#include <stdlib.h>
#include "main.h"


/*
 * @brief  Update the main display with Telemetry data
 *
 * @param  None
 * @retval None
 */
void update_display(void){
    font_t font1, font2, font3;
    
    // Get the fonts we want to use
    font1 = gdispOpenFont("UI2");
    font2 = gdispOpenFont("UI2 Double");
    font3 = gdispOpenFont("LargeNumbers");
    
    if(display == 1){
        if(screen==0){      // Only display this stuff once!!
            screen=1;		// Set flag to never come back
            //TITLE BAR
            gdispClear(Black);
            gdispDrawStringBox(0, 0, width, 40, "OBDII Telemetry", font2, White, justifyCenter);	// Position box at top of screen
            gdispFillArea(0,35,width,height,Gray);	// Background color
            
            //RPM TILE
            gdispFillArea(5, 40, width/2-5, height/2-30, Cyan);
            gdispDrawLine(5,67,width/2-5,67,Black);
            gdispDrawString(10,45, "Engine RPM", font2, Black);
            
            //FUEL % TILE
            gdispFillArea(width/2+5, 40, width/2 -10, height/2-30, Yellow);
            gdispDrawLine(width/2+5,67,width-10,67,Black);
            gdispDrawString(width/2+30,45, "Fuel Left", font2, Black);
            gdispDrawString(width/2+90, 85, "%", font3, Red);
            
            //MPG TILE
            gdispFillArea(5, height/2+20, width/2-5, height/2-30, Green);
            gdispDrawLine(5,height/2+30+15,width/2-5,height/2+30+15,Black);
            gdispDrawString(50,height/2+20+5, "MPG", font2, Black);
            
            //Oil Temp TILE
            gdispFillArea(width/2+5, height/2+20, width/2-10, height/2-30, Purple);
            gdispDrawLine(width/2+5, height/2+30+15,width-10,height/2+30+15,Black);
            gdispDrawString(width/2+10,height/2+20+5, "Engine Temp", font2, Black);
            gdispDrawCircle(width/2+100,184,2, Red);						//Draw degree mark
            gdispDrawString(width/2+105, 182, "F", font2, Red);				//Draw F for Farenheit
            
            // 0-60 App Button
            gdispFillArea(width-50, 0, 50, 30, White);			// White Button
            gdispDrawString(width-38, 10, "0-60", font1, Black);
            
        }
        // Values to display
        gdispFillArea(5,69, width/2-5,height/2-60,Cyan);            // RPM-Box
        gdispDrawString(60, 85, RPM, font3, Red);
        
        gdispFillArea(width/2+5,69, width/4+5,height/2-60,Yellow);	// Fuel-Box
        gdispDrawString(width/2+60, 85, Fuel, font3, Red);
        
        gdispFillArea(5,height/2+30+18, width/2-5,height/2-60,Green);	//MPG-Box
        gdispDrawString(65, 185, MPG, font3, Red);
        
        gdispFillArea(width/2+5, height/2+30+18, width/4+10,height/2-60,Purple);	// Temp-Box
        if(strlen(Eng_temp) == 3)
            gdispDrawString(width/2+60, 185, Eng_temp, font3, Red);					// Temp is over 100
        else
            gdispDrawString(width/2+68, 185, Eng_temp, font3, Red);					// Temp is under 100
    }
    else{
        if(display == 2){
            gdispFillArea(width/2-50, height/2-40, 40, 30, Gray);			// Draw gray box around words in order to delete old value
            gdispDrawString(width/2-40, height/2-40, Speed, font2, Red);	// Speed value
            gdispFillArea(width/2-50, height/2+40, 60, 30, Gray);			// Draw gray box around words in order to delete old value
            gdispDrawString(width/2-40, height/2+40, Time, font2, Red);		// Time value
        }
    }
}


/*
 * @brief Displays home and setting screens
 *
 * @param   width:  width of output screen
 * @param   height: height of output screen
 * @retval None
 */
void home_screen(coord_t width, coord_t height){
    font_t font1, font2;
    
    font1 = gdispOpenFont("UI2 Double");
    font2 = gdispOpenFont("UI2");
    gdispClear(Black);
    gdispDrawString(width/2-90, height/2-60, "ENEE408A", font1, Green);
    gdispDrawString(width/2-90, height/2+20, "OBDII Telemetry", font1, Green);
    chThdSleepMilliseconds(2000);
    gdispClear(Black);
    gdispDrawString(15,20 , "Bluetooth connecting...", font2, Green);
    chThdSleepMilliseconds(2000);
    gdispDrawString(15,50 , "Initializing USART...", font2, Green);
    chThdSleepMilliseconds(1500);
    gdispDrawString(15,80 , "Connecting to OBDII...", font2, Green);
    chThdSleepMilliseconds(1500);
    gdispDrawString(15,110, "Loading UI...", font2, Green);
    chThdSleepMilliseconds(2000);
    gdispCloseFont(font1);
    gdispCloseFont(font2);
    gdispClear(Black);
}