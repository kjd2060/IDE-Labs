/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* Spring 2016
*
* Filename: main_timer_template.c
*/

#include "MK64F12.h"
//#include "uart_ftdi.h"
#include "isr.h"
#include <stdio.h>
#include "uart.h"

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
    uart_init(); // assume this is good
    put("Starting the Program.\n\r");
	initPDB();
	initGPIO();
	initFTM();
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
	
	// Enables PDB
	PDB0_SC |= PDB_SC_PDBEN_MASK;
	
	//Enable coninuous mode
	PDB0_SC |= PDB_SC_CONT_MASK;
	
	//Set prescaler of 128
	PDB0_SC |= PDB_SC_PRESCALER(0x7);
	
	//Set multiplication of factor 20
	PDB0_SC |= PDB_SC_MULT(0x2);
	
	
	//Sets trigger to be the software trigger
	PDB0_SC |= PDB_SC_TRGSEL(0xF);
	
	//Set the mod field to get a 1 second period.
	//There is a division by 2 to make the LED blinking period 1 second.
	//This translates to two mod counts in one second (one for on, one for off)
	PDB0_MOD = PDB_MOD_MOD(0x1000);
	
	
	//Configure the Interrupt Delay register.
	//Not sure if this is the correct val here
	PDB0_IDLY = PDB_IDLY_IDLY(0x1);
	
	//Enable the interrupt mask.
	PDB0_SC |= PDB_SC_PDBIE_MASK;
	
	//Enable LDOK to have PDB0_SC register changes loaded. 
	PDB0_SC |= PDB_SC_LDMOD(0x0);
	PDB0_SC |= PDB_SC_LDOK_MASK;
	PDB0_SC |= PDB_SC_SWTRIG_MASK;
	
	return;
}

void initFTM(void){
	//Enable clock for FTM module (use FTM0)
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	
	//turn off FTM Mode to  write protection;
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;
	
	//divide the input clock down by 128,  
	FTM0_SC |= FTM_SC_PS_MASK;
	
	//reset the counter to zero
	FTM0_CNT |= FTM_CNT_COUNT(0x0);
	
	//Set the overflow rate
	//(Sysclock/128)- clock after prescaler
	//(Sysclock/128)/1000- slow down by a factor of 1000 to go from
	//Mhz to Khz, then 1/KHz = msec
	//Every 1msec, the FTM counter will set the overflow flag (TOF) and 
	FTM0_MOD |= FTM_MOD_MOD((DEFAULT_SYSTEM_CLOCK/(1<<7))/1000);
	
	//Select the System Clock 
	FTM0_SC |= FTM_SC_CLKS(0x1);
	
	//Enable the interrupt mask. Timer overflow Interrupt enable
  FTM0_SC |= FTM_SC_TOIE_MASK;
	
	return;
}

void initGPIO(void){
	// Enable LEDs
	// Enable clocks on Ports B and E for LED timing
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	// Configure the Signal Multiplexer for GPIO
	PORTB_PCR21 = PORT_PCR_MUX(1); 	//PTB21 = Blue LED
	PORTB_PCR22 = PORT_PCR_MUX(1);	//PTB22 = Red LED
	PORTE_PCR26 = PORT_PCR_MUX(1);	//PTE26 = Green LED
	
	// Switch the GPIO pins to output mode
	GPIOB_PDDR |= gBlue; // Blue LED
	GPIOB_PDDR |= gRed; // Red LED
	GPIOE_PDDR |= gGreen; // Green LED
	
	// Turn off the LEDs
	// LEDs are active low
	GPIOB_PDOR |= gBlue;
	GPIOB_PDOR |= gRed;
	GPIOE_PDOR |= gGreen;
    
	
	// Enable Buttons
  // Enable clocks
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	 
	
	// Configure the Mux for the button
	PORTC_PCR6 = PORT_PCR_MUX(0x1);	// SW2
	PORTA_PCR4 = PORT_PCR_MUX(0x1);	// SW3
	 
	// interrupt configuration for 
	// SW3 when pressed (falling-edge) 
	// SW2 when pressed and when released (both edges)
	PORTC_PCR6 |= PORT_PCR_IRQC(0xB);
	PORTA_PCR4 |= PORT_PCR_IRQC(0xA);

	// Set the push button as an input
	GPIOC_PDDR &= ~(1<<6);
	GPIOA_PDDR &= ~(1<<4);
    
	return;
}

void initInterrupts(void){
	/*Can find these in MK64F12.h*/
	// Enable NVIC for portA,portC, PDB0,FTM0
	NVIC_EnableIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(PDB0_IRQn);
  NVIC_EnableIRQ(PORTA_IRQn);
  NVIC_EnableIRQ(PORTC_IRQn);
    
	return;
}
