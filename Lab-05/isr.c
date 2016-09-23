/*
 * isr.c
 */

#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include "uart.h"

//variables global to the IRQ handlers which dictates if timer is enabled &  timer counter


void PDB0_IRQHandler(void){ //For PDB timer
	
	

	return;
}
	
void FTM0_IRQHandler(void){ //For FTM timer
	
	
	
	return;
}
	
void PORTA_IRQHandler(void){ //For switch 3
	
	
	return;
}
	
void PORTC_IRQHandler(void){ //For switch 2
	
	
	
	return;
}
