/* 
* AD7705ADC.h
*
* Copyright Runcible Software Pty Ltd 2015
*
* Created: 7/5/2015 7:30:05 PM
* Author: tom.biskupic@gmail.com
*/

#include "SPIDevice.h"

#ifndef __AD7705ADC_H__
#define __AD7705ADC_H__

class AD7705ADC : public SPIDevice
{
public:
    enum Channel
    {
        CHANNEL_0=0,
        CHANNEL_1=1,
        CHANNEL_0_0=2,
        CHANNEL_0_1=3
    };
    
    /*
    Constructs the interface to the ADC device. The SS ping is used to activate
    the device's select line. ]
    */
    AD7705ADC(int ssPin,int dataReadyPin);
    
    /*
    Specifies the clock speeds supported by the part
    */
    enum ClockFrequency
    {
        CLOCK_2_4676,
        CLOCK_4_9152,
        CLOCK_2,
        CLOCK_1
    };
    
    /*
    Defines the different modes the ADC can be in
    */
    enum ADCMode
    {
        MODE_NORMAL=0,
        MODE_SELF_CAL=1,
        MODE_ZERO_SCALE_CAL=2,
        MODE_FULL_SCALE_CAL=3
    };

    enum Gain
    {
        GAIN_1=0,
        GAIN_2=1,
        GAIN_4=2,
        GAIN_8=3,
        GAIN_16=4,
        GAIN_32=5,
        GAIN_64=6,
        GAIN_128=7
    };
    
    enum Polarity
    {
        UNIPOLAR=1,
        BIPOLAR=0
    };
    
    /*
    Resets the device back to its initial state
    */
    void reset();
    
    /*
    This sets up the clock register. The settings are made based on
    the clock crystal being used. See page 19 of the datasheet
    for more details.
    See filterSelects is a number from 0-3 and sets the filter for 
    different maximal update rates. See Page 19 for this also.
    */
    void setClockRegister(
        ClockFrequency	clockFrequency,
        const uint8_t	filterSelect);
        
    /*
    Configures the ADC's operation. The mode is used to choose between normal
    operation and one of the calibration modes. The gain selection allows the
    part to configure an internal amplifier for low input values. The unipolar
    mode selects the input range and restricts AIN+ to be greater than AIN-. 
    Buffer control enables or disables the input buffer. FilterSync is to put the
    filters in hold state so the part can be synchronised with some process.
    */
    void setSetupRegister(
        const Channel   channel,
        const ADCMode	adcMode,
        const Gain		gain,
        const Polarity	polarity,
        const bool		buffered,
        const bool		filterSync );
        
    /*
    Reads the current value from the channel specified as a bipolar value
    */
    uint16_t getValue( const Channel channel );
    
    uint32_t readOffsetRegister(const Channel channel);
    
    uint32_t readFullScaleRegister(const Channel channel);
    
protected:

    int m_dataReadyPin;
    
    enum ReadWrite
    {
        WRITE=0,
        READ=1	
    };	
    
    void writeToCommsRegister(int register, const ReadWrite readWrite, const bool standby, const Channel channel);
    
    bool dataReady(const Channel channel);
};

#endif //__AD7705ADC_H__
