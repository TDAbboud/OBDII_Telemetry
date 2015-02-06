/*  Zero to Sixty Application
 *
 * Developed by: Tony Abboud
 *
 *
 * Description:
 *      - This allows the user to time their vehicle from 0 to 60 mph.
 *      - Started when the user presses the "0-60" button on title screen
 */

#include <string.h>
#include <stdlib.h>
#include "main.h"


/*
 * @brief  Time user vehicle from 0-60mph
 *          - Screen1: Asks user if they want to log their 0-60 time (Yes or No (return Home))
 *          - Screen2: Actual data logging of mph and time. Max time = 30 sec.
 *          - Screen3: Final time and a return home button
 *
 * @param  arg: Unused parameter
 * @retval None
 */
static msg_t ZeroToSixty(void * arg){
    (void) arg;
    
    int return_home = 0;	// End of App flag
    //Open fonts
    font_t font1 = gdispOpenFont("UI2");
    font_t font2 = gdispOpenFont("Larger Double");
    font_t font3 = gdispOpenFont("Larger");
    font_t font5 = gdispOpenFont("UI2 Double");
    
    
    while(1){
        chSemWait(&tel);		// Wait for telemetry app to post
        
        /** Screen 1 **/
        gdispClear(Black);
        gdispDrawStringBox(0, 0, width, 40, "0 - 60 Time", font2, White, justifyCenter);	// Position box at top of screen
        gdispFillArea(0,35,width,height,Gray);	// Background color
        //Create Button for selection
        gdispFillArea(width/2-125, height/2-10,100,50,White);			// Yes button
        gdispFillArea(width/2+25,  height/2-10,100,50,White);			// No button
        gdispDrawString(width/2-85, height/2+10, "Yes", font1, Black);
        gdispDrawString(width/2+65, height/2+10, "No", font1, Black);
        gdispDrawString(width/2-90, height/2 - 50, "Do you want to log a 0-60 time?",font3 , Black);
        gdispDrawString(width/2-125, height/2 +100, "*Ensure car is stopped before pressing Yes.",font3 , Black);
        
        // Poll the on-screen buttons
        while(1){
            ginputGetMouseStatus(0, &ev);
            if ((ev.current_buttons & GINPUT_MOUSE_BTN_LEFT)){
                // Yes Button pressed
                if((ev.y >= (height/2-10) && ev.y <=(height/2+40))&&(ev.x>=(width/2-125) && ev.x<=(width/2-25)))
                    break;
                
                // No Button pressed
                if((ev.y >= (height/2-10) && ev.y <=(height/2+40))&&(ev.x>=(width/2+25) && ev.x<=(width/2+125))){
                    display = 1;
                    screen = 0;
                    return_home = 1;		//Go back to home (tile) screen
                    break;
                }
            }
        }
        
        if (return_home == 0){
            
            /** Screen 2 **/
            strcpy(Speed, "00");
            strcpy(Time, "00.0");
            gdispFillArea(0,35,width,height,Gray);	// Background color
            gdispDrawString(width/2-80, height/2-80, "Vehicle Speed:", font2, Black);
            gdispDrawString(width/2, height/2-40, "mph", font2, Red);			//"mph"
            gdispDrawString(width/2-30, height/2+10, "Time:", font2, Black);
            gdispDrawString(width/2+20, height/2+40, "sec", font2, Red);		//"sec"
            
            
            // Initial values for screen
            int veh_speed = 0;
            display = 2;
            strcpy(Speed, "00");
            strcpy(Time, "00.0");
            update_display();
            while(1){
                if (get_speed() >= 1)           // Car has started moving, mph >= 1mph
                    break;
                chThdSleepMilliseconds(200);	// Wait  200ms and test mph again
            }
            
            float run_time = 0.1;               // Time duration of speed test (100ms interval)
            while(1){
                chThdSleepMilliseconds(200);	// Sleep for 0.1 seconds
                run_time += 0.2;				// Increment time duration
                veh_speed = get_speed();
                Time[0] = ((int)run_time/10)%10 +'0';
                Time[1] = ((int)run_time/1) %10 +'0';
                Time[2] = '.';
                Time[3] = (((int)(run_time*10.0))/1)%10 +'0';
                Time[4] = '\0';
                update_display();
                if((veh_speed >= 60) || (run_time >= 30.0))	// Hit 60mph or ran for 30 seconds
                    break;
            }
            
            
            /** Screen 3 **/
            gdispFillArea(0,35,width,height,Gray);	// Clear screen under title
            gdispDrawString(width/2-70, height/2 - 80, "Your Time:",font5 , Black);
            gdispDrawString(width/2-40, height/2-40, Time, font2, Red);		// Time value
            gdispDrawString(width/2+20, height/2-40, "sec", font2, Red);	// "sec"
            // Create Button for selection
            gdispFillArea(width/2-50, height/2+10,100,50,White);			// Home button - SINGLE BUTTON centered
            gdispDrawString(width/2-10, height/2+30, "Home", font1, Black);
            
            // Poll the buttons
            while(1){
                ginputGetMouseStatus(0, &ev);
                if ((ev.current_buttons & GINPUT_MOUSE_BTN_LEFT)){
                    // Home Button Pressed
                    if((ev.y >= (height/2) && ev.y <=(height/2+70))&&(ev.x>=(width/2-100) && ev.x<=(width/2+100))){
                        display = 1;
                        screen = 0;
                        return_home = 1;		// Go back to home (tile) screen
                        break;
                    }
                } 	
            }
        }
        return_home = 0;
        chSemSignal(&app);  // Post thread
    }
}


/*
 * @brief  Gets the current vehicle speed from the OBDII
 *
 * @param  None
 * @retval integer of current vehicle speed
 */
int get_speed(void){
    char array[150];
    int veh_speed=0, i=0;
    
    print(VEHICLE_SPEED);               // Send Vehicle Speed AT command
    for(i=0;1;i++){
        array[i] = (char)sdGet(&SD2);	// Keep reading char until '>'
        if(array[i] == '>')
            break;
    }
    array[i] = '\0';        
    strcpy(Rx_copy, array);					// Copy into global, so we dont mess with array
    veh_speed = get_value(2)*0.62137;		// Numeric speed
    Speed[0] = (veh_speed/10)%10 + '0';		// Convert int to string, can use my itost() func
    Speed[1] = (veh_speed)%10 + '0';
    Speed[2] = '\0';
    return veh_speed;
}

