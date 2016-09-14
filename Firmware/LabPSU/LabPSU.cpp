/* 
* LabPSU.cpp
*
* Created: 19/07/2015 2:15:23 AM
* Author: tom
*/

#define F_CPU 16000000

#include "LabPSU.h"
#include "LineariserStorage.h"
#include "math.h"
#include "stdio.h"
#include <avr/io.h>
#include "util/delay.h"
#include <math.h>

namespace
{
    const AD5689DAC::Channel VOLTAGE_DAC_CHANNEL = AD5689DAC::DAC_A;	
    const AD5689DAC::Channel CURRENT_DAC_CHANNEL = AD5689DAC::DAC_B;

    const AD7705ADC::Channel VOLTAGE_ADC_CHANNEL = AD7705ADC::CHANNEL_0;
    const AD7705ADC::Channel CURRENT_ADC_CHANNEL = AD7705ADC::CHANNEL_1;
    
    //
    //	These constants define the max current limit when we are in the
    //	series and parallel configuration for the transformer taps
    //
    const float SERIES_TAP_MAX_CURRENT_LIMIT=3.0;
    const float PARA_TAP_MAX_CURRENT_LIMIT=4.8;
    
    //
    //  These are both on the D port
    //
    const int BIAS_VOLTAGE_CTRL_PIN = 6;
    const int RELAY_CONTROL_PIN = 7;
    const int CONST_CURRENT_MODE_PIN=5;
    
    const int DAC_SS_PIN=1;
    const int ADC_SS_PIN=0;
    const int ADC_DATA_READY_PIN=4; // PORTD
    
    //
    //	This is the highest voltage we will go to before switching to the
    //	series configuration of the transformer taps. In series mode the current
    //	limit drops also.
    //
    const float THIRTY_VOLT_MODE_THRESHOLD=15.0;
}

LabPSU::LabPSU() :	m_setVoltage(0.0f),
                    m_currentLimit(0.0f),
                    m_outputEnabled(false),
                    m_inThirtyVoltMode(true),
                    m_voltsLinearizer(),
                    m_voltsADCLinearizer(),
                    m_ampsDACLinearizer(),
                    m_ampsADCLinearizer(),
                    m_ampsPerStep(4.9958/65535.0),
                    m_dac(DAC_SS_PIN),
                    m_adc(ADC_SS_PIN,ADC_DATA_READY_PIN)
{
    //
    //  Set the relay and bias control as outputs
    //
    DDRD |= (1 << BIAS_VOLTAGE_CTRL_PIN) | ( 1<< RELAY_CONTROL_PIN );
    
    //
    //	Set the constant current mode pin as an input
    //
    DDRD &= ~(1 << CONST_CURRENT_MODE_PIN);
	
}

void LabPSU::init()
{
	LineariserStorage::loadLineariser(LineariserStorage::VOLTAGE_DAC,m_voltsLinearizer);
	LineariserStorage::loadLineariser(LineariserStorage::VOLTAGE_ADC,m_voltsADCLinearizer);
	LineariserStorage::loadLineariser(LineariserStorage::CURRENT_DAC,m_ampsDACLinearizer);
	LineariserStorage::loadLineariser(LineariserStorage::CURRENT_ADC,m_ampsADCLinearizer);

    setThirtyVoltMode(false);
    //printf("Doing a DAC Reset\r\n");
    m_dac.reset();

    setOutputVoltageLimit(m_setVoltage);
    setCurrentLimit(m_currentLimit);
    
    //printf("Doing a ADC Init\r\n");   
    initADC();    
}

void LabPSU::setThirtyVoltMode(const bool enable)
{
    if ( m_inThirtyVoltMode != enable )
    {
        if ( enable )
        {
            PORTD |= ( 1 << RELAY_CONTROL_PIN);
            PORTD &= ~( 1 << BIAS_VOLTAGE_CTRL_PIN );
        }
        else
        {
            PORTD |= ( 1 << BIAS_VOLTAGE_CTRL_PIN );
            PORTD &= ~( 1 << RELAY_CONTROL_PIN);           
        }
        
        m_inThirtyVoltMode = enable;
    }
}

