/************************************************************
File: pwm.h

Description: Header File for the pwm module

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/


#ifndef PWM_H_
#define PWM_H_


/************************************************************
 Global Scope Function prototypes
************************************************************/
void InitPWM0( void );
void InitPWM1( void );
void PWM_ISR( void );
void SetDutyCyclePWM0( double DutyCycle, unsigned int Frequency, int dir );
void SetDutyCyclePWM1( double DutyCycle, unsigned int Frequency );



#endif /* PWM_H_ */

/* End of File */
