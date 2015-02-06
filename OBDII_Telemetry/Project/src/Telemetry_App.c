/*  Telemetry Application
 *
 * Developed by: Tony Abboud
 *
 *
 * Description:
 *      This file contains the thread which continually polls the car
 *      and processes the data depending on the OBDII command sent.
 */

#include <string.h>
#include <stdlib.h>
#include "main.h"


/*
 * @brief  Send / Receive Telemetry data from the OBDII
 *          - Update global variables with data
 *
 * @param  arg: Unused parameter
 * @retval None
 */
static msg_t Telemetry_info(void * arg){
    char array[150];	// Stores received string from OBDII
    int i;
    (void) arg;         // Unused parameter
   
    // Initial values
    static int cnt=0, thread_cnt=0;
    static int fuel_cnt=0;
    
    //variables used in calculations
    int real_Vss  = 0;
    int real_MPG  = 0;
    int real_RPM  = 0;
    int real_Temp = 0;
    static int real_Fuel = 0;
    static int prev_fuel = 0;
    
    // Send/Receive Telemetry data from OBDII
    while (1){
        
        // Poll the Fuel and Temp after every 100 cycles
        if(cnt >= TOTAL_CMDS - 2){
            if(command_counter == 100)
                command_counter=101;
            else if(command_counter == 101)
                command_counter=0;
            else{
                cnt = COMMAND_OFFSET;   // Reset index of commands array
            }
        }
        if(command_counter<100)         // Only increment when its less than 100
            command_counter++;
        
        print(OBD_Command[cnt]);		// Send command to car
        for(i=0;1;i++){
            array[i] = (char)sdGet(&SD2);	//keep reading char until '>'
            if(array[i] == '>')
                break;
        }
        array[i] = '\0';
        
        strcpy(Rx_copy, array);		// Copy into global, so we dont mess with array
        if(display==1){
            switch(array[3]){
                case '0': 	//MAF_RATE
                    real_MPG = 14.7 * 6.17 * 454 * (real_Vss * 0.621371) / (3600 * get_value(4) / 100);
                    if(real_MPG>99)
                        real_MPG = 99;          // Correct the MPG to 2 sig figs
                    itoSt(MPG, real_MPG, 1);	// Copy string into MPG
                    break;
                    
                case 'C': 	//ENGINE_RPM
                    real_RPM = get_value(4)/4;
                    if(real_RPM<400)
                        strcpy(RPM, "1024");	//Hard coded due to glitches
                    else
                        itoSt(RPM, real_RPM, 0);
                    break;
                    
                case '5':	//Oil Temp
                    real_Temp = (get_value(2)-40)*(9/5)+32;	//Farenheit
                    itoSt(Eng_temp, real_Temp, 0);
                    break;
                    
                case 'D': 	//VEHICLE_SPEED !!Must be called before MPG
                    real_Vss = get_value(2)*0.62137;
                    break;
                    
                case 'F': 	//FUEL_STATUS
                    prev_fuel = real_Fuel;	//Store prev fuel %
                    //update fuel %
                    real_Fuel = (get_value(2)*100)/255;	//(A*100)/255 = Fuel_%_left
                    /*Keep the fuel % from constantly changing*/
                    if(fuel_cnt>0)
                        if(!((real_Fuel>=prev_fuel-2)&&(real_Fuel<=prev_fuel+2)))
                            real_Fuel = prev_fuel;
                    if(real_Fuel > 99)
                        real_Fuel = 99;
                    itoSt(Fuel, real_Fuel, 1);
                    fuel_cnt=1;
                    break;
                    
                default: break;		// AT commands sent so disregard
            }
            chThdSleepMilliseconds(200);
            update_display();
        }
        cnt++;
        
        // Check if any event is triggered
        ginputGetMouseStatus(0, &ev);
        if ((ev.current_buttons & GINPUT_MOUSE_BTN_LEFT)){
            if((display==1)&&(ev.y >= 0 && ev.y <=70)&&(ev.x>=width-90 && ev.x<=width)){
                // Create new thread and switch applications
                if(!thread_cnt++)
                    chThdCreateStatic(waZeroToSixty, sizeof(waZeroToSixty), NORMALPRIO, ZeroToSixty, NULL);
                display=2;
                chSemSignal(&tel);
                chSemWait(&app);    // Wait for other thread to post
                update_display();	// Refresh display since returned
            }
        }
    }
}

