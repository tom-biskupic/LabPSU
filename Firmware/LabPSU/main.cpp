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

int freeRam () 
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int main(void)
{
    SerialSetup serialSetup;
    //printf("Tom's Lab Power Supply\r\n");
    //printf("Free ram is %d\r\n",freeRam());
    
    LabPSU      psu;
    //printf("Free ram is %d\r\n",freeRam());

    CommandInterpreter  commandInterpreter(&psu);
    //printf("Free ram is %d\r\n",freeRam());
    
    _delay_ms(500.0);
    //printf("Calling Init\r\n");

    psu.init();
    //printf("Init Complete\r\n");
    psu.setOutputVoltageLimit(10.0f);
    psu.setCurrentLimit(1.0f);
    
    //int i=0;
    
    //while(1)
    //{
    //    printf("1 fish %d fish\r\n",i++);
    //}
    
    
    while(1)
    {
        commandInterpreter.processNextCommand();
    }        
}

extern "C" void __cxa_pure_virtual()
{
	printf("Pure Virtual called! I'm outa here!\r\n");
}