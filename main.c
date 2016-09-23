/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* Spring 2016
*
* Filename: main_timer_template.c
*/

#include "MK64F12.h"
#include "uart_ftdi.h"
#include "isr.h"
#include <stdio.h>

#ifndef UART_CONSTANTS
#define BAUD_RATE 9600      //default baud rate
#endif

#ifndef SWITCH_BITS
#define SW2_IRQ_ENABLE (1 << 19) | (1 << 17) | (1 << 16)
#define SW3_IRQ_ENABLE (1 << 19) | (1 << 16)
#endif

#ifndef LED_BITS
#define gRed (1<<22)
#define gBlue (1<<21)
#define gGreen (1<<26)
#define gSwitch2 (1<<6)
#endif

/*From clock setup 0 in system_MK64f12.c*/
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */

void initPDB(void);
void initGPIO(void);
void initFTM(void);
void initInterrupts(void);

int main(void){
	//initializations
	initPDB();
	initGPIO();
	initFTM();
	uart_init();
	initInterrupts();
	
	for(;;){
		//To infinity and beyond
	}
}

void initPDB(void){
	//Enable clock for PDB module
	SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
	
	
	// Set continuous mode, prescaler of 128, multiplication factor of 20,
	// software triggering, and PDB enabled
	PDB0_SC |= (1<<1); //Enable coninuous mode
	PDB0_SC |= (1<<14) | (1<<13) | (1<<12); //Set prescaler of 128
	PDB0_SC |= (1<<3); //Set multiplication of factor 20
	PDB0_SC &= ~(1<<2); //Set multiplication factor of 20
	PDB0_SS |= (1<<11) | (1<<10) | (1<<9) | (1<<8); //Sets trigger to be the software trigger
	PDB0_SC |= (1<<7); // Enables PDB
	
	//Set the mod field to get a 1 second period.
	//There is a division by 2 to make the LED blinking period 1 second.
	//This translates to two mod counts in one second (one for on, one for off)
	PDB0_MOD |= (1<<1); //Sets the period to 2 (1 second)
	PDB0_MOD &= ~(1<<0); //Sets the period to 2 (1 second)
	
	//Configure the Interrupt Delay register.
	PDB0_IDLY = 10;
	
	//Enable the interrupt mask.
    NVIC_EnableIRQ(PDB0_IRQn);
	
	//Enable LDOK to have PDB0_SC register changes loaded. 
	PDB0_SC |= (1<<0); // Loaded the MOD and IDLY changes into PDB0_SC
	
	return;
}

void initFTM(void){
	//Enable clock for FTM module (use FTM0)
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	
	//turn off FTM Mode to  write protection;
	
	
	//divide the input clock down by 128,  

	
	//reset the counter to zero
	
	
	//Set the overflow rate
	//(Sysclock/128)- clock after prescaler
	//(Sysclock/128)/1000- slow down by a factor of 1000 to go from
	//Mhz to Khz, then 1/KHz = msec
	//Every 1msec, the FTM counter will set the overflow flag (TOF) and 
	FTM0->MOD = (DEFAULT_SYSTEM_CLOCK/(1<<7))/1000;
	
	//Select the System Clock 
	
	
	//Enable the interrupt mask. Timer overflow Interrupt enable
    NVIC_EnableIRQ(FTM0_IRQn);
	
	return;
}

void initGPIO(void){
    //initialize push buttons and LEDs
    //initialize clocks for each different port used.
    // Enable clocks on Ports B and E for LED timing
    SIM_SCGC5 = SIM_SCGC5_PORTB_MASK; //  Enables Clock on Port B
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; // Enables clock on port E
	
	//Configure Port Control Register for Inputs with pull enable and pull up resistor
    // Configure mux for Outputs
    PORTB_PCR21 = PORT_PCR_MUX(1);
	PORTB_PCR22 = PORT_PCR_MUX(1);
	PORTE_PCR26 = PORT_PCR_MUX(1);
	
	// Switch the GPIO pins to output mode
	GPIOB_PDDR = gRed | gBlue;
	GPIOE_PDDR = gGreen;
	
	// Turn off the LEDs
    GPIOB_PDOR = gRed | gBlue;
	GPIOE_PDOR = gGreen;
    
    
	// Enable clock for Port C PTC6 button
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; //  Enables Clock on Port C
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	// Configure the Mux for the button
	// interrupt configuration for SW3(Rising Edge) and SW2 (Either)
	PORTC_PCR6 = SW2_IRQ_ENABLE;
	PORTA_PCR4 = SW3_IRQ_ENABLE;
	// Set the push button as an input
	GPIOC_PDOR = (1 << 6); // Sets SW 2 as input
	GPIOA_PDOR = (1 << 6); // Sets SW 3 as input
    
	return;
}

void initInterrupts(void){
	/*Can find these in MK64F12.h*/
	// Enable NVIC for portA,portC, PDB0,FTM0
	
	
	return;
}
