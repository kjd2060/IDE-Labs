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

#ifndef STEPPER_CONSTANTS

#define STEPPER_ALL_PINS ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3))
#define STEPPER_PIN0 (1 << 0)
#define STEPPER_PIN1 (1 << 1)
#define STEPPER_PIN2 (1 << 2)
#define STEPPER_PIN3 (1 << 3)

#endif

void initialize();
void en_interrupts();
void delay();

int main(void)
{
	char c = 48;
  int i=0;
  int dcMotorSelect = 0;
	int servoMotorSelect = 1;
	uint16_t dc = 0;
  uint16_t freq = 10000; /* Frequency = 10 kHz */
	uint16_t freq_servo = 50;
  uint16_t dir = 0;
	// Initialize UART and PWM
	initialize();

	// Print welcome over serial
	put("Running... \n\r");
	
	//Step 3
    
	// for(;;) ;  //then loop forever
	
	//Step 9


					
				put("DC Motor Goin\n\r");
				SetDutyCycle(50, 300, dir, dcMotorSelect);
				// SetDutyCycle(20, freq_servo, dir, servoMotorSelect);
    /*
    else
    {
				int forward=1;
        int phase = 0;
        int i = 0;
				int del = 10;
			
        // enable clocks on port d
        SIM_SCGC5 = SIM_SCGC5_PORTD_MASK;  
			
        // Configure the signal multiplexer for port d gpio
        PORTD_PCR0 = PORT_PCR_MUX(1);
				PORTD_PCR1 = PORT_PCR_MUX(1);
				PORTD_PCR2 = PORT_PCR_MUX(1);
				PORTD_PCR3 = PORT_PCR_MUX(1);
			
			
        // configure the gpio pins for output 
				GPIOD_PDDR = STEPPER_ALL_PINS; // 0xF = 15 = 1111
        
				
        while(i < (del * 15)){
            // turn off all coils, set GPIO pins to 0
            GPIOD_PSOR = STEPPER_ALL_PINS;
            
            // set one pin at a time to high
            if(forward){
                if(phase == 0){ //turn on coil A pin 0
									  GPIOD_PCOR = STEPPER_PIN0;
                    phase++;
										put("Phase 0\n\r");
                }
                else if(phase == 1)
                {
                    // turn on coil B pin 1
										GPIOD_PCOR = STEPPER_PIN1;
                    phase++;
									put("Phase 1\n\r");
                }
                else if (phase == 2)
                {
                    // phase on coil C pin 2
										GPIOD_PCOR = STEPPER_PIN2;
                    phase++;									
										put("Phase 2\n\r");
                }
                else
                {
                    // turn on coil D pin 3
										GPIOD_PCOR = STEPPER_PIN3;
                    phase=0;									
										put("Phase 3\n\r");
                }
            }
            else
            {
                //reverse
                if(phase==0){
                    //turn on coil D
										GPIOD_PCOR = STEPPER_PIN3;
                    phase++;
									put("Phase 0\n\r");
                }
                else if(phase == 1)
                {
                    // turn on coil C
										GPIOD_PCOR = STEPPER_PIN2;
                    phase++;
									  put("Phase 1\n\r");
                }
                else if (phase == 2)
                {
                    // phase on coil B
										GPIOD_PCOR = STEPPER_PIN1;
                    phase++;
									  put("Phase 2\n\r");
                }
                else
                {
                    // turn on coil A
										GPIOD_PCOR = STEPPER_PIN0;
                    phase=0;
									  put("Phase 3\n\r");
                }								
            }
						putnumU(i);
						put("\n\r");
						i++;
						delay(1);
        }
    }
		*/
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
