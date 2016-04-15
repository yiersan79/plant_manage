/*
 * File:		io.c
 * Purpose:		Serial Input/Output routines
 *
 * Notes:       TERMINAL_PORT defined in <board>.h
 */

#include "common.h"
#include "uart.h"

/********************************************************************/

uint_8 *fp;
char
in_char (void)
{
  if (LANDZO_PORT == 0)
    return uart_pendchar (UART0,fp);
  else if (LANDZO_PORT == 1)
    return uart_pendchar (UART1,fp);
  else
    return uart_pendchar (UART2,fp);

}
/********************************************************************/
void
out_char (char ch)
{
  if (LANDZO_PORT == 0)
    uart_putchar(UART0,ch);
  else if (LANDZO_PORT == 1)
    uart_putchar(UART1,ch);
  else
    uart_putchar(UART2,ch);

}

/********************************************************************/
/********************************************************************/
