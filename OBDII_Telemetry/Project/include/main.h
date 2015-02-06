#ifndef MAIN_H
#define MAIN_H

/*OBDII AT COMMANDS*/
#define ELM_RESET 			"ATZ\r"
#define PROTOCOL_SELECT 	"ATSP0\r"
#define ADD_LINES 			"ATL1\r"
#define ENGINE_RPM 			"010C\r"
#define VEHICLE_SPEED 		"010D\r"
#define MAF_RATE 			"0110\r"
#define FUEL_STATUS 		"012F\r"
#define ENGINE_TEMP			"0105\r"	
#define COMMAND_OFFSET  3			// Array offset for first data command
#define TOTAL_CMDS      8			// # of commands present in software

//Commands data structure
const char *OBD_Command[TOTAL_CMDS] = {
	ELM_RESET,
	PROTOCOL_SELECT,
	ADD_LINES,
	ENGINE_RPM,
	VEHICLE_SPEED,
	MAF_RATE,
	FUEL_STATUS,
	ENGINE_TEMP
	};

//Global Variables - Used in Display
char Rx_copy[150];              // Stores received string from OBDII
char RPM[5]         = "0000";	// Vehicle RPM
char Eng_temp[4]    = "000";    // Engine Oil Temp
char MPG[3]         = "00";		// Current MPG
char Fuel[3]        = "00";     // Current Fuel %
char Speed[3];                  // Vehicle speed
char Time[5];                   // Time passed in 0-60

Semaphore tel, app;			// Semaphore used to control the applications running
coord_t height, width;		// Height and width of screen
GEventMouse	ev;				// Touchscreen variable to handle events - Button code
int display         = 1;    // Defualt display for screen
int command_counter = 100;	// When this reaches 100 we poll the fuel status and temp
int screen          = 0;	// Flag to print display 1 once



/**Functions**/
void print(const char * );
void init_USART(void);
int get_value(int );
char *itoSt(char *, int , int );
void update_display(void);
void home_screen(coord_t , coord_t );
int get_speed(void);
static msg_t Telemetry_info(void * ) __attribute__((noreturn));
static msg_t ZeroToSixty(void * ) __attribute__((noreturn));

// Get a working area for telemetry and 0-60 thread
static WORKING_AREA(waTelemetry, 512);
static WORKING_AREA(waZeroToSixty, 512);

#endif