/*
 * LabPSU.cpp
 *
 * Created: 7/1/2015 9:06:16 PM
 *  Author: tom
 */ 

#define F_CPU 20000000

#include <avr/io.h>

#include <util/delay.h>

//#include "AD5689DAC.h"
#include "SerialSetup.h"
#include "CommandInterpreter.h"
#include "LabPSU.h"

#include "stdio.h"
#include "string.h"
#include "util/delay.h"

int main(void)
{
	SerialSetup serialSetup;
    LabPSU      psu;
    CommandInterpreter  commandInterpreter(&psu);
    
    _delay_ms(500.0);
    psu.init();
    
    psu.setOutputVoltageLimit(10.0f);
    psu.setCurrentLimit(1.0f);
    
    printf("Tom's Lab Power Supply\r\n");
    
    int i=1;
    while(1)
    {
        _delay_ms(100.0);
        //
        //printf("%d\r\n",i++);
//
        //psu.setOutputVoltageLimit(10.0f);

        commandInterpreter.processNextCommand();
    }        
}
