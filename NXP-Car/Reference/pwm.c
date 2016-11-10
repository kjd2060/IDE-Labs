/************************************************************
File: pwm.c

Description: Creates 2 PWM's using FTM0 and FTM3

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/



/************************************************************
 Library and Header File Includes
************************************************************/
#include "MK64F12.h"
#include "pwm.h"
#include "stdio.h"
#include "uart.h"



/************************************************************
 File Scope Macros and Definitions
************************************************************/
#define DEFAULT_SYSTEM_CLOCK 20485760u
#define CLOCK					20485760u
#define PS 128
#define PWM0_FREQUENCY			10000
#define PWM1_FREQUENCY			50 // High resolution for 20Hz, 100 samples per period (every 1 interrupt is 1% of 20Hz)
#define FTM0_MOD_VALUE			(CLOCK/PWM0_FREQUENCY)
#define FTM3_MOD_VALUE			((CLOCK/PS)/PWM1_FREQUENCY)



 /************************************************************
Set the Motor Duty Cycle and Frequency

Parameters:
DutyCycle - (0 to 100) The Duty Cycle of the Motor PWM

Frequency - (~1000 Hz to 20000 Hz) The Frequency of the Motor
PWM

dir - The direction of the motor PWM, 1 for C3 active 
(forward) and C6 (forward), else C2 active (reverse) and C1 
active (reverse).

Returns:
N/A
************************************************************/
void SetDutyCyclePWM0( double DutyCycle, unsigned int Frequency, int dir )
{
	// Calculate the new channel value which is used to change 
	// the PWM from a 1 to a 0. When the FTM0 counter register
	// reaches this value it switches it's output from a 1 to
	// a 0.
	uint32_t mod = (uint32_t) (((CLOCK/Frequency) * DutyCycle) / 100);
  
	// Forward
	if(dir==1)
    {
    FTM0_C3V = mod; 
    FTM0_C2V=0;
    FTM0_C6V = mod; 
    FTM0_C1V=0;
    }
	
	// Reverse
	else
    {
    FTM0_C2V = mod; 
    FTM0_C3V=0;
    FTM0_C1V = mod; 
    FTM0_C6V=0;
    } 

	// Update the FTM0 clock to the new frequency, sets the overflow
	// interrupt to determine how many counts the FTM0 counter must
	// reach to restart the counter. Sets the PWM to 1 when the mod
	// is reached.
	FTM0_MOD = (CLOCK/Frequency);
}



/************************************************************
Set the Servo Duty Cycle and Frequency

Parameters:
DutyCycle - (0 to 100) The Duty Cycle of the Motor PWM

Frequency - (~1000 Hz to 20000 Hz) The Frequency of the Motor
PWM

Returns:
N/A
************************************************************/
void SetDutyCyclePWM1( double DutyCycle, unsigned int Frequency )
{
	// Calculate the new channel value which is used to change 
	// the PWM from a 1 to a 0. When the FTM0 counter register
	// reaches this value it switches it's output from a 1 to
	// a 0.
	uint32_t mod = (uint32_t) ((((CLOCK/PS)/Frequency) * DutyCycle) / 1000);
	
	FTM3_C6V = mod; 

	// Update the FTM0 clock to the new frequency, sets the overflow
	// interrupt to determine how many counts the FTM0 counter must
	// reach to restart the counter. Sets the PWM to 1 when the mod
	// is reached.
	FTM3_MOD = ((CLOCK/PS)/Frequency);
}



/************************************************************
Initialize the FTM0 module to create PWM0 (The motor control
PWM).

Parameters:
N/A

Returns:
N/A
************************************************************/
void InitPWM0( void )
{
	// 12.2.13 Enable the Clock to the FTM0 Module
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	// Enable clock on PORT A so it can output
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;
	
	// 11.4.1 Route the output of TPM channel 0 to the pins
	// Use drive strength enable flag to high drive strength
    PORTA_PCR1  = PORT_PCR_MUX(3)  | PORT_PCR_DSE_MASK; //Ch6
    PORTC_PCR2  = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK; //Ch1
    PORTC_PCR3  = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK; //Ch2
    PORTC_PCR4  = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK; //Ch3
	
	// 39.3.10 Disable Write Protection
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;
	
	// 39.3.4 FTM Counter Value
	// Initialize the CNT to 0 before writing to MOD
	FTM0_CNT = 0;
	
	// 39.3.8 Set the Counter Initial Value to 0
	FTM0_CNTIN = 0;
	
	// 39.3.5 Set the Modulo resister
	FTM0_MOD = FTM0_MOD_VALUE;

	// 39.3.6 Set the Status and Control of both channels
	// Used to configure mode, edge and level selection
	// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
	// PTC4
	FTM0_C3SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	FTM0_C3SC &= ~FTM_CnSC_ELSA_MASK;
	
	// See Table 39-67,  Edge-aligned PWM, Low-true pulses (clear out on match)
	// PTC3
	FTM0_C2SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	FTM0_C2SC &= ~FTM_CnSC_ELSA_MASK;
	
    // See Table 39-67,  Edge-aligned PWM, Low-true pulses (clear out on match)
	// PTA1
	FTM0_C6SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	FTM0_C6SC &= ~FTM_CnSC_ELSA_MASK;
    
    // See Table 39-67,  Edge-aligned PWM, Low-true pulses (clear out on match)
	// PTC2
	FTM0_C1SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	FTM0_C1SC &= ~FTM_CnSC_ELSA_MASK;
	
	// 39.3.3 FTM Setup
	// Set prescale value to 1 
	// Chose system clock source
	// Timer Overflow Interrupt Enable
	FTM0_SC = FTM_SC_PS(0) | FTM_SC_CLKS(1) | FTM_SC_TOIE_MASK;	
}



/************************************************************
Initialize the FTM3 module to create PWM1 (The servo control
PWM).

Parameters:
N/A

Returns:
N/A
************************************************************/
void InitPWM1( void )
{
	// 12.2.13 Enable the Clock to the FTM0 Module
	SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;
	
	// Enable clock on PORT A so it can output
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;
	
	// 11.4.1 Route the output of TPM channel 0 to the pins
	// Use drive strength enable flag to high drive strength
	//These port/pins may need to be updated for the K64 <Yes, they do. Here are two that work.>
    PORTC_PCR10  = PORT_PCR_MUX(3)  | PORT_PCR_DSE_MASK; //Ch6
    //PORTC_PCR4  = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK;//Ch3
	
	// 39.3.10 Disable Write Protection
	FTM3_MODE |= FTM_MODE_WPDIS_MASK;
	
	// 39.3.4 FTM Counter Value
	// Initialize the CNT to 0 before writing to MOD
	FTM3_CNT = 0;
	
	// 39.3.8 Set the Counter Initial Value to 0
	FTM3_CNTIN = 0;
	
	// 39.3.5 Set the Modulo resister
	FTM3_MOD = FTM3_MOD_VALUE;

	// 39.3.6 Set the Status and Control of both channels
	// Used to configure mode, edge and level selection
	// See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
	// PTC10
	FTM3_C6SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	FTM3_C6SC &= ~FTM_CnSC_ELSA_MASK;
	
	// 39.3.3 FTM Setup
	// Set prescale value to 1 
	// Chose system clock source
	// Timer Overflow Interrupt Enable
	FTM3_SC = FTM_SC_PS(7) | FTM_SC_CLKS(1) | FTM_SC_TOIE_MASK;	
}

/* End of File */
