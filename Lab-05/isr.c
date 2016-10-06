/*
 * isr.c
 */

#include "uart.h"
#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT_IF_MASK 0x18 // PORTx interupt flag mask
#define FTM_TOF_MASK 0xFFFFFFBF // FTM TOF interrupt enable
//variables global to the IRQ handlers which dictates if timer is enabled &  timer counter


#ifndef LED_BITS
#define gRed (1<<22)
#define gBlue (1<<21)
#define gGreen (1<<26)
#define gSwitch2 (1<<6)
#endif

int gCount = 0;
int gLocalVar = 0;

int Switch2_Down(void){
    //if((GPIOC_PDIR * (gSwitch2)) == 0)
    if (0 == (GPIOC_PDIR & gSwitch2))
		{
			return 1;
    }
    else
    {
      return 0;
    }
}

void PDB0_IRQHandler(void){ //For PDB timer
    //put("PDB0 IRQ Handler\n\r");
    // clear interrupt in register PDB0_SC
    PDB0_SC &= ~(PDB_SC_PDBIF_MASK); // turn off the 6th bit, which is the IRQ flag bit
	
    // toggle the output state for LED1
		if (0 == (GPIOB_PDIR & gRed))
		{
			GPIOB_PSOR = gRed;
		} else
		{
			GPIOB_PCOR = gRed;
		}
	return;
}
	
void FTM0_IRQHandler(void){ //For FTM timer
    // clear interrupt in register FTM0_SC
    //put("FTM0 IRQHandler\n\r");
		FTM0_SC &= ~(FTM_SC_TOF_MASK);
    // if switch2 has been pressed, increment the counter var
    if(gLocalVar == 1){
        gCount += 1;
    }
    // otherwise do nothing
    
	return;
}
	
void PORTA_IRQHandler(void){ //For switch 3
	int temp;
	//put("PORTA IRQ HANDLER\n\r");
	// clear interrupt; write a logic 1 to the flag
    PORTA_PCR4 |= PORT_IF_MASK;
	
	
    // if timer is enabled, disable it
    if(0 != (PDB0_SC & (PDB_SC_PDBEN_MASK)))
    {
			//put("PDB now Disabled.\n\r");
      PDB0_SC &= ~(PDB_SC_PDBEN_MASK);
    }
    // otherwise enable it and start it with a trigger
    else
    {
			//put("PDB now Enabled.\n\r");
      PDB0_SC |= (PDB_SC_PDBEN_MASK);
			PDB0_SC |= PDB_SC_SWTRIG_MASK;
    }
		
	return;
}
	
void PORTC_IRQHandler(void){ //For switch 2
	
    //put("PORTC IRQ HANDLER\n\r");
    // clear the interrupt
    PORTC_PCR6 |= PORT_IF_MASK;
    
    // if sw2 was being pressed
    if(Switch2_Down() == 1)
    {
        // set local var to affect the timer2 function
				//put("SW2 Down\n\r");
        gLocalVar = 1;
        // reset flex timer
        FTM0_CNT = FTM_CNT_COUNT(0x0);
        // reset timer counter
        gCount = 0;
        // turn on blue LED while button pressed
        GPIOB_PCOR = gBlue;
        
    }
    // otherwise, reset local variable to affect the timer2 function
    else
    {
				//put("SW2 Up\n\r");
        gLocalVar = 0; 
        // turn off the blue LED while button is up
        GPIOB_PSOR = gBlue;
        // print the result as 'Button held for XX ms'
        put("Button held for ");
				putnumU(gCount);
        put(" ms\n\r");
    } 
	return;
}
