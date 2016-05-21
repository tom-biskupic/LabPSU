/* 
* AD5689DAC.cpp
*
* Created: 7/5/2015 7:30:05 PM
* Author: tom
*/


#include "AD5689DAC.h"
#include "stdio.h"

namespace
{
    enum Command
    {
        COM_NO_OP=0,
        COM_WRITE_TO_INPUT_REGISTER=1,
        COM_UPDATE_DAC_WITH_INPUT=2,
        COM_WRITE_AND_UPDATE_DAC=3,
        COM_POWER_DOWN_DAC=4,
        COM_LDAC_MASK_REGISTER=5,
        COM_SOFTWARE_RESET=6,
        COM_REFERENCE_SETUP=7,
        COM_ENABLE_DAISY_CHAIN=8,
        COM_SETUP_READBACK_REG=9
    };    
}

AD5689DAC::AD5689DAC(int selectPin) : SPIDevice(true,selectPin)
{
    setClockPolarity(SPIDevice::CLOCK_POLARITY_RISING_LEADS);
    setClockPhase(SPIDevice::CLOCK_PHASE_SAMPLE_ON_TRAILING);
    setBitOrdering(SPIDevice::MSB_FIRST);
    setClockRate(SPIDevice::RATE_DIV_128);
}

void AD5689DAC::setOutput( const uint16_t value, const Channel channel)
{
    setup();
    setupSelectLine();
    
    //uint8_t commandAndAddress = (COM_WRITE_TO_INPUT_REGISTER << 4) | channel;
    uint8_t commandAndAddress = (COM_WRITE_AND_UPDATE_DAC << 4) | channel;
    writeByte(commandAndAddress);
    writeByte(value >> 8);
    writeByte(value & 0xff);
    clearSelectLine();
}

void AD5689DAC::reset()
{
    setup();
    setupSelectLine();
    
    uint8_t commandAndAddress = (COM_SOFTWARE_RESET << 4) | DAC_BOTH;
    writeByte(commandAndAddress);
    writeByte(0);
    writeByte(0);
    clearSelectLine();    
}
