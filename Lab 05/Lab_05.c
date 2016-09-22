
/*
 * File:        Lab05.c
 * Purpose:     Does the lab 5 stuff
 *
 * Notes:		
 *
 */

#include "MK64F12.h"
#define BAUD_RATE 9600      //default baud rate 
#define SYS_CLOCK 20485760 //default system clock (see DEFAULT_SYSTEM_CLOCK  in system_MK64F12.c)
#ifndef SWITCH_BITS
#define SW2_IRQ_ENABLE (1 << 19) | (1 << 17) | (1 << 16)
#define SW3_IRQ_ENABLE (1 << 19) | (1 << 16)
#endif
void init()
{
	//define variables for baud rate and baud rate fine adjust
	uint16_t ubd, brfa;

	//Enable clock for PDB and FTM
	SIM_SCGC6 |= SIM_SCGC6_PORTB_MASK; // Enables clock on port B
	SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
	// Enable the NVIC_IRQ....Couldn't find in MK64F12
	NVIC_EnableIRQ(PDB0_IRQn);
	NVIC_EnableIRQ(FTM0_IRQn);
	
	// enable ADC
	NVIC_EnableIRQ(ADC1_IRQn);
	
	// read result from ADC1
	unsigned short i = ADC1_RA >> 4;
	
	i =  (0x07 & DAC0_DAT0L) | (DAC0_DAT0H << 4);
	

}

void LED_Init(void){
	// Enable clocks on Ports B and E for LED timing
	SIM_SCGC5 = SIM_SCGC5_PORTB_MASK; //  Enables Clock on Port B
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; // Enables clock on port E
	
	
	// Configure the Signal Multiplexer for GPIO
  PORTB_PCR21 = PORT_PCR_MUX(1);
	PORTB_PCR22 = PORT_PCR_MUX(1);
	PORTE_PCR26 = PORT_PCR_MUX(1);
	
	// Switch the GPIO pins to output mode
	GPIOB_PDDR = gRed | gBlue;
	GPIOE_PDDR = gGreen;
	
	// Turn off the LEDs
    GPIOB_PDOR = gRed | gBlue;
	GPIOE_PDOR = gGreen;
}

void Button_Init(void){
	// Enable clock for Port C PTC6 button
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; //  Enables Clock on Port C
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	// Configure the Mux for the button
	PORTC_PCR6 = SW2_IRQ_ENABLE;
	PORTA_PCR4 = SW3_IRQ_ENABLE;
	// Set the push button as an input
	GPIOC_PDOR = (1 << 6); // Sets SW 2 as input
	GPIOA_PDOR = (1 << 6); // Sets SW 3 as input
}
