/************************************************************
File: uart.c

Description: UART Device Driver

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/


/************************************************************
 Library and Header File Includes
************************************************************/
#include "MK64F12.h"
#include "uart.h"



/************************************************************
 File Scope Macros and Definitions
************************************************************/
// Default baud rate 
#define BAUD_RATE 9600

// Default system clock (see DEFAULT_SYSTEM_CLOCK in 
// system_MK64F12.c)
#define SYS_CLOCK 20485760



/************************************************************
The initialize function for the uart0 module

Parameters:
N/A

Returns:
N/A
************************************************************/
void uart_init( void )
{

	//define variables for baud rate and baud rate fine adjust
	uint16_t ubd, brfa;
	uint8_t bdh;
	
	//Enable clock for UART
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	 
	//Configure the port control register to alternative 3 (which is UART mode for K64)
	PORTB_PCR16 |= PORT_PCR_MUX(3);
	PORTB_PCR17 |= PORT_PCR_MUX(3);
		
	/*Configure the UART for establishing serial communication*/

	//Disable transmitter and receiver until proper settings are chosen for the UART module
	UART0_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK); //disable transmit and recieve

	//Select default transmission/reception settings for serial communication of UART by clearing the control register 1
	UART0_C1 = 0x00;

	//UART Baud rate is calculated by: baud rate = UART module clock / (16 × (SBR[12:0] + BRFD))
	//13 bits of SBR are shared by the 8 bits of UART0_BDL and the lower 5 bits of UART0_BDH 
	//BRFD is dependent on BRFA, refer Table 52-234 in K64 reference manual
	//BRFA is defined by the lower 4 bits of control register, UART0_C4 

	//calculate baud rate settings: ubd = UART module clock/16* baud rate
	ubd = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * 16));  

	//clear SBR bits of BDH
	UART0_BDH &= 0xE0 ;
	
	//Save non-SBR bits of BDH
	bdh = UART0_BDH & ~(UART_BDH_SBR(0x1F));
	
	//distribute this ubd in BDH and BDL
	UART0_BDH = bdh | ((ubd & 0x1F00)>>8) ;
	UART0_BDL = (uint8_t)(ubd & UART_BDL_SBR_MASK);

	//BRFD = (1/32)*BRFA 
	//make the baud rate closer to the desired value by using BRFA
	brfa = (((SYS_CLOCK*32)/(BAUD_RATE * 16)) - (ubd * 32));

	//write the value of brfa in UART0_C4
	UART0_C4 &= ~(UART_C4_BRFA_MASK);
	UART0_C4 |= UART_C4_BRFA(brfa); 
		
	//Enable transmitter and receiver of UART
	UART0_C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK); //enable transmit and recieve
}



/************************************************************
Polls the RDRF bit in the UART0 status register 1 until a
charater is received and then returns that character.

Parameters:
N/A

Returns:
The character obtained from the UART0 serial bus
************************************************************/
uint8_t uart_getchar()
{
	while ((UART0_S1 & UART_S1_RDRF_MASK) == 0)
	{
		// Wait until a character is received and there is
		// space in the receiver buffer
	}

	// Return the 8-bit data from the receiver
	return (uint8_t)UART0_D;
}



/************************************************************
Polls the TDRE bit in the UART0 status register 1 until a
transmit buffer is available and then sends the character.

Parameters:
ch - Character to be sent on UART0

Returns:
N/A
************************************************************/
void uart_putchar(char ch)
{
	while ((UART0_S1 & UART_S1_TDRE_MASK) == 0)
	{
		// Wait until the transmit buffer is available
	}
		
	// Send the character
	UART0_D = ch;
}



/************************************************************
Polls the TDRE bit in the UART0 status register 1 until a
transmit buffer is available and then sends the character.

Parameters:
ptr_str - A pointer to a character array (string) to be sent
across the UART0.

Returns:
N/A
************************************************************/
void uart_put(char *ptr_str)
{
	while(*ptr_str)
	{
		// Send a character across the serial bus
		uart_putchar(*ptr_str++);
	}
}


/* End of File */
