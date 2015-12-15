/* 
* AD5689DAC.h
*
* Copyright Runcible Software Pty Ltd 2015
*
* Created: 7/5/2015 7:30:05 PM
* Author: tom.biskupic@gmail.com
*/

#ifndef __AD5689DAC_H__
#define __AD5689DAC_H__

#include "SPIDevice.h"
/*
Implements an Abstraction of the Analog Devices AD5689 DAC.
*/
class AD5689DAC : public SPIDevice
{
public:

    enum Channel
    {
        DAC_A = 0x1,
        DAC_B = 0x8,
        DAC_BOTH = 0x9
    };
            
	AD5689DAC(int selectPin);
    
    /*
    Sets the output of the DAC channel specified to the value specified.
    */
    void setOutput( const uint16_t value, const Channel channel);
    
    /*
    Initiates a software reset
    */
    void reset();
    
protected:

};

#endif //__AD5689DAC_H__
