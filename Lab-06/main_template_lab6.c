/*
 * Main Method for testing the PWM Code for the K64F
 * PWM signal can be connected to output pins are PC3 and PC4
 * 
 * Author:  
 * Created:  
 * Modified:  
 */

#include "MK64F12.h"
#include "uart.h"
#include "PWM.h"

void initialize();
void en_interrupts();
void delay();

int main(void)
{
    int i = 0;
	// Initialize UART and PWM
	initialize();

	// Print welcome over serial
	put("Running... \n\r");
	
	//Step 3
    
	// for(;;) ;  //then loop forever
	
	//Step 9
    if(0)
    {
        for(;;)  //loop forever
        {
            uint16_t dc = 0;
            uint16_t freq = 10000; /* Frequency = 10 kHz */
            uint16_t dir = 0;
            char c = 48;
            int i=0;
            
            // 0 to 100% duty cycle in forward direction
            put("Starting forward 0-100\r\n");
            for (i=0; i<100; i++){
                if(i%10 == 0)
                {
                    putnumU(i);
                    put("\r\n");
                }
                SetDutyCycle(i, freq, dir);
                delay(10);
            }
            put("Done forward 0-100\n\r");
            delay(10);
            // 100% down to 0% duty cycle in the forward direction
            put("Starting foward 100-0\n\r");
            for (i=100; i>=0; i--){
                if(i%10 == 0)
                {
                    putnumU(i);
                    put("\r\n");
                }
                SetDutyCycle(i, freq, dir);
                delay(10);
            }
            put("End foward 100-0\n\r");
            delay(1);
            // 0 to 100% duty cycle in reverse direction
            put("Starting reverse 0-100\n\r");
            for (i=0; i<100; i++){
                if(i%10 == 0)
                {
                    putnumU(i);
                    put("\r\n");
                }
                SetDutyCycle(i, freq, 1);
                delay(10);
            }
            put("Starting reverse 0-100\n\r");
            delay(10);
            // 100% down to 0% duty cycle in the reverse direction
            put("Starting reverse 100-0\n\r");
            for (i=100; i>=0; i--){
                if(i%10 == 0)
                {
                    putnumU(i);
                    put("\r\n");
                }
                SetDutyCycle(i, freq, 1);
                delay(10);
            }
            put("Starting reverse 100-0\n\r");
            delay(1);
        }
    }
    else
    {
        // enable clocks on port d
        
        // Configure the signal multiplexer for port d gpio
        
        // configure the gpio pins for output
        
        int forward=1;
        int phase = 0;
        
        while(1){
            // turn off all coils, set GPIO pins to 0
            
            
            // set one pin at a time to high
            if(forward){
                if(phase == 0){ //turn on coil A
                    phase++;
                }
                else if(phase == 1)
                {
                    // turn on coil B
                    phase++;
                }
                else if (phase == 2)
                {
                    // phase on coil C
                    phase++;
                }
                else
                {
                    // turn on coil D
                    phase=0;
                }
            }
            else
            {
                //reverse
                if(phase==0){
                    //turn on coil D
                    phase++;
                }
                else if(phase == 1)
                {
                    // turn on coil C
                    phase++;
                }
                else if (phase == 2)
                {
                    // phase on coil B
                    phase++;
                }
                else
                {
                    // turn on coil A
                    phase=0;
                }
            }
            delay(10);
        }
    }
    return 0;
}


/**
 * Waits for a delay (in milliseconds)
 * 
 * del - The delay in milliseconds
 */
void delay(int del){
	int i;
	for (i=0; i<del*50000; i++){
		// Do nothing
	}
}

void initialize()
{
	// Initialize UART
	uart_init();	
	
	// Initialize the FlexTimer
	InitPWM();
}
