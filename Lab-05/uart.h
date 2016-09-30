#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include "MK64F12.h"

void uart_put(char *ptr_str);
void uart_init(void);
uint8_t uart_getchar(void);
void uart_putchar(char ch);

#endif /*UART_H_*/
