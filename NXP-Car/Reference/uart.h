/************************************************************
File: uart.h

Description: Header File for the uart module

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/


#ifndef UART_H
#define UART_H


/************************************************************
 Library and Header File Includes
************************************************************/
#include <stdint.h>



/************************************************************
 Global Scope Function prototypes
************************************************************/
void uart_init( void );
uint8_t uart_getchar( void );
void uart_putchar( char ch );
void uart_put( char *ptr_str );



#endif /* UART_H_ */

/* End of File */