void LabPSU::setOutputVoltageLimit(const float value)
{
    if ( m_setVoltage != value )
    {
        m_setVoltage = value;
        setThirtyVoltMode(m_setVoltage > THIRTY_VOLT_MODE_THRESHOLD);

        //
        //  Adjust the current limit in case we switched from parallel to series
        //  mode
        //
        float maxCurrentLimit = getMaxCurrentLimit();
        if ( m_currentLimit > maxCurrentLimit )
        {
            setCurrentLimit(maxCurrentLimit);
        }

        if ( m_outputEnabled )
        {
            setVoltageDAC(m_setVoltage);
        }
    }
}

void LabPSU::setVoltageDACCount(unsigned int count)
{
    m_outputEnabled=true;
    m_setVoltage = (30.0/(float)0xffff) * (float)count;
    setThirtyVoltMode(m_setVoltage > THIRTY_VOLT_MODE_THRESHOLD);
    
    //
    //  Adjust the current limit in case we switched from parallel to series
    //  mode
    //
    float maxCurrentLimit = getMaxCurrentLimit();
    if ( m_currentLimit > maxCurrentLimit )
    {
        setCurrentLimit(maxCurrentLimit);
    }
    m_dac.setOutput(count,VOLTAGE_DAC_CHANNEL);
}

void LabPSU::setVoltageDAC(const float voltage)
{
    uint16_t count = m_voltsLinearizer.valueToCode(voltage);

    m_dac.setOutput(count,VOLTAGE_DAC_CHANNEL);
}

const float LabPSU::getOutputVoltageLimit() const
{
    return m_setVoltage;
}
    
float LabPSU::getOutputVoltage() const
{
    uint16_t code = getOutputVoltageCount();
    return m_voltsADCLinearizer.codeToValue(code);
}

uint16_t LabPSU::getOutputVoltageCount() const
{
    return readADC(VOLTAGE_ADC_CHANNEL);
}

void LabPSU::setCurrentLimit(const float value)
{
    if ( m_currentLimit != value )
    {
        float maxCurrentLimit = getMaxCurrentLimit();
        if ( value > maxCurrentLimit )
        {
            m_currentLimit = maxCurrentLimit;    
        }
        else
        {
             m_currentLimit = value;
        }
       
        if ( m_outputEnabled )
        {
            setCurrentDAC(m_currentLimit);
        }
    }
}

void LabPSU::setCurrentDACCount(unsigned int count)
{
    m_currentLimit = (5.0/(float)0xffff)*(float)count;
    m_dac.setOutput(count,CURRENT_DAC_CHANNEL);  
}

void LabPSU::setCurrentDAC(const float value)
{
    uint16_t count = m_ampsDACLinearizer.valueToCode(value);
    m_dac.setOutput(count,CURRENT_DAC_CHANNEL);
}

const float LabPSU::getCurrentLimit() const
{
    return m_currentLimit;
}

const float LabPSU::getMaxCurrentLimit() const
{
    if ( m_inThirtyVoltMode )
    {
        return SERIES_TAP_MAX_CURRENT_LIMIT;
    }	
    else
    {
        return PARA_TAP_MAX_CURRENT_LIMIT;
    }
}

const float LabPSU::getOutputCurrent() const
{
    uint16_t code = readADC(CURRENT_ADC_CHANNEL);
    return m_ampsADCLinearizer.codeToValue(code);
}

unsigned int LabPSU::getOutputCurrentCount() const
{
    return readADC(CURRENT_ADC_CHANNEL);    
}

void LabPSU::enableOutput(const bool enable )
{
    if ( enable != m_outputEnabled )
    {
        m_outputEnabled = enable;
    
        if ( m_outputEnabled )
        {
            setVoltageDAC(m_setVoltage);
            setCurrentDAC(m_currentLimit);
        }
        else
        {
            setVoltageDAC(0.0f);
            setCurrentDAC(0.0f);
        }
    }
}
    
