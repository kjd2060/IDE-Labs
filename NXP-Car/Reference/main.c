/************************************************************
File: main.c

Description: Main module for the line-following application,
contains main program loop along with debug code to test the
PWM signals.

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/



/************************************************************
 Library and Header File Includes
************************************************************/
#include "MK64F12.h"
#include "uart.h"
#include "PWM.h"
#include "CAMERA.h"
#include "pid.h"
#include "stdio.h"



/************************************************************
 File Scope Function prototypes
************************************************************/
void initialize( void );



/************************************************************
 File Scope Variables
************************************************************/
static double error = 0;
static int prev_error = 0;
static int old_prev_error = 0;
static double serv_dc = 75;



/************************************************************
The main function of the program

Parameters:
N/A

Returns:
N/A
************************************************************/
int main( void )
{

	uint16_t freq = 10000; /* Frequency = 10 kHz */
	uint16_t dir = 1;
	uint16_t cameradata[128];
	
	// Initialize UART and PWM and ADC
	initialize();

	// Print welcome over serial
	uart_put("Running... \n\r");
	
	// Set the PWM Duty Cycle for the motors
	SetDutyCyclePWM0(44,freq,dir);
	
	// Main Loop
    for(;;)
    {    
			// Every 10 ms
			if(run_main)
			{
				// Get the Raw Camera Data
				read_camera(cameradata);
				
				// Save the previous errors
				old_prev_error = prev_error;
				prev_error = error;
				
				// Get the current error of the cars location and clip it
				// to between 25 and -25
				error = processCamData( cameradata );
				if(error > 25)
				{
					error = 25;
					
				}
				else if(error < -25)
				{
					error = -25;
				}
				
				// Use PD Controller to get the new duty cycle for the servo
				// P  - Error
				// D  - error - 2*prev_error + old_prev_error
				// Kp - 0.40
				// Kd - 0.25
				serv_dc = (0.40*error) + (0.25*(error - 2*prev_error + old_prev_error)) + 75;
				SetDutyCyclePWM1(serv_dc,50);
				
				// Reset run main flag
				run_main = 0;
			}
    }
}



/************************************************************
Initialze the basic components for I/O and PWM generation

Parameters:
N/A

Returns:
N/A
************************************************************/
void initialize( void )
{
	// Initialize UART
	uart_init();	
	
	// Initialize the FTM0 with a prescaler of 1 
	// Used for 10 Khz PWM
	InitPWM0();
	
	// Initialilze the FTM3 with a prescaler of 128
	// Used for 50 Hz PWM
	InitPWM1();
    
	// Initialze the Modules used to run the camera
	init_camera();
	
}

/* End of File */
