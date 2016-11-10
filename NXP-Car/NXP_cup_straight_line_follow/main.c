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
#include "pwm.h"
#include "camera.h"

#ifndef STEPPER_CONSTANTS

#define STEPPER_ALL_PINS ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3))
#define STEPPER_PIN0 (1 << 0)
#define STEPPER_PIN1 (1 << 1)
#define STEPPER_PIN2 (1 << 2)
#define STEPPER_PIN3 (1 << 3)

#endif
    int dcMotorSelect = 0;
    int dcMotorSelect2 = 2;
    int servoMotorSelect = 1;
    uint16_t freq = 10000; /* Frequency = 10 kHz */
	uint16_t freq_servo = 50;
    uint16_t dir = 0;
    uint16_t camera[128];
    float servoPosition = 7.8;

void initialize(void);
void en_interrupts(void);
void delay(int del);
void camera_follow(void);

int main(void)
{
	// Initialize UART and PWM
	initialize();

	// Print welcome over serial
	put("Running... \n\r");
	
	//Step 3
    
    
    
    pwm_set_duty_cycle(25, freq, dir, dcMotorSelect);
    pwm_set_duty_cycle(25, freq, dir, dcMotorSelect2);
    pwm_set_duty_cycle(servoPosition, freq_servo, dir, servoMotorSelect);
	for(;;)  //then loop forever
	{
        camera_test();
        camera_read(camera);
        
        camera_follow();
        
        pwm_set_duty_cycle(servoPosition, freq_servo, dir, servoMotorSelect);
        
        delay(10);
    }
    
	//Step 9

    return 0;
}

void camera_follow(void)
{
    int i;
    int sumL = 0;
    int sumR = 0;
    int diff = 0;
    for (i = 31; i < 64 ; i++)
    {
        sumL += camera[i];        
    }
    for (i = 64; i < 96; i++)
    {
        sumR += camera[i];        
    }
    diff = sumR - sumL;
    if (0 > diff)
    {
        //turn left
        servoPosition -= 0.5;
    } else if (0 < diff)
    {
        //turn right
        servoPosition += 0.5;
    }
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

void initialize(void)
{
	// Initialize UART
	uart_init();	
	put("UART initialized\r\n");
	// Initialize the FlexTimer
	pwm_init();
	put("PWM initialized\r\n");
	
	camera_init();
	put("Camerrrrra initialized\r\n");
}
