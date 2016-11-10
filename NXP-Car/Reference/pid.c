/************************************************************
File: pid.c

Description: Process the raw camera data using a 5-point
averager and then find the mininum value of the smoothed data.
Use this value to obtain the error value used to adjust the 
pd (Proportional Derivative) controller.

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/



/************************************************************
 Library and Header File Includes
************************************************************/
#include "MK64F12.h"
#include "pid.h"
#include "uart.h"
#include "stdio.h"



/************************************************************
 File Scope Function prototypes
************************************************************/
int errorCalc( void );



/************************************************************
 File Scope Variables
************************************************************/
char str_avg[10];
int avgCamData[124];



/************************************************************
Calculates the error in the trajectory of the car that the 
camera has detected.

Parameters:
N/A

Returns:
Error: The expected (duty cycle * 10) for the servo PWM
************************************************************/
int errorCalc( void )
{

	int minValue = 65535;
	uint16_t minSegment = 0;
	int error = 0;
	int i = 0;
	
	
	// Find the lowest (Darkest) camera data point, cut off the
	// invalid data points (0-29, 100-127)
	for(i = 30; i < 100; i++)
	{	
		// If the data point is in range and lower than
		// the lowest value found so far set it to the
		// new minimum value and record its index as the
		// location of the lowest value in the array.
		if (avgCamData[i] < minValue && avgCamData[i] > 15000)
		{
			minValue = avgCamData[i];
			minSegment = i;
		}
	}
	
	// There is a range of 50 to 100 for the error so the
	// value must be normalized to that range. Since the top
	// and bottom of the averaged array are cut off, indexes
	// 30 - 100 are left so 30 is subtracted from the index of
	// the minimum value to put it in a range from 0 to 70
	// this number is then divided by 70 to get the percentage
	// of the way through the array (percentage of the way across
	// the field of exposure) at which the minimum value was found.
	error = ((50*(minSegment - 30))/70) + 50;
	
	return error;
}



/************************************************************
Use a 5 point averager to smooth the raw camera data and then
find the error in the current position of the car.

Parameters:
cameraData - The raw camera data from the ADC

Returns:
Error: The expected (duty cycle * 10) for the servo PWM
************************************************************/
int processCamData( uint16_t* cameraData )
{

	// Split camera data into 16 segments
	// Every Segement is 5 Camera data points
	//int avgCamData[124];
	long sum = 0;
	int i;

	// The current error the camera detected
	int error = 0;

	// Use a 5 point averager with coefficients 
	// bk = {0.05 0.2 0.5 0.2 0.05} to smooth out the camera data
	for (i = 2; i <= 125; i++)
	{	
		
		sum = cameraData[i-2]/20 + cameraData[i-1]/5 + cameraData[i]/2 + cameraData[i+1]/5 + cameraData[i+2]/20;
		avgCamData[i] = sum;
	}
	
	// Calculate the error in the current position of the car
	error = errorCalc();
	
	// Return the distance from 75 (Center) that the error was
	return (75-error);
}

/* End of File */
