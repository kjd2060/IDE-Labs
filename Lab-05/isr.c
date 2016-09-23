/*
 * isr.c
 */

#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include "uart.h"

#define PDB_IF_MASK 0xFFFFFFBF // PDB interrupt flag mask
#define FTM_IF_MASK 0xFFFFFF7F // FTM interrupt flag mask
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

int Switch2_Down(void){
    if((GPIOC_PDIR * (gSwitch2)) == 0){
        return 1;
    }
    else
    {
        return 0;
    }
}

void PDB0_IRQHandler(void){ //For PDB timer
	
    // clear interrupt in register PDB0_SC
    PDB0_SC &= PDB_IF_MASK; // turn off the 6th bit, which is the IRQ flag bit
    
    // toggle the output state for LED1
    GPIOB_PCOR = gRed;
	return;
}
	
void FTM0_IRQHandler(void){ //For FTM timer
    
    // clear interrupt in register FTM0_SC
	FTM0_SC &= FTM_IF_MASK;
    // if switch2 has been pressed, increment the counter var
    if(Switch2_Down() == 1){
        gCount += 1;
    }
    // otherwise do nothing
    
	return;
}
	
void PORTA_IRQHandler(void){ //For switch 3
	
	// clear interrupt; write a logic 1 to the flag
    PORTA_PCR4 |= PORT_IF_MASK;
    
    // if timer is enabled, disable it
    if((FTM0_SC & (1<<6))==1)
    {
        FTM0_SC &= FTM_TOF_MASK;
    }
    // otherwise enable it and start it with a trigger
    else
    {
        FTM0_SC |= (1 << 6); // not sure if this is all we have to do
    }
    
	return;
}
	
void PORTC_IRQHandler(void){ //For switch 2
	
    // clear the interrupt
    // if sw2 was being pressed, set local var to
    // affect the timer2 function, reset the FlexTimer,
    // reset the timer counter, turn on blue LED while button
    // pressed
    
    // otherwise, reset local variable to affect the timer2 function
    // turn off the blue LED while button is up
    // print the result as 'Button held for XX ms'
	
	return;
}
