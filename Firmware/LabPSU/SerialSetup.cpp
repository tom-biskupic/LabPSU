/* 
* SerialSetup.cpp
*
* Created: 19/07/2015 10:51:45 PM
* Author: tom
*/

#define F_CPU 20000000

#include "SerialSetup.h"
#include <avr/interrupt.h>

extern "C"
{
    #include "uart.h"
}
#include "string.h"

int serialWrite(char c, FILE *fp)
{
    uart_putc(c);
    return 1;
}

int serialRead(FILE *fp)
{
    return uart_getc();
}

// default constructor
SerialSetup::SerialSetup()
{
    memset(&m_uart_io,sizeof(m_uart_io),0);
    fdev_setup_stream(&m_uart_io,serialWrite,serialRead,_FDEV_SETUP_RW);
        
    //uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(9600L,20000000L)/8);
    uart_init(15);
        
    sei();
        
    stdout = &m_uart_io;
    stdin  = &m_uart_io;
}

