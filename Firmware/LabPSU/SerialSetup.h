/* 
* SerialSetup.h
*
* Created: 19/07/2015 10:51:45 PM
* Author: tom
*/


#ifndef __SERIALSETUP_H__
#define __SERIALSETUP_H__

#include "stdio.h"

/*
Just creating this class configures stdin/stdout to communicate
via the serial port.
*/
class SerialSetup
{
public:
    SerialSetup();
private:
    SerialSetup( const SerialSetup &c );
    SerialSetup& operator=( const SerialSetup &c );
    
    FILE m_uart_io;
};

#endif //__SERIALSETUP_H__
