/* 
* AD7705ADC.cpp
*
* Created: 7/08/2015 11:48:15 PM
* Author: tom
*/

#include "AD7705ADC.h"
#include "BitTools.h"
#include <avr/io.h>
#include "stdio.h"

namespace
{
	//
	//	Communication register bits
	//
	const int COM_DRDY_BIT=7;
	const int COM_RS_BIT=4;
	const int COM_RW_BIT=3;
	const int COM_STBY_BIT=2;
	const int COM_CH1_BIT=1;
	const int COM_CH0_BIT=0;
	
	enum RegisterSelection
	{
		COMMS_REGISTER=0,
		SETUP_REGISTER=1,
		CLOCK_REGISTER=2,
		DATA_REGISTER=3,
		TEST_REGISTER=4,
		NO_OP_REGISTER=5,
		OFFSET_REGISTER=6,
		GAIN_REGISTER=7	
	};
	
	//
	//	Setup register bits
	//
	const int SETUP_MODE_BIT=6;
	const int SETUP_GAIN_BIT=3;
	const int SETUP_BIPOLAR_MODE_BIT=2;
	const int SETUP_BUFFER_CTRL_BIT=1;
	const int SETUP_FSYNC_BIT=0;
	
	//
	//	Clock Register
	//
	const int CLOCK_CLKDIS_BIT=4;
	const int CLOCK_CLKDIV_BIT=3;
	const int CLOCK_CLK_BIT=2;
	const int CLOCK_FS=0;
}

AD7705ADC::AD7705ADC(int ssPin,int dataReadyPin) : SPIDevice(true,ssPin),m_dataReadyPin(dataReadyPin)
{
	setClockPolarity(SPIDevice::CLOCK_POLARITY_FALLING_LEADS);
	setClockPhase(SPIDevice::CLOCK_PHASE_SAMPLE_ON_TRAILING);
    setDoubleSpeedModeEnabled(true);
    setClockRate(SPIDevice::RATE_DIV_DBL_2);
	setBitOrdering(SPIDevice::MSB_FIRST);
    
    //
    //  Set the dataready pin as an input
    //
    DDRD &= ~(1<<m_dataReadyPin);
}

void AD7705ADC::reset()
{
    setup();
    setupSelectLine();
    for(int i=0;i<4;i++)
    {
        writeByte(0xff);        
    }
    clearSelectLine();
}

void AD7705ADC::setClockRegister(
	ClockFrequency	clockFrequency,
	const uint8_t	filterSelect )
{
	bool clockBit=false;
	bool clkDivBit=false;
	
	switch(clockFrequency)
	{
		case CLOCK_1:
		{
			clockBit=false;
			clkDivBit=false;
			break;
		}
		case CLOCK_2:
		{
			clockBit=false;
			clkDivBit=true;
			break;
		}
		case CLOCK_4_9152:
		{
			clockBit=true;
			clkDivBit=true;
			break;
		}
		case CLOCK_2_4676:
		{
			clockBit=true;
			clkDivBit=false;
			break;
		}
	}
	
	uint8_t clockReg =	(BitTools::boolToBit(clkDivBit) << CLOCK_CLKDIV_BIT) 
						| 
						(BitTools::boolToBit(clockBit) << CLOCK_CLK_BIT )
						|
						(filterSelect & 0x3 );
	
    writeToCommsRegister(CLOCK_REGISTER,WRITE,false,(Channel)0);

	setup();
	setupSelectLine();
	writeByte(clockReg);
	clearSelectLine();
    
    printf("Wrote 0x%x to clock register\r\n",clockReg);
}

bool AD7705ADC::dataReady(const Channel channel)
{
    //writeToCommsRegister(COMMS_REGISTER,READ,false,channel);
    //setup();
    //setupSelectLine();
    //uint8_t value = readByte();
    //clearSelectLine();
      //
    //return (value & (1<<COM_DRDY_BIT)) == 0;
    
    return (PIND & (1<<m_dataReadyPin)) == 0;
}

uint16_t AD7705ADC::getValue( const Channel channel )
{
	writeToCommsRegister(DATA_REGISTER,READ,false,channel);

    //
    //  Wait for data ready to go low
    //
    while( !dataReady(channel) );

	setup();
	setupSelectLine();	
	uint16_t value = readByte() << 8;
	value |= readByte();
	clearSelectLine();
      
	return value;
}

uint32_t AD7705ADC::readOffsetRegister(const Channel channel)
{
	writeToCommsRegister(OFFSET_REGISTER,READ,false,channel);
    
	setup();
	setupSelectLine();
	uint32_t value = ((uint32_t)readByte()) << 16;
	value |= ((uint32_t)readByte()) << 8;
    value |= readByte();
	clearSelectLine();
    
    return value;    
}

uint32_t AD7705ADC::readFullScaleRegister(const Channel channel)
{
    writeToCommsRegister(OFFSET_REGISTER,READ,false,channel);
    	
    setup();
    setupSelectLine();
    uint32_t value = ((uint32_t)readByte()) << 16;
    value |= ((uint32_t)readByte()) << 8;
    value |= readByte();
    clearSelectLine();
    	
    return value;
}

void AD7705ADC::setSetupRegister(
    const Channel   channel,
	const ADCMode	adcMode,
	const Gain		gain,
	const Polarity  polarity,
	const bool		buffered,
	const bool		filterSync )
{
	uint8_t setupRegValue =	( (adcMode & 0x3) << SETUP_MODE_BIT)
							|
							( (gain & 0x7) << SETUP_GAIN_BIT)
							|
							( polarity << SETUP_BIPOLAR_MODE_BIT )
							|
							( BitTools::boolToBit(buffered) << SETUP_BUFFER_CTRL_BIT )
							|
							BitTools::boolToBit(filterSync);

	writeToCommsRegister(SETUP_REGISTER,WRITE,false,channel);

	setup();
	setupSelectLine();
	writeByte(setupRegValue);
	clearSelectLine();	
    
    //
    //  Wait for data ready to go low
    //
    while( !dataReady(channel) );
    
    printf("Wrote 0x%x to setup register\r\n",setupRegValue);
}

void AD7705ADC::writeToCommsRegister(
	int				registerID,
	const ReadWrite readWrite,
    const bool      standby, 
	const Channel	channel)
{
	uint8_t value = ((registerID & 0x7) << COM_RS_BIT)
	                |
	                ( readWrite << COM_RW_BIT )
	                |
	                ( BitTools::boolToBit(standby) << COM_STBY_BIT )
	                |
	                (0x3 & channel);

    //printf("Writing 0x%X to Comms register\r\n",value);
    	    
    setup();
    setupSelectLine();
	writeByte(value);
	clearSelectLine();
}