const bool LabPSU::isOutputEnabled() const
{
    return m_outputEnabled;
}

bool LabPSU::isInCurrentLimit()
{
    if ( m_outputEnabled )
    {
        return ((PIND & (1<<CONST_CURRENT_MODE_PIN)) != 0);
    }
    else
    {
        return false;
    }        
}

void LabPSU::initADC() const
{
    //
    //	We run the ADC in bipolar mode so we can get the full 0..5V range.
    //	The AIN- is tied to the reference so the input is a bipolar number
    //	relative to that.
    //
    //	Because of the bipolar requirement we can't use gain.
    //	I am using a 2MHz crystal which means clockdiv is 1. 
    //	An update rate of 50Hz or less is fine so filter select is 0
    //
    m_adc.reset();
    m_adc.setClockRegister(AD7705ADC::CLOCK_2,0);

    m_adc.setSetupRegister(
        VOLTAGE_ADC_CHANNEL,                    //  Channel
        AD7705ADC::AD7705ADC::MODE_SELF_CAL,    //	CAL mode
        AD7705ADC::GAIN_1,					    //	Gain = 1
        AD7705ADC::BIPOLAR,				        //	Polarity
        false,								    //	Not buffered
        false);								    //	F Sync off
    
    m_adc.reset();
        
    m_adc.setSetupRegister(
        CURRENT_ADC_CHANNEL,                    //  Channel
        AD7705ADC::AD7705ADC::MODE_SELF_CAL,    //	CAL mode
        AD7705ADC::GAIN_1,					    //	Gain = 1
        AD7705ADC::BIPOLAR,                     //	Polarity
        false,								    //	Not buffered
        false);								    //	F Sync off
}

uint16_t LabPSU::readADC( const AD7705ADC::Channel channel ) const
{
    m_adc.reset();
    
    uint16_t value = m_adc.getValue(channel);
    
    //printf("Raw ADC Value Read %u from ADC channel %d\r\n",value,channel);
    
    return value;
}

void LabPSU::calibrateVoltageADCZeroOffset()
{
    float oldSetVoltage = m_setVoltage;
    
    setOutputVoltageLimit(0.0f);
    _delay_ms(5000.0);
    
    m_adc.setSetupRegister(
        VOLTAGE_ADC_CHANNEL,                         //  Channel
        AD7705ADC::AD7705ADC::MODE_ZERO_SCALE_CAL,  //	CAL mode
        AD7705ADC::GAIN_1,					        //	Gain = 1
        AD7705ADC::BIPOLAR,						    //	Polarity
        false,								        //	Not buffered
        false);								        //	F Sync off
        
    setOutputVoltageLimit(oldSetVoltage);
}    
    
void LabPSU::calibrateVoltageADCFullScale()
{
    float oldSetVoltage = m_setVoltage;
    
    setOutputVoltageLimit(30.0f);
    _delay_ms(5000.0);
    
    m_adc.setSetupRegister(
        VOLTAGE_ADC_CHANNEL,                        //  Channel
        AD7705ADC::AD7705ADC::MODE_FULL_SCALE_CAL,  //	CAL mode
        AD7705ADC::GAIN_1,					        //	Gain = 1
        AD7705ADC::BIPOLAR,						    //	Polarity
        false,								        //	Not buffered
        false);								        //	F Sync off
    
    setOutputVoltageLimit(oldSetVoltage);    
}

uint32_t LabPSU::getVoltageZeroCalibrationCoefficient()
{
    return m_adc.readOffsetRegister(VOLTAGE_ADC_CHANNEL);
}

uint32_t LabPSU::getVoltageFullScaleCalibrationCoefficient()
{
    return m_adc.readFullScaleRegister(VOLTAGE_ADC_CHANNEL);
}

